/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/JsonReader.h
 */

#pragma once

#include <base/base.h>
#include <fatfs/fatfs.h>
#include <rapidjson/fast.h>

namespace json
{

enum struct ValueType
{
    Null,
    Boolean,
    Integer,
    Float,
    String,
    Object,
    Array,
};

class JsonSimpleReader
{
public:
    JsonSimpleReader(Delegate<bool, const JsonSimpleReader&> callback, char pathSeparator = '.')
        : pathSeparator(pathSeparator), callback(callback)
    {
    }

    #pragma region RapidJson handler interface methods

    bool Null() { return ProcessValue(0, NAN, ValueType::Null); }
    bool Bool(bool b) { return ProcessValue(b, NAN, ValueType::Boolean); }
    bool Int(int n) { return ProcessValue(n, n, ValueType::Integer); }
    bool Uint(unsigned n) { return ProcessValue(n, n, ValueType::Integer); }
    bool Float(float n) { return ProcessValue(n, n, ValueType::Float); }
    bool String(const char* s, size_t len, bool copy) { return ProcessStringValue(Span(s, len)); }

    bool StartArray() { return Push(true); }
    bool EndArray(size_t cnt) { return Pop(cnt); }

    bool StartObject() { return Push(false); }
    bool EndObject(size_t cnt) { return Pop(cnt); }

    bool Key(const char* s, size_t len, bool copy);

    #pragma endregion

    #pragma region Callback access

    /*! Path of the current key, may be truncated */
    const char* Path() const { return path; }
    /*! Current key, may be truncated */
    const char* Key() const { return path + stack[depth].offset; }

    /*! FNV1a of the entire path including the current key */
    ID PathFNV() const { return stack[depth].path; }
    /*! FNV1a of the entire path excluding the current key */
    ID ParentFNV() const { return depth ? stack[depth - 1].path : FNV1_BASIS; }
    /*! FNV1a of the current key, if @ref IsObject is true */
    ID KeyFNV() const { return stack[depth].key; }
    /*! Index of the current element, if @ref IsArray is true */
    size_t Index() const { return stack[depth].key; }
    /*! Current value is an element of an array */
    bool IsArray() const { return stack[depth].array; }
    /*! Current value is an element of an object */
    bool IsObject() const { return !stack[depth].array; }

    /*! Type of the value being processed */
    enum ValueType ValueType() const { return vt; }
    /*! Name of the type of the value being processed */
    const char* ValueTypeName() const { return STRINGS("Null", "Boolean", "Integer", "Float", "String", "Object", "Array")[int(vt)]; }
    /*! Value being processed represented as an integer */
    int IntValue() const { return n; }
    /*! Value being processed represented as a float */
    float FloatValue() const { return f; }
    /*! Value being processed represented as a string */
    Span StringValue() const { return s; }

    #pragma endregion

    static async(Process, fatfs::File& f, Delegate<bool, const JsonSimpleReader&> callback);
    static async(Process, const char* path, Delegate<bool, const JsonSimpleReader&> callback);

private:
    struct
    {
        uint32_t key;
        uint32_t path;
        uint16_t offset;
        bool array;
    } stack[8];

    size_t depth = ~0u;
    enum ValueType vt;
    char pathSeparator;
    int n;
    float f;
    Span s;
    Delegate<bool, const JsonSimpleReader&> callback;
    char path[63];
    char zero;
    rapidjson::Reader rd;

    bool Push(bool array);
    bool Pop(size_t cnt);

    bool ProcessValue(int i, float f, enum ValueType type);
    bool ProcessStringValue(Span s);
    void UpdateArrayIndex();
};

}
