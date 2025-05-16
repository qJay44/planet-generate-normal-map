#pragma once

#include <stdint.h>
#include <filesystem>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u8 byte;
typedef s8 sbyte;

typedef std::filesystem::path fspath;

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::dvec2;
using glm::dvec3;
using glm::dvec4;

using glm::mat4;

