/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/JsonSimpleWriter.h
 */

#pragma once

#include <base/base.h>
#include <fatfs/fatfs.h>
#include <rapidjson/fast.h>

namespace json
{

class JsonSimpleWriter
{
public:
    JsonSimpleWriter(fatfs::File& f);
    ~JsonSimpleWriter();

    void PathKey(const char* path, const char* comment = NULL);

    void Float(float f) { wr.Float(f); }
    void Bool(bool b) { wr.Bool(b); }
    void Int(int n) { wr.Int(n); }
    void String(Span s) { wr.String(s.Pointer(), s.Length()); }

private:
    using WR = rapidjson::PrettyWriter<fatfs::RapidJsonOutputStream>;

    fatfs::RapidJsonOutputStream os;
    WR wr;
    const char* path = "";

    const char* UpdatePath(const char* path);
};

}
