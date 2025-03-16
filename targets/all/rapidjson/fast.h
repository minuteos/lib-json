/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * cortex-m/rapidjson/fast.h
 *
 * A fast pre-configured for the specifics of Cortex-M microcontrollers
 */

#pragma once

#include <base/base.h>
#include <base/float.h>

#ifndef RAPIDJSON_ASSERT
#define RAPIDJSON_ASSERT(x)
#endif

#ifndef RAPIDJSON_PARSE_DEFAULT_FLAGS
#define RAPIDJSON_PARSE_DEFAULT_FLAGS   kParseCommentsFlag | kParseTrailingCommasFlag
#endif

#define RAPIDJSON_FAST32    1
#define RAPIDJSON_FAST32_FTOA(f, buf, max)   fast_ftoa(f, buf)
#define RAPIDJSON_FAST32_ITOF(s, m, k)       fast_itof(s, m, k)

#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
