#pragma once

#include "google/protobuf/service.h"
#include <Buffer.h>
#include <Callbacks.h>
#include <EventLoop.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <InetAddress.h>
#include <string>
#include <TcpConnection.h>
#include <Timestamp.h>
#include <unordered_map>
/**
 * @brief 服务发布类
 *
 */

class Service;

class RpcProvider : noncopyable {
public:
    void NotifyService(::google::protobuf::Service *service);
    void Run();

private:
    struct ctx {
    public:
        ctx(::google::protobuf::Message *req, ::google::protobuf::Message *res)
            : request(req)
            , response(res) { }

        ~ctx() {
            delete request;
            delete response;
        }

        ::google::protobuf::Message *request;
        ::google::protobuf::Message *response;
    };

    void OnConnection(TcpConnectionPtr const &);
    void OnMessage(TcpConnectionPtr const &, Buffer *, [[maybe_unused]] Timestamp);
    /* 用于序列化rpc的相应和网络发送 */
    void SendRpcResponse(TcpConnectionPtr const &, ctx *c);

private:
    /* 服务信息 */
    struct ServiceInfo {
        ::google::protobuf::Service *_service;
        std::unordered_map<std::string, ::google::protobuf::MethodDescriptor const *> _methods;
    };

    /* 映射表 */
    std::unordered_map<std::string, ServiceInfo> _services;

    EventLoop *_loop;
};
