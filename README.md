
## 项目介绍

该项目是在 Linux 环境下基于 muduo、Protobuf 和 Zookeeper 实现的一个轻量级 RPC 框架。可以把单体架构系统的本地方法调用，重构成基于 TCP 网络通信的 RPC 远程方法调用，实现统一台机器不同进程或者不同机器之间的服务调用。

## 项目特点

- 基于 muduo 网络库实现高并发网络通信模块，作为 RPC 远程调用的基础。
- 基于 Protobuf 实现 RPC 方法调用和参数的序列化和反序列化，并根据其提供得 RPC 接口编写 RPC 服务。
- 基于 ZooKeeper 分布式协调服务中间件提供服务注册和服务发现功能。
- 基于生产者消费者模型，实现了异步工作方式的日志模块。

## 项目依赖

- zookeeper
- muduo网络库
- protobuf

## 安装

```sh
# 先自行安装完成其他依赖然后:
git clone https://github.com/Viveksssss/mprpc.git

cd mprpc

./auto_build.sh
```

## 快速使用

### 1.cmake导入mprpc库

```cmake
find_package(mprpc REQUIRED)
target_link_libraries(project PRIVATE mprpc)
```

### 2. 创建双方的协议格式protobuf

实例创建了三个服务方法,分别是注册,登陆和查询好友列表.

```protobuf
syntax = "proto3";

package fixbug;

option cc_generic_services = true;

message ResultCode {
    int32 errcode = 1;
    bytes errmsg = 2;
}

message LoginRequest {
    bytes name = 1;
    bytes pwd = 2;
}

message LoginResponse {
    ResultCode result = 1;
    bool success = 2;
}

/* 用户远程调用 */

message RegisterRequest {
    uint32 id = 1;
    bytes name = 2;
    bytes pwd = 3;
}

message RegisterResponse {
    ResultCode result = 1;
    bool success = 2;
}

message GetFriendRequest {
    uint32 id = 1;
    uint32 size = 2;
}

message GetFriendResponse {
    uint32 size = 1;
    repeated bytes name = 2;
    ResultCode result = 3;
    bool success = 4;
}

// 用户服务
service UserServiceRpc {
    /* 用户调用 */
    rpc Login(LoginRequest) returns (LoginResponse);
    /* 用户注册 */
    rpc Register(RegisterRequest) returns (RegisterResponse);
    /* 拉取好友列表 */
    rpc GetFriends(GetFriendRequest) returns (GetFriendResponse);
}
```

### 3. 使用

#### 1. 服务端

导入我们的rpc头文件和protobuf生成的头文件.

`UserService`类继承了`protobuf`自动生成的`UserServiceRpc`类(就是我们protobuf中的服务名称),然后分别实现我们的业务的逻辑和rpc处理逻辑.

初始化`MprpcApplication`,然后创建我们的服务发布类`MprpcProvider`和我们的自定义服务类`UserService`,将服务类使用provider进行注册.然后启动`provider`,rpc服务开启.

```sh
❯ ./callee -i config.conf
2026-04-27 09:22:14.250239389 CST /home/vivek/Codes/Cpp/mprpc/example/callee/UserService.cc:114 [info] callee_start
2026-04-27 09:22:14.250291245 CST /home/vivek/Codes/Cpp/mprpc/example/callee/UserService.cc:116 [info] Init ok
UserService ptr in main: 0x556473eed330
2026-04-27 09:22:14.250828070 CST /home/vivek/Codes/Cpp/mprpc/example/callee/UserService.cc:121 [info] 注册完成！
2026-04-27 09:22:14.250897419 CST /home/vivek/Codes/Cpp/mprpc/src/MprpcProvider.cc:51 [info] Started in 127.0.0.1:8888
2026-04-27 09:22:14.250908324 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:41 [info] Zookeeper Started in 127.0.0.1:2181
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1250: Client environment:zookeeper.version=zookeeper C client 3.8.6
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1254: Client environment:host.name=archlinux
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1261: Client environment:os.name=Linux
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1262: Client environment:os.arch=6.19.12-arch1-1
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1263: Client environment:os.version=#1 SMP PREEMPT_DYNAMIC Sat, 11 Apr 2026 22:36:29 +0000
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1271: Client environment:user.name=vivek
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1279: Client environment:user.home=/home/vivek
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@log_env@1291: Client environment:user.dir=/home/vivek/Codes/Cpp/mprpc/out
2026-04-27 09:22:14,250:552548(0x7f5d409aeac0):ZOO_INFO@zookeeper_init_internal@1335: Initiating client connection, host=127.0.0.1:2181 sessionTimeout=30000 watcher=0x7f5d42800ac0 sessionId=0 sessionPasswd=<null> context=(nil) flags=0
2026-04-27 09:22:14,251:552548(0x7f5d3fac76c0):ZOO_INFO@check_events@2987: initiated connection to server 127.0.0.1:2181
2026-04-27 09:22:14,262:552548(0x7f5d3fac76c0):ZOO_INFO@finalize_session_establishment@2861: session establishment complete on server 127.0.0.1:2181, sessionId=0x100003c69b80000, negotiated timeout=30000 
2026-04-27 09:22:14.262785910 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:55 [info] zookeeper_init_success!
znode create success:/UserServiceRpc/GetFriends
znode create success:/UserServiceRpc/Register
znode create success:/UserServiceRpc/Login
2026-04-27 09:22:14.274201650 CST /home/vivek/Codes/Cpp/mprpc/src/MprpcProvider.cc:75 [info] RpcProvide Started
```

```cpp
#include "../user.pb.h"
#include "Loggers.h"
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
    // Loggers::GetInstance().set_path("/home/vivek/");
    LOG_INFO("/home");

    log_start();
    set_log_level(log_level::info);
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
```



#### 2. 客户端

导入protobuf生成的cc头文件和我们的mprpc头文件`MprpcApplication.h`

然后在请求函数中发送请求等待服务相应结果:

```sh
./caller -i config.conf
./caller-i:config.conf
2026-04-27 09:22:42.554861413 CST /home/vivek/Codes/Cpp/mprpc/example/caller/CallUserService.cc:12 [info] MprpcClient Started
2026-04-27 09:22:42.555528814 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:41 [info] Zookeeper Started in 127.0.0.1:2181
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1250: Client environment:zookeeper.version=zookeeper C client 3.8.6
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1254: Client environment:host.name=archlinux
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1261: Client environment:os.name=Linux
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1262: Client environment:os.arch=6.19.12-arch1-1
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1263: Client environment:os.version=#1 SMP PREEMPT_DYNAMIC Sat, 11 Apr 2026 22:36:29 +0000
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1271: Client environment:user.name=vivek
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1279: Client environment:user.home=/home/vivek
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1291: Client environment:user.dir=/home/vivek/Codes/Cpp/mprpc/out
2026-04-27 09:22:42,555:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_init_internal@1335: Initiating client connection, host=127.0.0.1:2181 sessionTimeout=30000 watcher=0x7fc7a5e4bac0 sessionId=0 sessionPasswd=<null> context=(nil) flags=0
2026-04-27 09:22:42,555:555971(0x7fc7a39196c0):ZOO_INFO@check_events@2987: initiated connection to server 127.0.0.1:2181
2026-04-27 09:22:42,559:555971(0x7fc7a39196c0):ZOO_INFO@finalize_session_establishment@2861: session establishment complete on server 127.0.0.1:2181, sessionId=0x100003c69b80001, negotiated timeout=30000 
2026-04-27 09:22:42.559394772 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:55 [info] zookeeper_init_success!
2026-04-27 09:22:42,561:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_close@3850: Closing zookeeper sessionId=0x100003c69b80001 to 127.0.0.1:2181

2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_close@3873: Freeing zookeeper resources for sessionId=0x100003c69b80001

2026-04-27 09:22:42.563086494 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:62 [info] Zookeeper connection closed
2026-04-27 09:22:42.563149913 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:41 [info] Zookeeper Started in 127.0.0.1:2181
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1250: Client environment:zookeeper.version=zookeeper C client 3.8.6
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1254: Client environment:host.name=archlinux
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1261: Client environment:os.name=Linux
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1262: Client environment:os.arch=6.19.12-arch1-1
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1263: Client environment:os.version=#1 SMP PREEMPT_DYNAMIC Sat, 11 Apr 2026 22:36:29 +0000
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1271: Client environment:user.name=vivek
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1279: Client environment:user.home=/home/vivek
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1291: Client environment:user.dir=/home/vivek/Codes/Cpp/mprpc/out
2026-04-27 09:22:42,563:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_init_internal@1335: Initiating client connection, host=127.0.0.1:2181 sessionTimeout=30000 watcher=0x7fc7a5e4bac0 sessionId=0 sessionPasswd=<null> context=(nil) flags=0
2026-04-27 09:22:42,563:555971(0x7fc7a31186c0):ZOO_INFO@check_events@2987: initiated connection to server 127.0.0.1:2181
2026-04-27 09:22:42,565:555971(0x7fc7a31186c0):ZOO_INFO@finalize_session_establishment@2861: session establishment complete on server 127.0.0.1:2181, sessionId=0x100003c69b80002, negotiated timeout=30000 
2026-04-27 09:22:42.565360217 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:55 [info] zookeeper_init_success!
2026-04-27 09:22:42,566:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_close@3850: Closing zookeeper sessionId=0x100003c69b80002 to 127.0.0.1:2181

2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_close@3873: Freeing zookeeper resources for sessionId=0x100003c69b80002

2026-04-27 09:22:42.567661489 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:62 [info] Zookeeper connection closed
2026-04-27 09:22:42.567705703 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:41 [info] Zookeeper Started in 127.0.0.1:2181
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1250: Client environment:zookeeper.version=zookeeper C client 3.8.6
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1254: Client environment:host.name=archlinux
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1261: Client environment:os.name=Linux
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1262: Client environment:os.arch=6.19.12-arch1-1
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1263: Client environment:os.version=#1 SMP PREEMPT_DYNAMIC Sat, 11 Apr 2026 22:36:29 +0000
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1271: Client environment:user.name=vivek
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1279: Client environment:user.home=/home/vivek
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@log_env@1291: Client environment:user.dir=/home/vivek/Codes/Cpp/mprpc/out
2026-04-27 09:22:42,567:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_init_internal@1335: Initiating client connection, host=127.0.0.1:2181 sessionTimeout=30000 watcher=0x7fc7a5e4bac0 sessionId=0 sessionPasswd=<null> context=(nil) flags=0
2026-04-27 09:22:42,567:555971(0x7fc7a39196c0):ZOO_INFO@check_events@2987: initiated connection to server 127.0.0.1:2181
2026-04-27 09:22:42,570:555971(0x7fc7a39196c0):ZOO_INFO@finalize_session_establishment@2861: session establishment complete on server 127.0.0.1:2181, sessionId=0x100003c69b80003, negotiated timeout=30000 
2026-04-27 09:22:42.570041219 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:55 [info] zookeeper_init_success!
2026-04-27 09:22:42,570:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_close@3850: Closing zookeeper sessionId=0x100003c69b80003 to 127.0.0.1:2181

2026-04-27 09:22:42,571:555971(0x7fc7a3ff9ac0):ZOO_INFO@zookeeper_close@3873: Freeing zookeeper resources for sessionId=0x100003c69b80003

2026-04-27 09:22:42.571875046 CST /home/vivek/Codes/Cpp/mprpc/src/ZKClient.cc:62 [info] Zookeeper connection closed
朋友0
朋友1
朋友2
朋友3
朋友4
朋友5
朋友6
朋友7
朋友8
朋友9
朋友10
朋友11
朋友12
朋友13
朋友14
朋友15
朋友16
朋友17
朋友18
朋友19
朋友20
朋友21
朋友22
朋友23
朋友24
```

```cpp
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
```

编写相应的服务请求函数

```cpp
// CallUserLogin.h
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


// CallRegister.h
#pragma once

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
    // log_info("{}", response2.success());
}


// CallGetFriends.h
#pragma once

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
    for (auto &p: response3.name()) {
        std::cout << p << std::endl;
    }
}
```

