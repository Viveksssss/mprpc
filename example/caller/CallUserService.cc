#include "CallGetFriends.h"
#include "CallLogin.h"
#include "CallRegister.h"
#include "MprpcApplication.h"
#include "user.pb.h"
#include <Logger.h>

auto main(int argc, char **argv) -> int {
    std::cout << argv[0] << argv[1] << ":" << argv[2] << std::endl;
    log_start();
    set_log_level(log_level::info);
    log_info("MprpcClient Started");
    MprpcApplication::Init(argc, argv);
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel{});
    MprpcController controller;

    func_login(&stub, &controller);

    func_register(&stub, &controller);

    func_getfriends(&stub, &controller);

    return 0;
}
