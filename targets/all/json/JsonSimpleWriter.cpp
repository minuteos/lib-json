/*
 * Copyright (c) 2025 triaxis s.r.o.
 * Licensed under the MIT license. See LICENSE.txt file in the repository root
 * for full license information.
 *
 * json/JsonSimpleWriter.cpp
 */

#include "JsonSimpleWriter.h"

namespace json
{

JsonSimpleWriter::JsonSimpleWriter(fatfs::File& f)
    : os(f), wr(os)
{
    wr.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    wr.StartObject();
}

JsonSimpleWriter::~JsonSimpleWriter()
{
    UpdatePath("");
    wr.EndObject();
}

void JsonSimpleWriter::PathKey(const char* path, const char* comment)
{
    auto name = UpdatePath(path);
    if (comment && comment[0])
    {
        wr.CommentLine(comment);
    }
    wr.Key(name);
}

const char* JsonSimpleWriter::UpdatePath(const char* path)
{
    const char* oldPath = this->path;
    int s = 0;  // start of first differing path element

    for (int i = 0; path[i]; i++)
    {
        if (oldPath[i] != path[i])
        {
            break;
        }
        if (oldPath[i] == '.')
        {
            s = i + 1;
        }
    }

    // end objects from the old path
    for (int i = s; oldPath[i]; i++)
    {
        if (oldPath[i] == '.')
        {
            wr.EndObject();
        }
    }

    // add objects from the new path, looking for the name part
    for (int i = s; path[i]; i++)
    {
        if (path[i] == '.')
        {
            wr.Key(path + s, i - s);
            wr.StartObject();
            s = i + 1;
        }
    }

    this->path = path;
    return path + s;
}

}
