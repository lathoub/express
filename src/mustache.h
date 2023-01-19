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
};

static RenderEngineCallback mustacheExpress()
{
    return mustache::renderFile;
}

END_EXPRESS_NAMESPACE
