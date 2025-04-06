/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/JsonSimpleProcessor.h
 */

#pragma once

#include <base/base.h>
#include <fatfs/fatfs.h>

#include "JsonSimpleReader.h"

namespace json
{

class JsonSimpleProcessor
{
public:
    async(Process, fatfs::File& f);
    async(Process, const char* path);

protected:
    fatfs::File& File() const { return *file; }
    const char* Path() const { return path; }

    constexpr kernel::Exception Exception() const { return err.Exception(); }
    void SetResult(intptr_t value) { err = value; }

    virtual void OnStart() { }
    virtual bool OnNode(const JsonSimpleReader& rdr) = 0;
    virtual void OnEnd() { }
    virtual void OnError() { }

private:
    fatfs::File* file = NULL;
    const char* path = NULL;
    AsyncCatchResult err;

    async_res_t Parse();
};

}
