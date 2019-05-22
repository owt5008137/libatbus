#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "common/string_oprs.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <atbus_node.h>

#include "detail/libatbus_protocol.h"

#include "frame/test_macros.h"

CASE_TEST(atbus_node_rela, basic_test) {
    atbus::protocol::msg m_src, m_dst;
    std::string packed_buffer;
    char test_buffer[] = "hello world!";

    {
        m_src.init(0x12345678, ATBUS_CMD_DATA_TRANSFORM_REQ, 123, 0, 13);
        m_src.body.make_forward(456, 789, test_buffer, sizeof(test_buffer));
        m_src.body.forward->router.push_back(210);

        std::stringstream ss;
        msgpack::pack(ss, m_src);
        packed_buffer = ss.str();
        std::stringstream so;
        util::string::serialization(packed_buffer.data(), packed_buffer.size(), so);
        CASE_MSG_INFO() << "msgpack encoded(size=" << packed_buffer.size() << "): " << so.str() << std::endl;
    }

    msgpack::unpacked result;
    {
        msgpack::unpack(result, packed_buffer.data(), packed_buffer.size());
        msgpack::object obj = result.get();
        CASE_EXPECT_FALSE(obj.is_nil());
        obj.convert(m_dst);
    }

    {
        CASE_EXPECT_EQ(ATBUS_CMD_DATA_TRANSFORM_REQ, m_dst.head.cmd);
        CASE_EXPECT_EQ(123, m_dst.head.type);
        CASE_EXPECT_EQ(0, m_dst.head.ret);
        CASE_EXPECT_EQ(13, m_dst.head.sequence);
        CASE_EXPECT_EQ(0x12345678, m_dst.head.src_bus_id);

        CASE_EXPECT_EQ(456, m_dst.body.forward->from);
        CASE_EXPECT_EQ(789, m_dst.body.forward->to);
        CASE_EXPECT_EQ(210, m_dst.body.forward->router.front());
        CASE_EXPECT_EQ(
            0, UTIL_STRFUNC_STRNCMP(test_buffer, reinterpret_cast<const char *>(m_dst.body.forward->content.ptr), sizeof(test_buffer)));
    }
}

CASE_TEST(atbus_node_rela, child_endpoint_opr) {
    atbus::node::conf_t conf;
    atbus::node::default_conf(&conf);
    conf.children_mask = 16;

    atbus::node::ptr_t node = atbus::node::create();
    node->init(0x12345678, &conf);


    atbus::endpoint::ptr_t ep = atbus::endpoint::create(node.get(), 0x12345679, 8, node->get_pid(), node->get_hostname());
    // 插入到末尾
    CASE_EXPECT_EQ(0, node->add_endpoint(ep));
    CASE_EXPECT_EQ(1, node->get_children().size());

    // 插入到中间
    ep = atbus::endpoint::create(node.get(), 0x12345589, 8, node->get_pid(), node->get_hostname());
    CASE_EXPECT_EQ(0, node->add_endpoint(ep));
    CASE_EXPECT_EQ(2, node->get_children().size());

    // 新端点子域冲突-父子关系
    ep = atbus::endpoint::create(node.get(), 0x12345680, 4, node->get_pid(), node->get_hostname());
    CASE_EXPECT_EQ(EN_ATBUS_ERR_PARAMS, node->add_endpoint(atbus::endpoint::ptr_t()));
    CASE_EXPECT_EQ(EN_ATBUS_ERR_ATNODE_MASK_CONFLICT, node->add_endpoint(ep));

    // 新端点子域冲突-子父关系
    ep = atbus::endpoint::create(node.get(), 0x12345780, 12, node->get_pid(), node->get_hostname());
    CASE_EXPECT_EQ(EN_ATBUS_ERR_ATNODE_MASK_CONFLICT, node->add_endpoint(ep));
    ep = atbus::endpoint::create(node.get(), 0x12345480, 12, node->get_pid(), node->get_hostname());
    CASE_EXPECT_EQ(EN_ATBUS_ERR_ATNODE_MASK_CONFLICT, node->add_endpoint(ep));

    // 新端点子域冲突-ID不同子域相同
    ep = atbus::endpoint::create(node.get(), 0x12345680, 8, node->get_pid(), node->get_hostname());
    CASE_EXPECT_EQ(EN_ATBUS_ERR_ATNODE_MASK_CONFLICT, node->add_endpoint(ep));

    // 移除失败-找不到
    CASE_EXPECT_EQ(EN_ATBUS_ERR_ATNODE_NOT_FOUND, node->remove_endpoint(0x12345680));
    // 移除成功
    CASE_EXPECT_EQ(EN_ATBUS_ERR_SUCCESS, node->remove_endpoint(0x12345589));
}
