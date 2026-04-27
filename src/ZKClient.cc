
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
            // 1. 关闭旧会话
            zookeeper_close(zh);
            // 下面的步骤是可能需要做的，这里不予演示
            // 2. 重新创建链接
            // 3. 重新创建所有临时节点
            // 4. 重新注册所有watcher
        }
    }
}

} // namespace

ZKClient::ZKClient() : _zhandle(nullptr), _timeout(30000) { }

ZKClient::~ZKClient() {
    Close();
}

void ZKClient::Start() {
    std::string host = MprpcApplication::GetInstance().GetConfig().Get("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Get("zookeeperport");
    _host = host + ":" + port;
    log_info("Zookeeper Started in {}", _host);
    _zhandle = zookeeper_init(_host.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (_zhandle == nullptr) {
        log_error("Zookeeper_init Error");
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);

    zoo_set_context(_zhandle, &sem);

    sem_wait(&sem);
    sem_destroy(&sem);
    log_info("zookeeper_init_success!");
}

void ZKClient::Close() {
    if (_zhandle) {
        zookeeper_close(_zhandle);
        _zhandle = nullptr;
        log_info("Zookeeper connection closed");
    }
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

void ZKClient::Reconnect() {
    log_info("Attempting to reconnect to ZooKeeper...");
    Close(); // 关闭旧的

    _zhandle = zookeeper_init(_host.c_str(), global_watcher, _timeout, nullptr, nullptr, 0);
    if (_zhandle == nullptr) {
        log_error("Reconnect failed: zookeeper_init error");
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(_zhandle, &sem);
    sem_wait(&sem);
    sem_destroy(&sem);

    log_info("Reconnected to ZooKeeper successfully");
}

std::string ZKClient::GetData(char const *path, Stat *stat) {
    /*
        将stat作为传出参数，用户可以接受stat结构数据
    */
    char buffer[1024];
    int buffer_len = sizeof buffer;
    int flag = zoo_get(_zhandle, path, 0, buffer, &buffer_len, stat);
    if (flag != ZOK) {
        log_error("GetData failed :{}", zerror(flag));
        return "";
    }

    return std::string(buffer, buffer_len);
}

void ZKClient::SetData(char const *path, char const *data, int datalen, int version) {
    int flag = zoo_set(_zhandle, path, data, datalen, version);
    if (flag == ZOK) {
        log_info("SetData success for path: {}", path);
    } else {
        log_error("SetData failed for path {}: {}", path, zerror(flag));
        exit(EXIT_FAILURE);
    }
}

void ZKClient::Delete(char const *path, int version) {
    int flag = zoo_delete(_zhandle, path, version);
    if (flag == ZOK) {
        log_info("Delete success for path: {}", path);
    } else if (flag == ZNONODE) {
        log_info("Delete: node does not exist, path: {}", path);
    } else {
        log_error("Delete failed for path {}: {}", path, zerror(flag));
        exit(EXIT_FAILURE);
    }
}

bool ZKClient::Exists(char const *path) {
    int flag = zoo_exists(_zhandle, path, 0, nullptr);
    return (flag == ZOK);
}

std::vector<std::string> ZKClient::GetChildren(char const *path) {
    std::vector<std::string> children;
    struct String_vector strings;
    int flag = zoo_get_children(_zhandle, path, 0, &strings);

    if (flag != ZOK) {
        log_error("GetChildren failed for path {}: {}", path, zerror(flag));
        return children;
    }

    for (int i = 0; i < strings.count; ++i) {
        children.emplace_back(strings.data[i]);
    }

    deallocate_String_vector(&strings);
    return children;
}
