#ifndef SGCC_GENERAL_FORM
#define SGCC_GENERAL_FORM

// sgcc headers
#include "axis_option_group.hpp"
#include "section_editor.hpp"
#include "elementbox.hpp"
#include "info.hpp"

#include "headers.h"

namespace sgcc
{
    class general_form :
        public nana::form
    {
    public:
        general_form(globals& gl);
    private:
        nana::place m_plc;
        nana::label m_cursor_status;
        nana::button m_calculate;
        nana::menubar m_menu;
        elementbox m_elements_list;
        section_editor m_section_editor;
        axis_option_group m_axis_options;
    };

    inline general_form::general_form(globals& gl) : 
        nana::form{},
        m_cursor_status{*this, "Current position : - ; -"},
        m_calculate{*this, "Calculate"},
        m_menu{*this},
        m_elements_list{*this},
        m_section_editor{*this, gl, m_elements_list, m_cursor_status},
        m_axis_options{*this, m_section_editor}
    {
        m_plc.bind(*this);

        //Toggle size of created form based on screen resolution
        this->move(gl.resolution.x, gl.resolution.y);
        this->outline_size(gl.resolution.dimension());
        this->caption("SGC Calc");

        //Set icon for the app
        nana::API::window_icon(*this, nana::paint::image(gl.icon_filename));

        m_cursor_status.text_align(nana::align::left, nana::align_v::center);

        m_menu.push_back("File").append("Calculate", [&](nana::menu::item_proxy&)
        {
            m_section_editor.calculate(gl);
        });

        m_menu.at(0).append("Quit", [&](nana::menu::item_proxy&)
        {
            this->close();
        });

        m_menu.push_back("Help").append("Info", [&](nana::menu::item_proxy&)
        {
            info inform {*this, gl};
            nana::API::modal_window(inform); 
        });

        m_plc.div( R"(vertical gap=10 margin=5
            <menu_panel weight = 50> 
            <horizontal
                <section_editor_panel> | 20%
                <vertical
                    <elements_list_panel margin=5>
                    <axis_options_panel margin=5 weight=100>
                    <control_button margin=5 weight=50>
                >
            > 
            <status_panel weight = 50>
        )" );

        m_plc["menu_panel"] << m_menu ;
        m_plc["section_editor_panel"] << m_section_editor;
        m_plc["elements_list_panel"] << m_elements_list;
        m_plc["axis_options_panel"] << m_axis_options;
        m_plc["control_button"] << m_calculate;
        m_plc["status_panel"] << m_cursor_status;

        m_calculate.events().click([&]
        {
            m_section_editor.calculate(gl);
        });

        this->events().unload([&,this](const nana::arg_unload& arg)
        {
            nana::msgbox qst(this->handle(), {}, nana::msgbox::yes_no);
            qst.icon(nana::msgbox::icon_t::icon_question);
            qst << "Are you sure you want to quit?";
            arg.cancel = (qst() == qst.pick_no);
        });

        m_plc.collocate();

        this->show();
    }
}
#endif