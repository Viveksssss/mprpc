#pragma once

#include <google/protobuf/service.h>

class MprpcController : public ::google::protobuf::RpcController {
public:
    MprpcController();

    virtual void Reset() override;
    virtual bool Failed() const override;
    virtual std::string ErrorText() const override;
    virtual void StartCancel() override;
    virtual void SetFailed(std::string const &reason) override;
    virtual bool IsCanceled() const override;
    virtual void NotifyOnCancel(::google::protobuf::Closure *callback) override;

private:
    bool _failed;
    std::string _errot_text;
};
