
#include "include/ZKClient.h"
#include "Logger.h"
#include "MprpcApplication.h"
#include <cstdlib>
#include <semaphore.h>
#include <string>
#include <zookeeper.h>

namespace {

void global_watcher(zhandle_t *zh, int type, int state, char const *path, void *watcherCtx) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            log_error("Session expired");
        }
    }
}

} // namespace

ZKClient::ZKClient() : _zhandle(nullptr) { }

ZKClient::~ZKClient() {
    if (_zhandle) {
        zookeeper_close(_zhandle);
    }
}

void ZKClient::Start() {
    std::string host = MprpcApplication::GetInstance().GetConfig().Get("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Get("zookeeperport");
    std::string connstr = host + ":" + port;
    log_info("Zookeeper Started in {}", connstr);
    _zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (_zhandle == nullptr) {
        log_error("Zookeeper_init Error");
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);

    zoo_set_context(_zhandle, &sem);

    sem_wait(&sem);
    log_info("zookeeper_init_success!");
}

void ZKClient::Create(char const *path, char const *data, int datalen, int state) {
    char path_buf[128];
    int bufflen = sizeof path_buf;
    int flag;
    flag = zoo_exists(_zhandle, path, 0, nullptr);
    if (flag == ZNONODE) /* 节点不存在 */ {
        flag = zoo_create(
            _zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buf, bufflen);
        if (flag == ZOK) {
            std::cout << "znode create success:" << path << std::endl;
        } else {
            log_error("znode create failed,with path {} ", path);
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZKClient::GetData(char const *path) {
    char buffer[1024];
    int buffer_len = sizeof buffer;
    struct Stat stat;
    int flag = zoo_get(_zhandle, path, 0, buffer, &buffer_len, &stat);
    if (flag != ZOK) {
        log_error("GetData failed :{}", zerror(flag));
        return "";
    }

    return std::string(buffer, buffer_len);
}
