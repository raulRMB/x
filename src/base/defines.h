#ifndef R_DEFINES_H
#define R_DEFINES_H

#include <cstdint>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH_F (f32)WINDOW_WIDTH
#define WINDOW_HEIGHT_F (f32)WINDOW_HEIGHT
#define WINDOW_TITLE "x"

#define X_WINDOWING_API_SDL 1

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef unsigned char byte;

typedef glm::vec2 v2;
typedef glm::vec3 v3;
typedef glm::vec4 v4;

typedef glm::mat2 m2;
typedef glm::mat3 m3;
typedef glm::mat4 m4;

typedef glm::quat q4;

typedef glm::ivec2 iv2;
typedef glm::ivec3 iv3;
typedef glm::ivec4 iv4;

#endif // R_DEFINES_H