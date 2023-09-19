#ifndef SGCC_GLOBALS
#define SGCC_GLOBALS

#include "headers.h"

namespace sgcc
{
    struct globals
    {
        int lineweight;
        int pointweight;
        unsigned pxpermm;
        bool align;
        int alignradius;
        int axisgap;
        int axisweight;
        size_t id;
        std::string xlsxsheetname;
        std::string xlsxname;
        nana::screen screen;
        nana::rectangle resolution;
        const char* icon_filename = {"sgcc.ico"};
    };
}
#endif