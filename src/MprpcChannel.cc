#include "include/MprpcChannel.h"
#include "Defer.h"
#include "rpcheader.pb.h"
#include <arpa/inet.h>
#include <Buffer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <Logger.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief rpc调用者向服务端发送请求
    header_size + service_name method_name args_size + args
 *  4                   len                          args_size
 * @param method
 * @param controller
 * @param request
 * @param response
 * @param done
 */
void MprpcChannel::CallMethod(MethodDescriptor const *method, RpcController *controller,
    Message const *request, Message *response, Closure *done) {
    log_info("CallMethod");
    ::google::protobuf::ServiceDescriptor const *descriptor = method->service();
    std::string service_name = descriptor->name().data();
    std::string method_name = method->name().data();
    std::string args_str;
    // std::cout << service_name << "\t" << method_name << "\t" << args_str << std::endl;
    int args_size = 0;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        log_error("SerializeToString failed");
        controller->SetFailed("SerializeToString failed");
        return;
    }
    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    } else {
        log_error("SerializeToString failed");
        controller->SetFailed("SerializeToString failed");
        return;
    }
    std::string send_rpc_str;
    send_rpc_str.reserve(128);
    send_rpc_str.insert(0, (char *)&header_size, 4);
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        return;
    }
    /* 清理 */
    Defer defer([clientfd]() { close(clientfd); });

    /* 绑定地址 */
    sockaddr_in addr;
    addr.sin_port = htons(8888);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    socklen_t sock_len = sizeof addr;
    if (connect(clientfd, (sockaddr *)&addr, sock_len) == -1) {
        std::cout << "error connect" << std::endl;
        controller->SetFailed("Error connect");
        return;
    }
    send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0);
    // 接受响应值
    char buf[1024] = {0};
    ssize_t len = recv(clientfd, buf, sizeof buf, 0);

    if (len == -1 || len == 0) {
        log_error("recv res:{}", len);
        controller->SetFailed("Recv Error");
        return;
    }
    std::string response_str(buf, len);
    if (!response->ParseFromString(response_str)) {
        log_error("ParseFromString failed");
        controller->SetFailed("ParseFromString failed");
        return;
    }
}
