#pragma once

#ifndef LIBATBUS_DETAIL_CRC64_H_
#define LIBATBUS_DETAIL_CRC64_H_

#include <stddef.h>
#include <stdint.h>

namespace atbus {
    namespace detail {
        uint64_t crc64(uint64_t crc, const unsigned char *s, size_t l);
    }
}

#endif
