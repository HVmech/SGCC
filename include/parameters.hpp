#ifndef SGCC_PARAMETERS
#define SGCC_PARAMETERS

#include "headers.h"

namespace sgcc
{
    enum class type
    {
        Rect
        // TODO: implement other shape types
    };

    struct parameters
    {
        std::string name;
        type type;
        size_t id;
        nana::point placement;
        nana::size size;
        nana::colors color;
        bool visible;
    };
}
#endif