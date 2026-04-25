#include "include/MprpcProvider.h"
#include "include/Config.h"
#include "include/rpcheader.pb.h"
#include <Callbacks.h>
#include <EventLoop.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/callback.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/async_unary_call.h>
#include <InetAddress.h>
#include <Logger.h>
#include <muduo/TcpServer.h>
#include <string>

#undef LOG_ENABLED
#define LOG_ENABLED true

void MprpcProvider::NotifyService(::google::protobuf::Service *service) {
    /* 一个服务信息结构体 */
    ServiceInfo service_info;
    ::google::protobuf::ServiceDescriptor const *descriptor = service->GetDescriptor();
    std::string service_name = descriptor->name().data();
    /* 服务的函数个数 */
    int method_count = descriptor->method_count();
    for (auto i = 0; i < method_count; ++i) {
        /* 获取一个函数descriptor */
        ::google::protobuf::MethodDescriptor const *method_descriptor = descriptor->method(i);
        /* 获取这个函数的名称 */
        std::string method_name = method_descriptor->name().data();
        /* 将<方法名称，函数descriptor>插入到映射表中 */
        service_info._methods.insert({method_name, method_descriptor});
    }
    service_info._service = service;
    /* 插入映射表 */
    _services.insert({std::move(service_name), service_info});
}

void MprpcProvider::Run() {
    std::cout << ("===== to be Run =====\n");
    std::string serverip = Config::GetInstance().Get("rpcserverip");
    std::string serverport = Config::GetInstance().Get("rpcserverport");
    InetAddress addr(std::stoi(serverport), serverip);
    _loop = new EventLoop;
    TcpServer server(_loop, addr, "RpcServer");

    // log_info("{}:{}", serverip, serverport);
    std::cout << serverip << ":" << serverport << std::endl;

    server.setConnectionCallback(
        std::bind(&::MprpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&::MprpcProvider::OnMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);
    server.start();
    _loop->loop();
}

void MprpcProvider::OnConnection(TcpConnectionPtr const &conn) {
    if (!conn->connected()) {
        conn->shutdown();
    }
}

/**
 * @brief
     定义proto的message类型，进行序列化和反序列化
     len-RpcHeaderStr-ArgsStr
 *
 * @param conn
 * @param buffer
 */
void MprpcProvider::OnMessage(
    TcpConnectionPtr const &conn, Buffer *buffer, [[maybe_unused]] Timestamp) {
    std::string recv_buf = buffer->retrieveAllAsString();
    uint32_t header_size;
    recv_buf.copy((char *)&header_size, 4, 0);
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str)) {
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    } else {
        log_error("反序列化失败:{}", rpc_header_str);
        return;
    }
    std::string args_str = recv_buf.substr(4 + header_size);
    log_debug("======================================");
    log_debug("RpcHeaderStr:{}", rpc_header_str);
    log_debug("ServiceName:{}", service_name);
    log_debug("MethodName:{}", method_name);
    log_debug("ArgsSize:{}", args_size);
    log_debug("ArgsStr:{}", args_str);
    log_debug("======================================");

    /* 获取Service和Method对象 */
    auto it = _services.find(service_name);
    if (it == _services.end()) { /* ... */
        return;
    }

    ServiceInfo &info = it->second;
    ::google::protobuf::Service *service = info._service; // 注意：protobuf::Service*

    auto mit = info._methods.find(method_name);
    if (mit == info._methods.end()) { /* ... */
        return;
    }

    ::google::protobuf::MethodDescriptor const *method_desc = mit->second;
    if (!method_desc) {
        log_error("bad method_desc");
    }

    // 创建 request/response
    ::google::protobuf::Message *request = service->GetRequestPrototype(method_desc).New();
    ::google::protobuf::Message *response = service->GetResponsePrototype(method_desc).New();

    if (!request->ParseFromString(args_str)) {
        log_debug("Parse args from request failed:{}", args_str);
        delete request;
        delete response;
        return;
    }

    ctx *c = new ctx(request, response);

    ::google::protobuf::Closure *done
        = ::google::protobuf::NewCallback<MprpcProvider, TcpConnectionPtr const &, ctx *>(
            this, &MprpcProvider::SendRpcResponse, conn, c);

    /*
        最后一个参数 done 是一个 Closure 回调。如果传 nullptr，表示同步调用——CallMethod
        会阻塞，直到方法执行完毕，然后你才能继续处理 response。
        但如果传了回调函数，就变成了异步调用：
            CallMethod 立即返回，不阻塞
            当 RPC 方法真正执行完毕后，框架会自动调用 done->Run()
            你的 SendRpcResponse 被执行，把 response 发回客户端
     */
    service->CallMethod(method_desc, nullptr, request, response, done);
}

void MprpcProvider::SendRpcResponse(TcpConnectionPtr const &conn, ctx *c) {
    std::string reponse_str;
    if (!c->response->SerializeToString(&reponse_str)) {
        log_error("序列化失败");
        return;
    }

    conn->send(reponse_str);
    conn->shutdown();

    delete c;
}
