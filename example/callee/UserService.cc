#include "../user.pb.h"
#include "MprpcApplication.h"
#include "MprpcProvider.h"
#include <absl/base/call_once.h>
#include <Config.h>
#include <cstddef>
#include <cstdint>
#include <google/protobuf/service.h>
#include <grpcpp/support/status.h>
#include <Logger.h>
#include <string>

class UserService final : public fixbug::UserServiceRpc {
public:
    UserService() { }

    bool Login(std::string name, std::string pwd) {
        std::cout << "name:" << name << "\t" << "passwd:" << pwd << "\n";
        return true;
    }

    /**
     * @brief
     * 框架直接调用，当请求发来，框架直接调用相应的::fixbug::UserServiceRpc::Service::Login,然后我们可以在其中调用我们的业务Login
     *
     * @param context
     * @param request
     * @param response
     * @return ::grpc::Status
     */
    void Login(::google::protobuf::RpcController *controller, ::fixbug::LoginRequest const *request,
        ::fixbug::LoginResponse *response, ::google::protobuf::Closure *done) override {
        std::string name = request->name();
        std::string passwd = request->pwd();
        bool ok = this->Login(name, passwd);
        if (ok) {
            response->set_success(true);
            response->mutable_result()->set_errcode(0);
            response->mutable_result()->set_errmsg("");
        } else {
            response->set_success(false);
            response->mutable_result()->set_errcode(1);
            response->mutable_result()->set_errmsg("Unknown params");
        }
        if (done) {
            done->Run();
        }
    }

    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local code" << std::endl;
        return true;
    }

    void Register(::google::protobuf::RpcController *controller,
        ::fixbug::RegisterRequest const *request, ::fixbug::RegisterResponse *response,
        ::google::protobuf::Closure *done) override {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool ret = Register(id, name, pwd);
        if (ret) {
            response->set_success(true);
        } else {
            response->set_success(false);
            response->mutable_result()->set_errmsg("not user");
            response->mutable_result()->set_errcode(3);
        }
        if (done) {
            done->Run();
        }
    }

    std::vector<std::string> GetFriends(uint32_t id, size_t size) {
        std::vector<std::string> vec;
        vec.reserve(size);
        for (auto i = 0; i < size; ++i) {
            vec.push_back("朋友" + std::to_string(i));
        }
        return vec;
    }

    void GetFriends(::google::protobuf::RpcController *controller,
        ::fixbug::GetFriendRequest const *request, ::fixbug::GetFriendResponse *response,
        ::google::protobuf::Closure *done) override {
        auto size = request->size();
        auto id = request->id();
        auto vec = GetFriends(id, size);
        if (!vec.empty()) {
            response->set_success(true);
            response->mutable_result()->set_errcode(0);
            response->mutable_result()->set_errmsg("");
            for (auto &p: vec) {
                response->add_name(p);
            }
        } else {
            response->set_success(false);
            response->mutable_result()->set_errcode(1);
            response->mutable_result()->set_errmsg("Unknown params");
        }
        if (done) {
            done->Run();
        }
    }
};

auto main(int argc, char **argv) -> int {
    log_start();
    set_log_level(log_level::debug);
    log_info("callee_start");
    MprpcApplication::Init(argc, argv);
    log_info("Init ok");
    MprpcProvider provider;
    auto *svc = new UserService{};
    printf("UserService ptr in main: %p\n", (void *)svc);
    provider.NotifyService(svc);
    log_info("注册完成！");
    provider.Run();
}
