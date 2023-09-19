#ifndef SGCC_INFO
#define SGCC_INFO

#include "globals.hpp"

#include "headers.h"

namespace sgcc
{
    class info :
        public nana::form
    {
    public:
        info(nana::window owner, globals& gl);
        virtual ~info() {}
    private:
        std::stringstream m_str;
        nana::label m_version, m_info;
        nana::place m_plc;
    };
}
#endif