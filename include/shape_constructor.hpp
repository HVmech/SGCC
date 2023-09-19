#ifndef SGCC_SHAPE_CONSTRUCTOR
#define SGCC_SHAPE_CONSTRUCTOR

// sgcc headers
#include "globals.hpp"
#include "parameters.hpp"

#include "headers.h"

namespace sgcc
{
    // SGCC Section editor widget
    // Inheritance from standart nana panel with HasBackground parameter <true>
    class shape_constructor :
        public nana::form
    {
    public:
        shape_constructor
        (
            nana::window owner,
            const nana::rectangle& rectan,
            const nana::appearance& appear,
            parameters& params,
            globals& gl
        );
    private:
        bool get_width();

        bool get_height();
        
        bool get_name();
        
        parameters m_params;
        nana::textbox m_n, m_a, m_b;
        nana::button m_ok, m_cancel;
        nana::label m_tlabel, m_nlabel, m_alabel, m_blabel;
        nana::picture m_picture;
        nana::paint::image m_figure;
        nana::place m_plc;
    };
}
#endif