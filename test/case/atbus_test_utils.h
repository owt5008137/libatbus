#ifndef UNITTEST_LIBATBUS_TEST_UTILS_H_
#define UNITTEST_LIBATBUS_TEST_UTILS_H_

#pragma once

#include "uv.h"

void unit_test_tick_handle(uv_timer_t *handle);
void unit_test_timeout_handle(uv_timer_t *handle);
void unit_test_setup_exit(uv_loop_t *ev, uint64_t timeout_ms = 30000);

struct unit_test_libuv_wait_manager {
    unit_test_libuv_wait_manager(uv_loop_t *ev, uint64_t timeout_ms, uint64_t tick_ms);
    ~unit_test_libuv_wait_manager();

    void run(uv_loop_t *ev);

    uv_timer_t timeout_timer_;
    uv_timer_t tick_timer_;
    bool tick_enabled;
};

#define UNITTEST_WAIT_UNTIL(ev, conndition, timeout_ms, tick_ms)                                                                    \
    for (unit_test_libuv_wait_manager libuv_wait_mgr(ev, timeout_ms, tick_ms); libuv_wait_mgr.timeout_timer_.data && !(conndition); \
         libuv_wait_mgr.run(ev)) \
        if (NULL == libuv_wait_mgr.tick_timer_.data)

#define UNITTEST_WAIT_IF(ev, conndition, timeout_ms, tick_ms)                                                                      \
    for (unit_test_libuv_wait_manager libuv_wait_mgr(ev, timeout_ms, tick_ms); libuv_wait_mgr.timeout_timer_.data && (conndition); \
         libuv_wait_mgr.run(ev)) \
        if (NULL == libuv_wait_mgr.tick_timer_.data)


#endif