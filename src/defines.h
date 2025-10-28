#ifndef SOLOMON_DEFINES_H
#define SOLOMON_DEFINES_H

#include <stdint.h>

/** @brief 32-bit boolean type, used for APIs which require it */
typedef int b32;

/** @brief Unsigned 8-bit integer */
typedef uint8_t u8;

/** @brief Unsigned 16-bit integer */
typedef uint16_t u16;

/** @brief Unsigned 32-bit integer */
typedef uint32_t u32;

/** @brief Unsigned 64-bit integer */
typedef uint64_t u64;

// Signed int types.

/** @brief Signed 8-bit integer */
typedef int8_t i8;

/** @brief Signed 16-bit integer */
typedef int16_t i16;

/** @brief Signed 32-bit integer */
typedef int32_t i32;

/** @brief Signed 64-bit integer */
typedef int64_t i64;

// Floating point types

/** @brief 32-bit floating point number */
typedef float f32;

/** @brief 64-bit floating point number */
typedef double f64;

// --- Memory sizes --- //
// 1024 Bytes
#define KIB 1024
// 1024 Kibibytes || 1,048,576 Bytes
#define MIB 1024 * KIB
// 1024 Mebibytes || 1,073,741,824 Bytes
#define GIB 1024 * MIB

#endif // SOLOMON_DEFINES_H