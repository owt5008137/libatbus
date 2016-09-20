#pragma once

#ifndef LIBATBUS_DETAIL_LIBATBUS_CONFIG_H_
#define LIBATBUS_DETAIL_LIBATBUS_CONFIG_H_

#include <stdint.h>

#ifndef ATBUS_MACRO_BUSID_TYPE
#define ATBUS_MACRO_BUSID_TYPE uint64_t
#endif

#ifndef ATBUS_MACRO_MSG_LIMIT
#define ATBUS_MACRO_MSG_LIMIT 65536
#endif

#ifndef ATBUS_MACRO_CONNECTION_CONFIRM_TIMEOUT
#define ATBUS_MACRO_CONNECTION_CONFIRM_TIMEOUT 30
#endif

#ifndef ATBUS_MACRO_CONNECTION_BACKLOG
#define ATBUS_MACRO_CONNECTION_BACKLOG 128
#endif


#ifndef ATBUS_MACRO_DATA_SMALL_SIZE
#define ATBUS_MACRO_DATA_SMALL_SIZE 512
#endif

#if defined(__cplusplus) &&                                                                                         \
    (__cplusplus >= 201103L || (defined(_MSC_VER) && (_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500) || \
     (defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)))
#include <unordered_map>
#include <unordered_set>
#define ATBUS_ADVANCE_TYPE_MAP(...) std::unordered_map<__VA_ARGS__>
#define ATBUS_ADVANCE_TYPE_SET(...) std::unordered_set<__VA_ARGS__>
#else
#include <map>
#include <set>
#define ATBUS_ADVANCE_TYPE_MAP(...) std::map<__VA_ARGS__>
#define ATBUS_ADVANCE_TYPE_SET(...) std::set<__VA_ARGS__>
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#define ATBUS_MACRO_ENABLE_STATIC_ASSERT 1
#elif defined(_MSC_VER) && _MSC_VER >= 1600
#define ATBUS_MACRO_ENABLE_STATIC_ASSERT 1
#endif

#endif
