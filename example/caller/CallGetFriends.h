#pragma once

#include "MprpcApplication.h"
#include "MprpcController.h"
#include "user.pb.h"
#include <Logger.h>

inline void func_getfriends(fixbug::UserServiceRpc_Stub *stub, MprpcController *controller) {
    fixbug::GetFriendRequest request3;
    fixbug::GetFriendResponse response3;
    request3.set_id(5);
    request3.set_size(25);

    stub->GetFriends(controller, &request3, &response3, nullptr);
    if (controller->Failed()) {
        std::cout << controller->ErrorText() << std::endl;
        return;
    }
    log_info("{}", response3.success());
    for (auto &p: response3.name()) {
        std::cout << p << std::endl;
    }
}
