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

    ALWAYS_INLINE void Null() { wr.Null(); }
    ALWAYS_INLINE void Float(float f) { wr.Float(f); }
    ALWAYS_INLINE void Bool(bool b) { wr.Bool(b); }
    ALWAYS_INLINE void Int(int n) { wr.Int(n); }
    ALWAYS_INLINE void String(Span s) { wr.String(s.Pointer(), s.Length()); }

    ALWAYS_INLINE void StartObject() { wr.StartObject(); }
    ALWAYS_INLINE void StartObject(Span key) { Key(key); StartObject(); }
    ALWAYS_INLINE void EndObject() { wr.EndObject(); }

    ALWAYS_INLINE void StartArray() { wr.StartArray(); }
    ALWAYS_INLINE void StartArray(Span key) { Key(key); StartArray(); }
    ALWAYS_INLINE void EndArray() { wr.EndArray(); }

    ALWAYS_INLINE void Key(Span key) { wr.Key(key.Pointer(), key.Length()); }
    ALWAYS_INLINE void CommentLine(Span comment) { wr.CommentLine(comment.Pointer(), comment.Length()); }
    ALWAYS_INLINE void CommentInline(Span comment) { wr.CommentInline(comment.Pointer(), comment.Length()); }

    template<typename T> ALWAYS_INLINE void Value(Span key, const T& value)
    {
        Key(key);
        Value<T>(value);
    }

    template<typename T> ALWAYS_INLINE void SingleOrArray(Span s)
    {
        if (s.Length() / sizeof(T) == 1) { Value(s.Element<T>()); }
        else { Array<T>(s); }
    }

    template<typename T> ALWAYS_INLINE void Array(Span s)
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

template<> ALWAYS_INLINE void JsonSimpleWriter::Value<float>(const float& v) { Float(v); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<unsigned short>(const unsigned short& s) { Int(s); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<short>(const short& s) { Int(s); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<unsigned>(const unsigned& u) { Int(u); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<int>(const int& i) { Int(i); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<unsigned long>(const unsigned long& u) { Int(u); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<long>(const long& l) { Int(l); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<bool>(const bool& b) { Bool(b); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<const char*>(const char* const& s) { wr.String(s); }
template<> ALWAYS_INLINE void JsonSimpleWriter::Value<Span>(const Span& s) { wr.String(s.Pointer(), s.Length()); }

}
