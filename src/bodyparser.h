#pragma once

#include "namespace.h"

BEGIN_EXPRESS_NAMESPACE

class bodyParser
{
public:
    bool inflate;
    String type;
    String limit;

public:
    static bool json(Request &req, Response &res)
    {
        EX_DBG_I(F("in json bodyParser"));

        if (req.get(F("content-type")).equalsIgnoreCase(F("application/json"))) {
            req.body = F("{ 'name': 'bart' }");
            return true;
        }

        return true;
    }
};

END_EXPRESS_NAMESPACE
