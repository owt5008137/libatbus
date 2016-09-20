/**
 * atbus_node.h
 *
 *  Created on: 2015年10月29日
 *      Author: owent
 */

#ifndef LIBATBUS_NODE_H_
#define LIBATBUS_NODE_H_

#pragma once

#include <bitset>
#include <cstddef>
#include <ctime>
#include <map>
#include <set>
#include <stdint.h>

#ifdef _MSC_VER
#include <WinSock2.h>
#endif

#include "design_pattern/noncopyable.h"
#include "lock/seq_alloc.h"
#include "std/functional.h"
#include "std/smart_ptr.h"

#include "detail/libatbus_channel_export.h"
#include "detail/libatbus_config.h"
#include "detail/libatbus_error.h"

#include "atbus_endpoint.h"

namespace atbus {

    class node CLASS_FINAL : public util::design_pattern::noncopyable {
    public:
        typedef std::shared_ptr<node> ptr_t;

        typedef ATBUS_MACRO_BUSID_TYPE bus_id_t;
        struct conf_flag_t {
            enum type {
                EN_CONF_GLOBAL_ROUTER, /** 全局路由表 **/
                EN_CONF_MAX
            };
        };

        /** 并没有非常复杂的状态切换，所以没有引入状态机 **/
        struct state_t {
            enum type { CREATED = 0, INITED, LOST_PARENT, CONNECTING_PARENT, RUNNING };
        };

        struct flag_t {
            enum type {
                EN_FT_RESETTING,       /** 正在重置 **/
                EN_FT_ACTIVED,         /** 已激活 **/
                EN_FT_PARENT_REG_DONE, /** 已通过父节点注册 **/
                EN_FT_SHUTDOWN,        /** 已完成关闭前的资源回收 **/
                EN_FT_MAX,             /** flag max **/
            };
        };

        typedef struct {
            adapter::loop_t *ev_loop;
            uint32_t children_mask;                      /** 子节点掩码 **/
            std::bitset<conf_flag_t::EN_CONF_MAX> flags; /** 开关配置 **/
            std::string father_address;                  /** 父节点地址 **/
            int loop_times;                              /** 消息循环次数限制，防止某些通道繁忙把其他通道堵死 **/
            int ttl;                                     /** 消息转发跳转限制 **/

            // ===== 连接配置 =====
            int backlog;
            time_t first_idle_timeout; /** 第一个包允许的空闲时间，秒 **/
            time_t ping_interval;      /** ping包间隔，秒 **/
            time_t retry_interval;     /** 重试包间隔，秒 **/
            size_t fault_tolerant;     /** 容错次数，次 **/

            // ===== 缓冲区配置 =====
            size_t msg_size;           /** 数据包大小 **/
            size_t recv_buffer_size;   /** 接收缓冲区，和数据包大小有关 **/
            size_t send_buffer_size;   /** 发送缓冲区限制 **/
            size_t send_buffer_number; /** 发送缓冲区静态Buffer数量限制，0则为动态缓冲区 **/
        } conf_t;

        typedef std::map<bus_id_t, endpoint::ptr_t> endpoint_collection_t;

        struct evt_msg_t {
            typedef std::function<int(const node &, const endpoint *, const connection *, const protocol::msg&, const void *, size_t)>
                on_recv_msg_fn_t;
            typedef std::function<int(const node &, const endpoint *, const connection *, const protocol::msg *m)> on_send_data_failed_fn_t;
            typedef std::function<int(const node &, const endpoint *, const connection *, int, int)> on_error_fn_t;
            typedef std::function<int(const node &, const endpoint *, const connection *, int)> on_reg_fn_t;
            typedef std::function<int(const node &, int)> on_node_down_fn_t;
            typedef std::function<int(const node &, int)> on_node_up_fn_t;
            typedef std::function<int(const node &, const connection *, int)> on_invalid_connection_fn_t;
            typedef std::function<int(const node &, const endpoint *, const connection *, bus_id_t,
                                      const std::vector<std::pair<const void *, size_t> > &)>
                on_custom_cmd_fn_t;
            typedef std::function<int(const node &, endpoint *, int)> on_add_endpoint_fn_t;
            typedef std::function<int(const node &, endpoint *, int)> on_remove_endpoint_fn_t;

            on_recv_msg_fn_t on_recv_msg;
            on_send_data_failed_fn_t on_send_data_failed;
            on_error_fn_t on_error;
            on_reg_fn_t on_reg;
            on_node_down_fn_t on_node_down;
            on_node_up_fn_t on_node_up;
            on_invalid_connection_fn_t on_invalid_connection;
            on_custom_cmd_fn_t on_custom_cmd;
            on_add_endpoint_fn_t on_endpoint_added;
            on_remove_endpoint_fn_t on_endpoint_removed;
        };

        // ================== 用这个来取代C++继承，减少层次结构 ==================
        struct no_stream_channel_t {
            void *channel;
            key_t key;
            int (*proc_fn)(node &, no_stream_channel_t *, time_t, time_t);
            int (*free_fn)(node &, no_stream_channel_t *);
        };

    public:
        static void default_conf(conf_t *conf);

    private:
        node();

        struct io_stream_channel_del {
            void operator()(channel::io_stream_channel *p) const;
        };

    public:
        static ptr_t create();
        ~node();

        /**
         * @brief 数据初始化
         * @return 0或错误码
         */
        int init(bus_id_t id, const conf_t *conf);

        /**
         * @brief 启动连接流程
         * @return 0或错误码
         */
        int start();

        /**
         * @brief 数据重置（释放资源）
         * @return 0或错误码
         */
        int reset();

        /**
         * @brief 执行一帧
         * @param sec 当前时间-秒
         * @param sec 当前时间-微秒
         * @return 本帧处理的消息数
         */
        int proc(time_t sec, time_t usec);

        /**
         * @brief poll libuv
         * @note can not be call in any libuv's callback
         * @return the number of message dispatched
         */
        int poll();

        /**
         * @brief 监听数据接收地址
         * @param addr 监听地址
         * @param is_caddr 是否是控制节点
         * @return 0或错误码
         */
        int listen(const char *addr);

        /**
         * @brief 连接到目标地址
         * @param addr 连接目标地址
         * @return 0或错误码
         */
        int connect(const char *addr);

        /**
         * @brief 连接到目标地址
         * @param addr 连接目标地址
         * @param ep 连接目标的端点
         * @return 0或错误码
         */
        int connect(const char *addr, endpoint *ep);

        /**
         * @brief 断开到目标的连接
         * @param id 目标ID
         * @return 0或错误码
         */
        int disconnect(bus_id_t id);


        /**
         * @brief 发送数据
         * @param tid 发送目标ID
         * @param type 自定义类型，将作为msg.head.type字段传递。可用于业务区分服务类型
         * @param buffer 数据块地址
         * @param s 数据块长度
         * @param require_rsp 是否强制需要回包（默认情况下如果发送成功是没有回包通知的）
         * @return 0或错误码
         * @note 接收端收到的数据很可能不是地址对齐的，所以这里不建议发送内存数据
         *       如果非要发送内存数据的话，一定要memcpy，不能直接类型转换，除非手动设置了地址对齐规则
         */
        int send_data(bus_id_t tid, int type, const void *buffer, size_t s, bool require_rsp = false);

        /**
         * @brief 发送数据消息
         * @param tid 发送目标ID
         * @param mb 消息构建器
         * @return 0或错误码
         */
        int send_data_msg(bus_id_t tid, atbus::protocol::msg &mb);

        /**
         * @brief 发送数据消息
         * @param tid 发送目标ID
         * @param mb 消息构建器
         * @param ep_out 如果发送成功，导出发送目标
         * @param conn_out 如果发送成功，导出发送连接
         * @return 0或错误码
         */
        int send_data_msg(bus_id_t tid, atbus::protocol::msg &mb, endpoint **ep_out, connection **conn_out);

        /**
         * @brief 发送自定义命令消息
         * @param tid 发送目标ID
         * @param arr_buf 自定义消息内容数组
         * @param arr_size 自定义消息内容长度
         * @param arr_count 自定义消息数组个数
         * @return 0或错误码
         */
        int send_custom_cmd(bus_id_t tid, const void *arr_buf[], size_t arr_size[], size_t arr_count);

        /**
         * @brief 发送控制消息
         * @param tid 发送目标ID
         * @param mb 消息构建器
         * @return 0或错误码
         */
        int send_ctrl_msg(bus_id_t tid, atbus::protocol::msg &mb);


        /**
         * @brief 发送控制消息
         * @param tid 发送目标ID
         * @param mb 消息构建器
         * @param ep_out 如果发送成功，导出发送目标
         * @param conn_out 如果发送成功，导出发送连接
         * @return 0或错误码
         */
        int send_ctrl_msg(bus_id_t tid, atbus::protocol::msg &mb, endpoint **ep_out, connection **conn_out);

        /**
         * @brief 发送消息
         * @param tid 发送目标ID
         * @param mb 消息构建器
         * @param fn 获取有效连接的接口
         * @param ep_out 如果发送成功，导出发送目标
         * @param conn_out 如果发送成功，导出发送连接
         * @return 0或错误码
         */
        int send_msg(bus_id_t tid, atbus::protocol::msg &mb, endpoint::get_connection_fn_t fn, endpoint **ep_out, connection **conn_out);

        /**
         * @brief 根据对端ID查找直链的端点
         * @param tid 目标端点ID
         * @return 直连的端点，不存在则返回NULL
         */
        endpoint *get_endpoint(bus_id_t tid);
        const endpoint *get_endpoint(bus_id_t tid) const;

        /**
         * @brief 添加目标端点
         * @param ep 目标端点
         * @return 0或错误码
         */
        int add_endpoint(endpoint::ptr_t ep);

        /**
         * @brief 移除目标端点
         * @param tid 目标端点ID
         * @return 0或错误码
         */
        int remove_endpoint(bus_id_t tid);

        /**
         * @brief 是否有到对端的数据通道(可以向对端发送数据)
         * @note 如果只有控制通道没有数据通道返回false
         * @param tid 目标端点ID
         * @return 有则返回true
         */
        bool is_endpoint_available(bus_id_t tid) const;

    public:
        channel::io_stream_channel *get_iostream_channel();

        inline const endpoint *get_self_endpoint() const { return self_ ? self_.get() : NULL; }

        inline const endpoint *get_parent_endpoint() const { return node_father_.node_.get(); }

        inline const endpoint_collection_t &get_children() const { return node_children_; };

        inline const endpoint_collection_t &get_brother() const { return node_brother_; };

        /**
         * @brief 获取关联的事件管理器,如果未设置则会初始化为默认时间管理器
         * @return 关联的事件管理器
         */
        adapter::loop_t *get_evloop();

    private:
        channel::io_stream_conf *get_iostream_conf();

    public:
        inline bus_id_t get_id() const { return self_ ? self_->get_id() : 0; }
        inline const conf_t &get_conf() const { return conf_; }

        inline bool check(flag_t::type f) const { return flags_.test(f); }
        inline state_t::type get_state() const { return state_; }

        ptr_t get_watcher();

        bool is_child_node(bus_id_t id) const;
        bool is_brother_node(bus_id_t id) const;
        bool is_parent_node(bus_id_t id) const;

        static int get_pid();
        static const std::string &get_hostname();
        static bool set_hostname(const std::string &hn);

        inline const std::list<std::string> &get_listen_list() const { return self_->get_listen(); }

        bool add_proc_connection(connection::ptr_t conn);
        bool remove_proc_connection(const std::string &conn_key);

        bool add_connection_timer(connection::ptr_t conn);

        time_t get_timer_sec() const;

        time_t get_timer_usec() const;

        void on_recv(connection *conn, protocol::msg *m, int status, int errcode);

        void on_recv_data(const endpoint *ep, connection *conn, const protocol::msg& m, const void *buffer, size_t s) const;

        void on_send_data_failed(const endpoint *, const connection *, const protocol::msg *m);

        int on_error(const char *file_path, size_t line, const endpoint *, const connection *, int, int);
        int on_disconnect(const connection *);
        int on_new_connection(connection *);
        int on_shutdown(int reason);
        int on_reg(const endpoint *, const connection *, int);
        int on_actived();
        int on_parent_reg_done();
        int on_custom_cmd(const endpoint *, const connection *, bus_id_t from,
                          const std::vector<std::pair<const void *, size_t> > &cmd_args);

        /**
         * @brief 关闭node
         * @param reason 关闭原因ID
         * @note 如果需要在关闭前执行资源回收，可以在on_node_down_fn_t回调中返回非0值来阻止node的reset操作，
         *       并在资源释放完成后再调用shutdown函数，在第二次on_node_down_fn_t回调中返回0值
         *
         * @note 或者也可以通过ref_object和unref_object来标记和解除数据引用，reset函数会执行事件loop知道所有引用的资源被移除
         */
        int shutdown(int reason);


        /** do not use this directly **/
        int fatal_shutdown(const char *file_path, size_t line, const endpoint *, const connection *, int status, int errcode);

        inline const detail::buffer_block *get_temp_static_buffer() const { return static_buffer_; }
        inline detail::buffer_block *get_temp_static_buffer() { return static_buffer_; }

        int ping_endpoint(endpoint &ep);

        int push_node_sync();

        int pull_node_sync();

        uint32_t alloc_msg_seq();

        void add_check_list(const endpoint::ptr_t &ep);

        void set_on_recv_handle(evt_msg_t::on_recv_msg_fn_t fn);
        evt_msg_t::on_recv_msg_fn_t get_on_recv_handle() const;

        void set_on_send_data_failed_handle(evt_msg_t::on_send_data_failed_fn_t fn);
        evt_msg_t::on_send_data_failed_fn_t get_on_send_data_failed_handle() const;

        void set_on_error_handle(evt_msg_t::on_error_fn_t fn);
        evt_msg_t::on_error_fn_t get_on_error_handle() const;

        void set_on_register_handle(evt_msg_t::on_reg_fn_t fn);
        evt_msg_t::on_reg_fn_t get_on_register_handle() const;

        void set_on_shutdown_handle(evt_msg_t::on_node_down_fn_t fn);
        evt_msg_t::on_node_down_fn_t get_on_shutdown_handle() const;

        void set_on_available_handle(evt_msg_t::on_node_up_fn_t fn);
        evt_msg_t::on_node_up_fn_t get_on_available_handle() const;

        void set_on_invalid_connection_handle(evt_msg_t::on_invalid_connection_fn_t fn);
        evt_msg_t::on_invalid_connection_fn_t get_on_invalid_connection_handle() const;

        void set_on_custom_cmd_handle(evt_msg_t::on_custom_cmd_fn_t fn);
        evt_msg_t::on_custom_cmd_fn_t get_on_custom_cmd_handle() const;

        void set_on_add_endpoint_handle(evt_msg_t::on_add_endpoint_fn_t fn);
        evt_msg_t::on_add_endpoint_fn_t get_on_add_endpoint_handle() const;

        void set_on_remove_endpoint_handle(evt_msg_t::on_remove_endpoint_fn_t fn);
        evt_msg_t::on_remove_endpoint_fn_t get_on_remove_endpoint_handle() const;

        void ref_object(void *);
        void unref_object(void *);

    private:
        static endpoint *find_child(endpoint_collection_t &coll, bus_id_t id);

        bool insert_child(endpoint_collection_t &coll, endpoint::ptr_t ep);

        bool remove_child(endpoint_collection_t &coll, bus_id_t id);

        bool remove_collection(endpoint_collection_t &coll);

        /**
         * @brief 增加错误计数，如果超出容忍值则移除
         * @return 是否被移除
         */
        bool add_endpoint_fault(endpoint &ep);

        /**
         * @brief 添加到ping列表
         */
        void add_ping_timer(endpoint::ptr_t &ep);

    public:
        void stat_add_dispatch_times();

    private:
        // ============ 基础信息 ============
        // ID
        endpoint::ptr_t self_;
        state_t::type state_;
        std::bitset<flag_t::EN_FT_MAX> flags_;

        // 配置
        conf_t conf_;
        std::weak_ptr<node> watcher_; // just like std::shared_from_this<T>
        util::lock::seq_alloc_u32 msg_seq_alloc_;

        // 引用的资源标记（释放时要保证这些资源引用被移除）
        std::set<void *> ref_objs_;

        // ============ IO事件数据 ============
        // 事件分发器
        adapter::loop_t *ev_loop_;
        std::unique_ptr<channel::io_stream_channel, io_stream_channel_del> iostream_channel_;
        std::unique_ptr<channel::io_stream_conf> iostream_conf_;
        evt_msg_t event_msg_;

        // ============ 定时器 ============
        typedef struct {
            template <typename TObj>
            struct timer_desc_ls {
                typedef std::pair<time_t, TObj> pair_type;
                typedef std::list<pair_type> type;
            };

            time_t sec;
            time_t usec;

            time_t node_sync_push;                                   // 节点变更推送
            time_t father_opr_time_point;                            // 父节点操作时间（断线重连或Ping）
            timer_desc_ls<std::weak_ptr<endpoint> >::type ping_list; // 定时ping
            timer_desc_ls<connection::ptr_t>::type connecting_list;  // 未完成连接（正在网络连接或握手）
            std::list<endpoint::ptr_t> pending_check_list_;          // 待检测列表
        } evt_timer_t;
        evt_timer_t event_timer_;

        // 轮训接收通道集
        detail::buffer_block *static_buffer_;
        detail::auto_select_map<std::string, connection::ptr_t>::type proc_connections_;

        // 基于事件的通道信息
        // 基于事件的通道超时收集

        // ============ 节点逻辑关系数据 ============
        // 父节点
        struct father_info_t {
            endpoint::ptr_t node_;
        };
        father_info_t node_father_;

        // 兄弟节点
        endpoint_collection_t node_brother_;

        // 子节点
        endpoint_collection_t node_children_;

        // 全局路由表

        // 统计信息
        struct stat_info_t {
            size_t dispatch_times;

            stat_info_t();
        };
        stat_info_t stat_;

        // 调试辅助函数
    public:
        void (*on_debug)(const char *file_path, size_t line, const node &, const endpoint *, const connection *, const protocol::msg *,
                         const char *fmt, ...);
    };
}


#define ATBUS_FUNC_NODE_ERROR(n, ep, conn, status, errorcode) (n).on_error(__FILE__, __LINE__, (ep), (conn), (status), (errorcode))
#define ATBUS_FUNC_NODE_FATAL_SHUTDOWN(n, ep, conn, status, errorcode) \
    (n).fatal_shutdown(__FILE__, __LINE__, (ep), (conn), (status), (errorcode))

#ifdef _MSC_VER
#define ATBUS_FUNC_NODE_DEBUG(n, ep, conn, m, fmt, ...)                             \
    if ((n).on_debug) {                                                             \
        (n).on_debug(__FILE__, __LINE__, (n), (ep), (conn), (m), fmt, __VA_ARGS__); \
    }
#else
#define ATBUS_FUNC_NODE_DEBUG(n, ep, conn, m, fmt, args...)                    \
    if ((n).on_debug) {                                                        \
        (n).on_debug(__FILE__, __LINE__, (n), (ep), (conn), (m), fmt, ##args); \
    }
#endif


#endif /* LIBATBUS_NODE_H_ */
