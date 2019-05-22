#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <memory>
#include <limits>
#include <numeric>

#include <common/string_oprs.h>

#include <detail/libatbus_error.h>
#include <atbus_node.h>
#include <atbus_endpoint.h>
#include "frame/test_macros.h"


CASE_TEST(atbus_endpoint, get_children_min_max)
{
    atbus::endpoint::bus_id_t tested = atbus::endpoint::get_children_max_id(0x12345678, 16);
    CASE_EXPECT_EQ(tested, 0x1234FFFF);

    tested = atbus::endpoint::get_children_min_id(0x12345678, 16);
    CASE_EXPECT_EQ(tested, 0x12340000);
}

CASE_TEST(atbus_endpoint, is_child)
{
    atbus::node::conf_t conf;
    atbus::node::default_conf(&conf);
    conf.children_mask = 16;

    {
        atbus::node::ptr_t node = atbus::node::create();
        node->init(0x12345678, &conf);


        // 0值边界检测
        CASE_EXPECT_TRUE(node->is_child_node(0x12340000));
        CASE_EXPECT_TRUE(node->is_child_node(0x1234FFFF));
        CASE_EXPECT_FALSE(node->is_child_node(0x1233FFFF));
        CASE_EXPECT_FALSE(node->is_child_node(0x12350000));

        // 自己是自己的子节点
        CASE_EXPECT_TRUE(node->is_child_node(node->get_id()));
    }

    {
        conf.children_mask = 0;
        atbus::node::ptr_t node = atbus::node::create();
        node->init(0x12345678, &conf);
        // 0值判定，无子节点
        CASE_EXPECT_TRUE(node->is_child_node(0x12345678));
        CASE_EXPECT_FALSE(node->is_child_node(0x12345679));
    }
}

CASE_TEST(atbus_endpoint, is_brother)
{
    uint32_t fake_mask = 24;
    atbus::node::conf_t conf;
    atbus::node::default_conf(&conf);
    conf.children_mask = 16;

    atbus::node::ptr_t node = atbus::node::create();
    node->init(0x12345678, &conf);

    // 自己不是自己的兄弟节点
    CASE_EXPECT_FALSE(node->get_self_endpoint()->is_brother_node(node->get_id(), fake_mask));

    /*       F               F
    //      / \             / \
    //    [A]  B          [A]  F
    //                        / \
    //                       X   B
    // 兄弟节点的子节点仍然是兄弟节点
    */
    CASE_EXPECT_TRUE(node->get_self_endpoint()->is_brother_node(0x12335678, fake_mask));

    /*       B
    //      / \
    //    [A]  X
    // 父节点是兄弟节点
    */
    CASE_EXPECT_TRUE(node->get_self_endpoint()->is_brother_node(0x12000001, fake_mask));
    
    
    /*      [A]
    //      / \
    //     B   X
    // 子节点不是兄弟节点
    */
    CASE_EXPECT_FALSE(node->get_self_endpoint()->is_brother_node(0x12340001, fake_mask));

    /*         F
    //        / \
    //       F   B
    //      / \
    //    [A]  X
    // 父节点的兄弟节点不是兄弟节点
    */
    CASE_EXPECT_FALSE(node->get_self_endpoint()->is_brother_node(0x11345678, fake_mask));

    // 0值判定，无父节点
    CASE_EXPECT_TRUE(node->get_self_endpoint()->is_brother_node(0x12000001, 0));
    CASE_EXPECT_FALSE(node->get_self_endpoint()->is_brother_node(0x12340001, 0));
}

CASE_TEST(atbus_endpoint, is_father)
{
    uint32_t fake_mask = 24;
    atbus::node::conf_t conf;
    atbus::node::default_conf(&conf);
    conf.children_mask = 16;

    atbus::node::ptr_t node = atbus::node::create();
    node->init(0x12345678, &conf);
    CASE_EXPECT_TRUE(node->get_self_endpoint()->is_parent_node(0x12000001, 0x12000001, fake_mask));

    CASE_EXPECT_FALSE(node->get_self_endpoint()->is_parent_node(0x12000002, 0x12000001, fake_mask));
}

CASE_TEST(atbus_endpoint, get_connection)
{
    atbus::node::conf_t conf;
    atbus::node::default_conf(&conf);
    conf.children_mask = 16;
    uv_loop_t ev_loop;
    uv_loop_init(&ev_loop);

    conf.ev_loop = &ev_loop;
    conf.recv_buffer_size = 64 * 1024;

    char* buffer = new char[conf.recv_buffer_size];
    memset(buffer, -1, sizeof(conf.recv_buffer_size)); // init it and then valgrind will now report uninitialised used

    char addr[32] = { 0 };
    UTIL_STRFUNC_SNPRINTF(addr, sizeof(addr), "mem://0x%p", buffer);
    if (addr[8] == '0' && addr[9] == 'x') {
        memset(addr, 0, sizeof(addr));
        UTIL_STRFUNC_SNPRINTF(addr, sizeof(addr), "mem://%p", buffer);
    }

    // 排除未完成连接
    {
        atbus::node::ptr_t node = atbus::node::create();
        node->init(0x12345678, &conf);

        atbus::connection::ptr_t conn1 = atbus::connection::create(node.get());

        CASE_EXPECT_EQ(0, conn1->connect(addr));

        atbus::connection::ptr_t conn2 = atbus::connection::create(node.get());
        conn2->connect("ipv4://127.0.0.1:80");

        atbus::endpoint::ptr_t ep = atbus::endpoint::create(node.get(), 0x12345679, 8, node->get_pid(), node->get_hostname());
        CASE_EXPECT_TRUE(ep->add_connection(conn1.get(), false));
        CASE_EXPECT_TRUE(ep->add_connection(conn2.get(), false));

        CASE_EXPECT_EQ(0, node->add_endpoint(ep));

        atbus::connection* conn3 = node->get_self_endpoint()->get_data_connection(ep.get());
        CASE_EXPECT_EQ(conn3, conn1.get());

    }

    while (UV_EBUSY == uv_loop_close(&ev_loop)) {
        uv_run(&ev_loop, UV_RUN_ONCE);
    }

    delete []buffer;
}
