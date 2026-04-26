#pragma once

#include "MprpcController.h"
#include "user.pb.h"
#include <Logger.h>

inline void func_login(fixbug::UserServiceRpc_Stub *stub, MprpcController *controller) {
    fixbug::LoginRequest request;
    fixbug::LoginResponse response;
    request.set_name("大狗熊");
    request.set_pwd("123456");
    /* rpc请求 */
    stub->Login(controller, &request, &response, nullptr);

    if (controller->Failed()) {
        std::cout << controller->ErrorText() << std::endl;
        return;
    }
    /* rpc调用完成，调用结果 */
    if (!response.success()) {
        log_info("{}:{}", response.result().errmsg(), response.result().errcode());
    } else {
        log_debug("rpc调用成功!");
    }
}
