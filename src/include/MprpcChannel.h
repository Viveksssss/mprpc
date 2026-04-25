#pragma once

#include <google/protobuf/service.h>

using namespace ::google::protobuf;

class MprpcChannel : public ::google::protobuf::RpcChannel {
public:
    void CallMethod(MethodDescriptor const *method, RpcController *controller,
        Message const *request, Message *response, Closure *done) override;
};
