/**
 * atbus_msg_handler.h
 *
 *  Created on: 2015年12月14日
 *      Author: owent
 */

#pragma once

#ifndef LIBATBUS_MSG_HANDLER_H_
#define LIBATBUS_MSG_HANDLER_H_

#include <bitset>
#include <ctime>
#include <list>

namespace atbus {
    namespace protocol {
        struct msg;
    }

    class node;
    class endpoint;
    class connection;

    struct msg_handler {
        typedef int (*handler_fn_t)(node &n, connection *conn, protocol::msg &, int status, int errcode);

        static int dispatch_msg(node &n, connection *conn, protocol::msg *, int status, int errcode);

        static int send_ping(node &n, connection &conn, uint32_t seq);

        static int send_reg(int32_t msg_id, node &n, connection &conn, int32_t ret_code, uint32_t seq);

        static int send_transfer_rsp(node &n, protocol::msg &, int32_t ret_code);

        static int send_msg(node &n, connection &conn, const protocol::msg &m);


        // ========================= 接收handle =========================
        static int on_recv_data_transfer_req(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_data_transfer_rsp(node &n, connection *conn, protocol::msg &, int status, int errcode);

        static int on_recv_custom_cmd_req(node &n, connection *conn, protocol::msg &, int status, int errcode);

        static int on_recv_node_sync_req(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_node_sync_rsp(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_node_reg_req(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_node_reg_rsp(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_node_conn_syn(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_node_ping(node &n, connection *conn, protocol::msg &, int status, int errcode);
        static int on_recv_node_pong(node &n, connection *conn, protocol::msg &, int status, int errcode);
    };
}

#endif /* LIBATBUS_MSG_HANDLER_H_ */
