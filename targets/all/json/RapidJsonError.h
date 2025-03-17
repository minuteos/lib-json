/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/RapidJsonError.h
 */

#pragma once

#include <kernel/Exception.h>

#include <rapidjson/fast.h>

namespace json
{

DECLARE_EXCEPTION(RapidJsonParseError);

static inline rapidjson::ParseErrorCode GetParseError(const ::kernel::Exception& ex)
{
    ASSERT(ex.Type() == RapidJsonParseError);
    return rapidjson::ParseErrorCode(uintptr_t(ex.Value()) >> 24);
}

static inline const char* GetParseErrorMessage(const ::kernel::Exception& ex)
{
    return rapidjson::GetParseError_En(GetParseError(ex));
}

static inline size_t GetParseErrorOffset(const ::kernel::Exception& ex)
{
    ASSERT(ex.Type() == RapidJsonParseError);
    return ex.Value() & MASK(24);
}

}
