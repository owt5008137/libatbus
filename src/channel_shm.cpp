/**
 * @brief 所有channel文件的模式均为 c + channel<br />
 *        使用c的模式是为了简单、结构清晰并且避免异常<br />
 *        附带c++的部分是为了避免命名空间污染并且c++的跨平台适配更加简单
 */

#include "lock/atomic_int_type.h"
#include <assert.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <stdint.h>

#include "detail/libatbus_channel_export.h"
#include "detail/libatbus_error.h"


// spin_lock and lock_holder will include Windows.h, which should be included after Winsock2.h
#include "common/string_oprs.h"
#include "lock/lock_holder.h"
#include "lock/spin_lock.h"

#ifdef WIN32
#include <Windows.h>

#ifdef _MSC_VER
#include <atlconv.h>
#endif

#ifdef UNICODE
#define ATBUS_VC_TEXT(x) A2W(x)
#else
#define ATBUS_VC_TEXT(x) x
#endif

#else
#include <unistd.h>
#endif
#ifdef ATBUS_CHANNEL_SHM

namespace atbus {
    namespace channel {

        struct shm_channel {};

        struct shm_conf {};

        typedef union {
            shm_channel *shm;
            mem_channel *mem;
        } shm_channel_switcher;

        typedef union {
            const shm_conf *shm;
            const mem_conf *mem;
        } shm_conf_cswitcher;

#ifdef WIN32
        typedef struct {
            HANDLE handle;
            LPCTSTR buffer;
            size_t size;
            size_t reference_count;
        } shm_mapped_record_type;
#else
        typedef struct {
            int shm_id;
            void *buffer;
            size_t size;
            size_t reference_count;
        } shm_mapped_record_type;
#endif

        static std::map<key_t, shm_mapped_record_type> shm_mapped_records;
        static ::util::lock::spin_lock shm_mapped_records_lock;

        static int shm_close_buffer(key_t shm_key) {
            ::util::lock::lock_holder< ::util::lock::spin_lock> lock_guard(shm_mapped_records_lock);

            std::map<key_t, shm_mapped_record_type>::iterator iter = shm_mapped_records.find(shm_key);
            if (shm_mapped_records.end() == iter) return EN_ATBUS_ERR_SHM_NOT_FOUND;

            assert(iter->second.reference_count > 0);
            if (iter->second.reference_count > 1) {
                --iter->second.reference_count;
                return EN_ATBUS_ERR_SUCCESS;
            } else {
                iter->second.reference_count = 0;
            }

            shm_mapped_record_type record = iter->second;
            shm_mapped_records.erase(iter);

#ifdef WIN32
            UnmapViewOfFile(record.buffer);
            CloseHandle(record.handle);
#else
            int res = shmdt(record.buffer);
            if (-1 == res) return EN_ATBUS_ERR_SHM_GET_FAILED;
#endif

            return EN_ATBUS_ERR_SUCCESS;
        }

        static int shm_open_buffer(key_t shm_key, size_t len, void **data, size_t *real_size, bool create) {
            ::util::lock::lock_holder< ::util::lock::spin_lock> lock_guard(shm_mapped_records_lock);

            shm_mapped_record_type shm_record;

            // 已经映射则直接返回
            {
                std::map<key_t, shm_mapped_record_type>::iterator iter = shm_mapped_records.find(shm_key);
                if (shm_mapped_records.end() != iter) {
                    if (data) *data = (void *)iter->second.buffer;
                    if (real_size) *real_size = iter->second.size;
                    ++iter->second.reference_count;
                    return EN_ATBUS_ERR_SUCCESS;
                }
            }

#ifdef _WIN32
#ifdef _MSC_VER
            USES_CONVERSION;
#endif
            memset(&shm_record, 0, sizeof(shm_record));
            SYSTEM_INFO si;
            ::GetSystemInfo(&si);
            // size_t page_size = static_cast<std::size_t>(si.dwPageSize);

            char shm_file_name[64] = {0};
            // Use Global\\ prefix requires the SeCreateGlobalPrivilege privilege, so we do not use it
            UTIL_STRFUNC_SNPRINTF(shm_file_name, sizeof(shm_file_name), "Global\\libatbus_win_shm_%ld.bus", shm_key);

            // 首先尝试直接打开
            shm_record.handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,         // read/write access
                                                FALSE,                       // do not inherit the name
                                                ATBUS_VC_TEXT(shm_file_name) // name of mapping object
                                                );
            if (NULL != shm_record.handle) {
                shm_record.buffer = (LPTSTR)MapViewOfFile(shm_record.handle,   // handle to map object
                                                          FILE_MAP_ALL_ACCESS, // read/write permission
                                                          0, 0, len);

                if (NULL == shm_record.buffer) {
                    CloseHandle(shm_record.handle);
                    return EN_ATBUS_ERR_SHM_GET_FAILED;
                }

                if (data) *data = (void *)shm_record.buffer;
                if (real_size) *real_size = len;

                shm_record.size = len;
                shm_record.reference_count = 1;
                shm_mapped_records[shm_key] = shm_record;
                return EN_ATBUS_ERR_SUCCESS;
            }

            // 如果允许创建则创建
            if (!create) return EN_ATBUS_ERR_SHM_GET_FAILED;

            shm_record.handle = CreateFileMapping(INVALID_HANDLE_VALUE,        // use paging file
                                                  NULL,                        // default security
                                                  PAGE_READWRITE,              // read/write access
                                                  0,                           // maximum object size (high-order DWORD)
                                                  static_cast<DWORD>(len),     // maximum object size (low-order DWORD)
                                                  ATBUS_VC_TEXT(shm_file_name) // name of mapping object
                                                  );

            if (NULL == shm_record.handle) return EN_ATBUS_ERR_SHM_GET_FAILED;

            shm_record.buffer = (LPTSTR)MapViewOfFile(shm_record.handle,   // handle to map object
                                                      FILE_MAP_ALL_ACCESS, // read/write permission
                                                      0, 0, len);

            if (NULL == shm_record.buffer) return EN_ATBUS_ERR_SHM_GET_FAILED;

            shm_record.size = len;
            shm_record.reference_count = 1;
            shm_mapped_records[shm_key] = shm_record;

            if (data) *data = (void *)shm_record.buffer;
            if (real_size) *real_size = len;

#else
            // len 长度对齐到分页大小
            size_t page_size = ::sysconf(_SC_PAGESIZE);
            len = (len + page_size - 1) & (~(page_size - 1));

            int shmflag = 0666;
            if (create) shmflag |= IPC_CREAT;

#ifdef __linux__
            // linux下阻止从交换分区分配物理页
            shmflag |= SHM_NORESERVE;

// 临时关闭大页表功能，等后续增加了以下判定之后再看情况加回来
// 使用大页表要先判定 /proc/meminfo 内的一些字段内容，再配置大页表
// -- Hugepagesize: 大页表的分页大小，如果ATBUS_MACRO_HUGETLB_SIZE小于这个值，要对齐到这个值
// -- HugePages_Total: 大页表总大小
// -- HugePages_Free: 大页表可用大小，如果可用值小于需要分配的空间，也不能用大页表
//#ifdef ATBUS_MACRO_HUGETLB_SIZE
//            // 如果大于4倍的大页表，则对齐到大页表并使用大页表
//            if (len > (4 * ATBUS_MACRO_HUGETLB_SIZE)) {
//                len = (len + (ATBUS_MACRO_HUGETLB_SIZE)-1) & (~((ATBUS_MACRO_HUGETLB_SIZE)-1));
//                shmflag |= SHM_HUGETLB;
//            }
//#endif

#endif
            shm_record.shm_id = shmget(shm_key, len, shmflag);
            if (-1 == shm_record.shm_id) return EN_ATBUS_ERR_SHM_GET_FAILED;

            // 获取实际长度
            {
                struct shmid_ds shm_info;
                if (shmctl(shm_record.shm_id, IPC_STAT, &shm_info)) return EN_ATBUS_ERR_SHM_GET_FAILED;

                shm_record.size = shm_info.shm_segsz;
            }


            // 获取地址
            shm_record.buffer = shmat(shm_record.shm_id, NULL, 0);
            shm_record.reference_count = 1;
            shm_mapped_records[shm_key] = shm_record;

            if (data) *data = shm_record.buffer;
            if (real_size) {
                *real_size = shm_record.size;
            }

#endif

            return EN_ATBUS_ERR_SUCCESS;
        }

        int shm_attach(key_t shm_key, size_t len, shm_channel **channel, const shm_conf *conf) {
            shm_channel_switcher channel_s;
            shm_conf_cswitcher conf_s;
            conf_s.shm = conf;

            size_t real_size;
            void *buffer;
            int ret = shm_open_buffer(shm_key, len, &buffer, &real_size, false);
            if (ret < 0) return ret;

            ret = mem_attach(buffer, real_size, &channel_s.mem, conf_s.mem);
            if (ret < 0) {
                shm_close_buffer(shm_key);
                return ret;
            }

            if (channel) *channel = channel_s.shm;

            return ret;
        }

        int shm_init(key_t shm_key, size_t len, shm_channel **channel, const shm_conf *conf) {
            shm_channel_switcher channel_s;
            shm_conf_cswitcher conf_s;
            conf_s.shm = conf;

            size_t real_size;
            void *buffer;
            int ret = shm_open_buffer(shm_key, len, &buffer, &real_size, true);
            if (ret < 0) return ret;

            ret = mem_init(buffer, real_size, &channel_s.mem, conf_s.mem);
            if (ret < 0) {
                shm_close_buffer(shm_key);
                return ret;
            }

            if (channel) *channel = channel_s.shm;

            return ret;
        }

        int shm_close(key_t shm_key) { return shm_close_buffer(shm_key); }

        int shm_send(shm_channel *channel, const void *buf, size_t len) {
            shm_channel_switcher switcher;
            switcher.shm = channel;
            return mem_send(switcher.mem, buf, len);
        }

        int shm_recv(shm_channel *channel, void *buf, size_t len, size_t *recv_size) {
            shm_channel_switcher switcher;
            switcher.shm = channel;
            return mem_recv(switcher.mem, buf, len, recv_size);
        }

        std::pair<size_t, size_t> shm_last_action() { return mem_last_action(); }

        void shm_show_channel(shm_channel *channel, std::ostream &out, bool need_node_status, size_t need_node_data) {
            shm_channel_switcher switcher;
            switcher.shm = channel;
            mem_show_channel(switcher.mem, out, need_node_status, need_node_data);
        }
    }
}

#endif
