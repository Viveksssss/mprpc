#pragma once

#include <semaphore.h>
#include <string>
#include <vector>
#include <zookeeper/zookeeper.h>

class ZKClient {
public:
    ZKClient();
    ~ZKClient();
    void Start();
    void Create(char const *path, char const *data, int datalen, int state = 0);
    std::string GetData(char const *path, Stat *stat = nullptr);
    void SetData(char const *path, char const *data, int datalen, int version = 1);
    void Delete(char const *path, int version = 1);
    bool Exists(char const *path);
    std::vector<std::string> GetChildren(char const *path);
    void Close();
    void Reconnect();

    // 获取原始句柄
    zhandle_t *GetHandle() const {
        return _zhandle;
    }

private:
    zhandle_t *_zhandle;
    int _timeout;
    std::string _host;
};
