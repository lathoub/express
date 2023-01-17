#pragma once

#include "defs.h"

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class mustache
{
public:
    // Entry point from Express, given by the mustacheExpress()
    static void renderFile()
    {
        EX_DBG_I(F("in renderFile"));
    }

public:
    static String render(const String &format, locals_t &namedValues)
    {
        String result = F("");

        int pos = 0;
        int start = format.indexOf(F("{{"), pos);

        // the string does not contain {{}}
        if (start < 0)
            return format;

        while (start > 0)
        {
            int end = format.indexOf(F("}}"), start);
            auto variable = format.substring(start + 2, end);

            result += format.substring(pos, start);
            result += namedValues[variable];
            pos = end + 2;

            start = format.indexOf(F("{{"), pos);
        }

        return result;
    }

    static String render(const char* format, size_t from, size_t to, locals_t &namedValues)
    {
        String result = F("");

        return result;
    }
};

static RenderEngineCallback mustacheExpress()
{
    return mustache::renderFile;
}

END_EXPRESS_NAMESPACE
