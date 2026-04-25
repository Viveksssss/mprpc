#pragma once

#include "MprpcApplication.h"
#include "MprpcController.h"
#include "user.pb.h"
#include <Logger.h>

inline void func_register(fixbug::UserServiceRpc_Stub *stub, MprpcController *controller) {
    fixbug::RegisterRequest request2;
    fixbug::RegisterResponse response2;
    request2.set_id(222);
    request2.set_name("牛🐮");
    request2.set_pwd("🐮二");

    stub->Register(controller, &request2, &response2, nullptr);
    if (controller->Failed()) {
        std::cout << controller->ErrorText() << std::endl;
        return;
    }
    log_info("{}", response2.success());
}
