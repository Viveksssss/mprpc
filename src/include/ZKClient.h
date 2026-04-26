#pragma once

#include <semaphore.h>
#include <string>
#include <zookeeper/zookeeper.h>

class ZKClient {
public:
    ZKClient();
    ~ZKClient();
    void Start();
    void Create(char const *path, char const *data, int datalen, int state = 0);
    std::string GetData(char const *path);

private:
    zhandle_t *_zhandle;
};
