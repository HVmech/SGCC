#include "shape_constructor.hpp"

namespace sgcc
{
    shape_constructor::shape_constructor
    (
        nana::window owner,
        const nana::rectangle& rectan,
        const nana::appearance& appear,
        parameters& params,
        globals& gl
    ) :
        nana::form(owner, rectan, appear),
        m_params{},
        m_n{*this},
        m_a{*this},
        m_b{*this},
        m_ok{*this, "Ok"},
        m_cancel{*this, "Cancel"},
        m_tlabel{*this, "Element type: Rect"},
        m_nlabel{*this, "Element name:"},
        m_alabel{*this, "Horizontal dimension (a) [мм/10]:"},
        m_blabel{*this, "Vertical dimension (b) [мм/10]:"},
        m_picture{*this},
        m_figure{"environment.bmp"}
    {
        this->caption("Parameters");

        //Set icon for the app
        nana::API::window_icon(this->handle(), nana::paint::image(gl.icon_filename));

        m_picture.load(m_figure);
        m_picture.align(nana::align::center, nana::align_v::center);

        m_n.multi_lines(false);
        m_a.multi_lines(false);
        m_b.multi_lines(false);

        m_plc.bind(*this);
        m_plc.div( R"(horizontal gap=10 margin=10 
            <vertical weight=200
                <element_type weight=30>
                <weight=20>
                <vertical input_name>
                <weight=10>
                <vertical input_width>
                <weight=10>
                <vertical input_height>
                <weight=30>
                <control_buttons weight=40 gap=10>
            >
            <weight=10>
            <figure>
        )" );

        m_plc["element_type"] << m_tlabel;
        m_plc["input_name"] << m_nlabel << m_n;
        m_plc["input_width"] << m_alabel << m_a;
        m_plc["input_height"] << m_blabel << m_b;
        m_plc["figure"] << m_picture;
        m_plc["control_buttons"] << m_ok << m_cancel;

        m_plc.collocate();

        m_cancel.events().click([&, this]
        {
            this->close();
        });

        m_ok.events().click([&, this]
        {
            if ((this->get_width()) && (this->get_height()) && (this->get_name()))
            {
                this->m_params.type = type::Rect;
                this->close();
            }
            else
            {
                this->m_params.size.width = 0;
                this->m_params.size.height = 0;
                nana::msgbox err(this->handle(), "Error");
                err.icon(nana::msgbox::icon_t::icon_information);
                err << "Invalid input";
                err.show();
            }
        });

        this->events().unload([&, this]
        {
            params = this->m_params;
        });
    }

    bool shape_constructor::get_width(){
        std::string str;
        if (m_a.getline(0, str))
        {
            if(str.empty()) return false;

            for (size_t i = 0; i < str.size(); ++i)
            {
                if (!std::isdigit(str[i]))
                {
                    if(i == 0 && str[i] == '-') continue;
                    
                    return false;
                }
            }

            int w {std::stoi(str)};

            if (w < 0)
            {
                m_params.placement.x = w;
                m_params.size.width = std::make_unsigned_t<int>(-w);
            }
            else
            {
                m_params.placement.x = 0;
                m_params.size.width = std::make_unsigned_t<int>(w);
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    bool shape_constructor::get_height()
    {
        std::string str;

        if (m_b.getline(0, str))
        {
            if(str.empty()) return false;
            
            for (size_t i = 0; i < str.size(); ++i)
            {
                if (!std::isdigit(str[i]))
                {
                    if(i == 0 && str[i] == '-') continue;
                    
                    return false;
                }
            }

            int h {std::stoi(str)};
            
            if (h < 0)
            {
                m_params.placement.y = 0;
                m_params.size.height = std::make_unsigned_t<int>(-h);
            }
            else
            {
                m_params.placement.y = -h;
                m_params.size.height = std::make_unsigned_t<int>(h);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
        
    bool shape_constructor::get_name(){
        m_n.getline(0, m_params.name);

        if(m_params.name.empty())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}