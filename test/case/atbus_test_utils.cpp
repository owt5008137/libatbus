
#include "atbus_test_utils.h"
#include "frame/test_macros.h"

void unit_test_tick_handle(uv_timer_t *handle) { 
    handle->data = NULL;
    uv_stop(handle->loop); 
}

void unit_test_timeout_handle(uv_timer_t *handle) {
    handle->data = NULL;
    uv_stop(handle->loop);

    CASE_MSG_ERROR() << CASE_MSG_FCOLOR(RED) << "wait timeout."<< std::endl;
}

void unit_test_setup_exit(uv_loop_t *ev, uint64_t timeout_ms) {
    size_t left_tick = timeout_ms / 8;
    while (left_tick > 0 && UV_EBUSY == uv_loop_close(ev)) {
        // loop 8 times
        for (int i = 0; i < 8; ++i) {
            uv_run(ev, UV_RUN_NOWAIT);
        }
        CASE_THREAD_SLEEP_MS(8);

        --left_tick;
    }

    CASE_EXPECT_NE(left_tick, 0);
}

static void unit_test_timer_close_handle(uv_handle_t *handle) {
    handle->data = NULL;
    uv_stop(handle->loop);
}

unit_test_libuv_wait_manager::unit_test_libuv_wait_manager(uv_loop_t *ev, uint64_t timeout_ms, uint64_t tick_ms) : tick_enabled(false) {
    uv_timer_init(ev, &timeout_timer_);

    if (tick_ms > 0) {
        if (0 == uv_timer_init(ev, &tick_timer_)) {
            tick_enabled = true;
        }
    }
    tick_timer_.data = &tick_timer_;
    timeout_timer_.data = &timeout_timer_;

    if (0 != uv_timer_start(&timeout_timer_, unit_test_timeout_handle, timeout_ms, 0)) {
        timeout_timer_.data = NULL;
    }

    if (tick_enabled) {
        uv_timer_start(&tick_timer_, unit_test_tick_handle, tick_ms, tick_ms);
    }
}

unit_test_libuv_wait_manager::~unit_test_libuv_wait_manager() {
    uv_loop_t *ev = timeout_timer_.loop;
    uv_timer_stop(&timeout_timer_);

    timeout_timer_.data = &timeout_timer_;
    uv_close(reinterpret_cast<uv_handle_t *>(&timeout_timer_), unit_test_timer_close_handle);

    if (tick_enabled) {
        uv_timer_stop(&tick_timer_);
        tick_timer_.data = &tick_timer_;
        uv_close(reinterpret_cast<uv_handle_t *>(&tick_timer_), unit_test_timer_close_handle);
    } else {
        tick_timer_.data = NULL;
    }

    while (NULL != timeout_timer_.data || NULL != tick_timer_.data) {
        uv_run(ev, UV_RUN_DEFAULT);
    }
}

void unit_test_libuv_wait_manager::run(uv_loop_t *ev) {
    tick_timer_.data = &tick_timer_;
    uv_run(ev, UV_RUN_ONCE);
}