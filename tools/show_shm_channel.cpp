#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <thread>

#include "config/compiler_features.h"
#include "detail/libatbus_channel_export.h"
#include <detail/libatbus_error.h>

#if defined(ATBUS_CHANNEL_SHM)
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s <shm key> [need node info: 0 or 1] [node data size]\n", argv[0]);
        return 0;
    }

    using namespace atbus::channel;
    shm_channel *channel = NULL;
    key_t shm_key;
    if ('0' == *argv[1] && *(argv[1] + 1) && ('x' == *(argv[1] + 1) || 'X' == *(argv[1] + 1))) {
        shm_key = (key_t)strtol(argv[1] + 2, NULL, 16);
    } else {
        shm_key = (key_t)strtol(argv[1], NULL, 10);
    }
    long need_node_info   = 0;
    size_t need_node_data = 0;

    if (argc > 2) {
        need_node_info = strtol(argv[2], NULL, 10);
    }

    if (argc > 3) {
        need_node_data = (size_t)strtol(argv[3], NULL, 10);
    }

    int res = shm_attach(shm_key, 0, &channel, NULL);
    if (res < 0) {
        fprintf(stderr, "shm_attach for 0x%llx failed, ret: %d\n", static_cast<unsigned long long>(shm_key), res);
        return res;
    }

    shm_show_channel(channel, std::cout, !!need_node_info, need_node_data);
    return 0;
}
#else
int main() {
    puts("shm channel disabled");
    return 0;
}
#endif