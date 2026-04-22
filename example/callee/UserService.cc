#include "../user.grpc.pb.h"
#include "../user.pb.h"
#include <absl/base/call_once.h>
#include <grpcpp/support/status.h>

class UserService final : public ::fixbug::UserServiceRpc::Service {
public:
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
    ::grpc::Status Login(::grpc::ServerContext *context,
        ::fixbug::LoginRequest const *request,
        ::fixbug::LoginResponse *response) override {
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

        return ::grpc::Status::OK;
    }
};
