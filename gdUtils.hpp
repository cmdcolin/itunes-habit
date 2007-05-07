#pragma once

#include <gd.h>
#include <gdfontt.h>
#include <stdexcept>

struct
    gdWimage
{
    gdImagePtr p;

    gdWimage(int x, int y) :
        p(gdImageCreate(x, y))
    {
        if(!p)
            throw std::runtime_error(std::string("Internal gdImageCreate failure"));
    }

    ~gdWimage(void)
    {
        gdImageDestroy(p);
    }
};
