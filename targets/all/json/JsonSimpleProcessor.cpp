/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/JsonSimpleProcessor.cpp
 */

#include "JsonSimpleProcessor.h"

#include <json/json.h>

namespace json
{

async_res_t JsonSimpleProcessor::Parse()
{
    fatfs::RapidJsonInputStream is(*file);

    OnStart();
    // minimze stack usage by allocating the reader on the heap
    auto rdr = new JsonSimpleReader(GetMethodDelegate(this, OnNode));
    auto jsonRes = rdr->Parse(is);
    delete rdr;

    err = {};
    if (file->GetError())
    {
        err = _ASYNC_RES(file->GetError(), fatfs::Error);
    }
    if (jsonRes.IsError())
    {
        err = _ASYNC_RES((jsonRes.Code() << 24) | jsonRes.Offset(), RapidJsonParseError);
    }

    if (err.Success())
    {
        OnEnd();
    }
    else
    {
        OnError();
    }

    return err;
}

async(JsonSimpleProcessor::Process, fatfs::File& file)
async_def()
{
    this->file = &file;
    this->path = NULL;
    async_return(await(kernel::Worker::Run, GetMethodDelegate(this, Parse)));
}
async_end

async(JsonSimpleProcessor::Process, const char* path)
async_def(
    fatfs::File f;
)
{
    await(f.f.Open, path);  // failing to open the file can throw

    this->file = &f.f;
    this->path = path;
    await_catch(kernel::Worker::Run, GetMethodDelegate(this, Parse));
    await_catch(f.f.Close); // ignore error
    async_return_forward(err);
}
async_end

}
