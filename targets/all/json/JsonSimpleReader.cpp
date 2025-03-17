/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/JsonSimpleReader.cpp
 */

#include "JsonSimpleReader.h"

#include <json/json.h>

namespace json
{

bool JsonSimpleReader::Push(bool array)
{
    depth++;
    if (depth < countof(stack))
    {
        stack[depth].array = array;
    }
    return true;
}

bool JsonSimpleReader::Key(const char* s, size_t len, bool copy)
{
    uint32_t key = fnv1a(s, len, FNV1_BASIS);
    if (depth < countof(stack))
    {
        char* p = path;
        if (depth > 0)
        {
            stack[depth].path = fnv1a(s, len, fnv1a(&pathSeparator, 1, stack[depth - 1].path));
            p += stack[depth - 1].offset;
            *p++ = pathSeparator;
        }
        else
        {
            stack[depth].path = key;
        }
        stack[depth].key = key;
        for (size_t i = 0; i < len && p < endof(path); i++)
        {
            *p++ = s[i];
        }
        *p = 0;
        stack[depth].offset = p - path;
    }
    return true;
}

bool JsonSimpleReader::ProcessValue(int n, float f, enum ValueType type)
{
    if (depth >= countof(stack))
    {
        // too deep, just ignore...
        return true;
    }

    this->n = n;
    this->f = f;
    this->vt = type;
    return callback(*this);
}

bool JsonSimpleReader::ProcessStringValue(Span s)
{
    if (depth >= countof(stack))
    {
        // too deep, just ignore...
        return true;
    }

    this->s = s;
    this->n = this->f = s.Length();
    this->vt = ValueType::String;
    return callback(*this);
}

async(JsonSimpleReader::Process, fatfs::File& file, Delegate<bool, const JsonSimpleReader&> callback)
async_def(
    fatfs::File* f;
    JsonSimpleReader* rdr;
    rapidjson::ParseResult res;
    void Parse()
    {
        fatfs::RapidJsonInputStream is(*f);
        res = rdr->rd.Parse(is, *rdr);
        delete rdr;
    }
)
{
    f.f = &file;
    // minimze stack usage by allocating the reader on the heap
    f.rdr = new JsonSimpleReader(callback);
    await(kernel::Worker::Run, GetMethodDelegate(&f, Parse));
    if (f.res.IsError())
    {
        async_throw(RapidJsonParseError, ((f.res.Code() << 24) | f.res.Offset()));
    }
    else
    {
        async_return(true);
    }
}
async_end

async(JsonSimpleReader::Process, const char* path, Delegate<bool, const JsonSimpleReader&> callback)
async_def(
    fatfs::File f;
    AsyncCatchResult res;
)
{
    await(f.f.Open, path);  // failing to open the file can throw
    f.res = await_catch(Process, f.f, callback);
    await_catch(f.f.Close); // ignore error
    async_return_forward(f.res);
}
async_end

}
