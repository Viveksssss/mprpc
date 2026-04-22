#include "./protobuf/test.pb.h"

auto main() -> int {
    // 1.
    // fixbug::LoginRequest req;
    // req.set_name("Bob");
    // req.set_pwd("123456");

    // std::string send_str = req.SerializeAsString();

    // std::cout << "send_str: " << send_str << std::endl;

    // fixbug::LoginRequest req2;
    // if (!req2.ParseFromString(send_str)) {
    //     std::cerr << "Failed to parse LoginRequest from string." <<
    //     std::endl; return -1;
    // }
    // std::cout << "name: " << req2.name() << std::endl;
    // std::cout << "pwd: " << req2.pwd() << std::endl;
    // 2.
    ::fixbug::LoginResponse res;
    ::fixbug::ResultCode *rc = res.mutable_result();
    rc->set_err_code(0);

    ::fixbug::GetFriendListResponse fres;
    ::fixbug::ResultCode *frc = fres.mutable_result();
    frc->set_err_code(0);
    ::fixbug::User *user1 = fres.add_friend_list();
    user1->set_age(23);
    user1->set_name("Jenny");
    user1->set_sex(::fixbug::User::MAN);

    ::google::protobuf::RepeatedPtrField<::fixbug::User> *users
        = fres.mutable_friend_list();
    for (auto &p: *users) {
        std::cout << p.age() << std::endl;
    }

    std::cout << fres.friend_list_size() << std::endl;

    return 0;
}
