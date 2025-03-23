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

    void Null() { wr.Null(); }
    void Float(float f) { wr.Float(f); }
    void Bool(bool b) { wr.Bool(b); }
    void Int(int n) { wr.Int(n); }
    void String(Span s) { wr.String(s.Pointer(), s.Length()); }

    template<typename T> void SingleOrArray(Span s)
    {
        if (s.Length() / sizeof(T) == 1) { Value(s.Element<T>()); }
        else { Array<T>(s); }
    }

    template<typename T> void Array(Span s)
    {
        size_t len = s.Length() / sizeof(T);
        wr.StartArray();
        for (size_t i = 0; i < len; i++) { Value<T>(s.Element<T>(i)); }
        wr.EndArray();
    }

    template<typename T> void Value(const T& v);

private:
    using WR = rapidjson::PrettyWriter<fatfs::RapidJsonOutputStream>;

    fatfs::RapidJsonOutputStream os;
    WR wr;
    const char* path = "";

    const char* UpdatePath(const char* path);
};

template<> inline void JsonSimpleWriter::Value<float>(const float& v) { Float(v); }
template<> inline void JsonSimpleWriter::Value<int>(const int& i) { Int(i); }
template<> inline void JsonSimpleWriter::Value<bool>(const bool& b) { Bool(b); }

}
