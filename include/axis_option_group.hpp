#ifndef SGCC_AXIS_OPTION_GROUP
#define SGCC_AXIS_OPTION_GROUP

// sgcc headers
#include "section_editor.hpp"

#include "headers.h"

namespace sgcc
{
    class axis_option_group :
        public nana::group
    {
    public:
        axis_option_group(nana::window wb, section_editor& sec);
    };

    inline axis_option_group::axis_option_group(nana::window wb, section_editor& sec) :
        nana::group(wb)
    {
        this->caption("Axis placement:");

        this->add_option("Top").events().checked([&](const nana::arg_checkbox& arg)
        {
            if(arg.widget->checked())
            {
                sec.ax.placement = true;
                nana::API::refresh_window(sec.handle());
            } 
        });

        this->add_option("Bottom").events().checked([&](const nana::arg_checkbox& arg)
        {
            if(arg.widget->checked())
            {
                sec.ax.placement = false;
                nana::API::refresh_window(sec.handle());
            } 
        });

        this->add_option("Hidden").events().checked([&](const nana::arg_checkbox& arg)
        {
            if(arg.widget->checked())
            {
                sec.ax.placement = true;
                sec.ax.visible = false;
                nana::API::refresh_window(sec.handle());
            } 
            else
            {
                sec.ax.visible = true;
                nana::API::refresh_window(sec.handle());
            }
        });

        this->radio_mode(true);
        this->option_check(0);
    }
}
#endif