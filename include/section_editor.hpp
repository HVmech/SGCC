#ifndef SGCC_SECTION_EDITOR
#define SGCC_SECTION_EDITOR

#include "shape_constructor.hpp"
#include "elementbox.hpp"
#include "shapes.hpp"

#include "headers.h"

namespace sgcc
{
    class section_editor :
        public nana::panel<true>
    {
    public:
        section_editor(nana::window wd, globals& gl, elementbox& elembox, nana::label& stat);

        std::string dynamic(globals& gl);

        bool alignment(nana::point& point, int radius) const;

        bool check_borders(const nana::point& point, unsigned scale) const;

        void modal_form(const nana::arg_click& arg, globals& gl);

        void calculate(globals& gl);

        std::string environment(globals& gl);

        virtual ~section_editor();

        friend class axis_option_group;
    private:
        axis ax;
        bool m_need_vscr, m_need_hscr;
        bool m_alignment = false;
        point m_align_point;
        nana::paint::image m_img;
        nana::drawing m_dw;
        std::vector<shape*> m_shapes;
        std::vector<point*> m_points;
        nana::scroll<true> m_vscr;
        nana::scroll<false> m_hscr;
        nana::place m_plc;
        nana::point m_pnt, m_mouse;
        nana::size m_imgsz, scr_size;
        parameters m_params;
        const std::string m_idle;
        std::string m_dynamic;
    };

    // Delegating c-tor :
    // Inherited widget c-tor,
    // Class nana::drawing c-tor,
    // Vertical scroll nana::scroll<true> c-tor,
    // Horizontal scroll nana::scroll<false> c-tor,
    // Class sgcc::point alignment point c-tor,
    // Class std::string mouse status idle c-tor,
    // Class sgcc::axis default c-tor 
    inline section_editor::section_editor(nana::window wd, globals& gl, elementbox& elembox, nana::label& stat) :
        nana::panel<true>(wd),
        m_dw{*this},
        m_vscr {this->handle(), nana::rectangle(10,10,500,500), true},
        m_hscr {this->handle(), nana::rectangle(10,10,500,500), true},
        m_align_point{nana::point {}, 0, nana::colors::black, nana::colors::white, false},
        m_idle{"Current position : - ; -"},
        ax{}
    {
        m_img = nana::paint::image{environment(gl)};
        m_plc.bind(*this);
        m_imgsz = m_img.size();

        m_plc.div("vert <<> <weight=16 vert_scroll>> <weight=16 h_bottom <horz_scroll><weight=16 corner>>");
        m_plc["vert_scroll"] << m_vscr;
        m_plc["horz_scroll"] << m_hscr;

        m_dw.draw([&](nana::paint::graphics& graph)
        {
            scr_size = graph.size();

            if (scr_size.height < m_imgsz.height)
            {
            scr_size.width -= 16;
                if (scr_size.width < m_imgsz.width) scr_size.height -= 16;
            }
            else if (scr_size.width < m_imgsz.width)
            {
                scr_size.height -= 16;
                if (scr_size.height < m_imgsz.height) scr_size.width -= 16;
            }

            if (m_pnt.x + m_imgsz.width < scr_size.width)
            {
                m_pnt.x = static_cast<int>(scr_size.width) - static_cast<int>(m_imgsz.width);
                
                if (m_pnt.x > 0) m_pnt.x = 0;
            }

            if (m_pnt.y + m_imgsz.height < scr_size.height)
            {
                m_pnt.y = static_cast<int>(scr_size.height) - static_cast<int>(m_imgsz.height);

                if (m_pnt.y > 0) m_pnt.y = 0;
            }

            m_need_vscr = scr_size.height < m_imgsz.height;
            m_plc.field_display("vert_scroll", m_need_vscr);

            if (m_need_vscr)
            {
                m_vscr.range(scr_size.height);
                m_vscr.amount(m_imgsz.height);
                m_vscr.value(-m_pnt.y);
            }
            else m_pnt.y = 0;

            m_need_hscr = scr_size.width < m_imgsz.width;
            m_plc.field_display("h_bottom", m_need_hscr);

            if (m_need_hscr)
            {
                m_hscr.range(scr_size.width);
                m_hscr.amount(m_imgsz.width);
                m_hscr.value(-m_pnt.x);
            }
            else m_pnt.x = 0;

            m_plc.field_display("corner", m_need_vscr && m_need_hscr);

            m_img.paste(graph, m_pnt);

            ax.regenerate(m_points, m_imgsz);
            ax.draw(m_pnt, graph, gl.axisweight, gl.axisgap);

            for (const auto& i : m_shapes)
            {
                i->draw(m_pnt, graph, gl.lineweight);
            }

            for (const auto& i : m_points)
            {
                i->draw(m_pnt, graph, gl.pointweight);
            }

            if(gl.align)
            {
                m_align_point.visible = m_alignment;
                m_align_point.draw(m_pnt, graph, gl.pointweight +2);
            }

            m_plc.collocate();

            //Draw the corner
            if (m_need_vscr && m_need_hscr)
                graph.rectangle(nana::rectangle( graph.width() - 20, graph.height() - 20, 20, 20 ), true, nana::API::bgcolor(*this));
        });

        m_vscr.events().value_changed([&]
        {
            m_pnt.y = -static_cast<int>(m_vscr.value());
            nana::API::refresh_window(*this);
        });

        m_hscr.events().value_changed([&]
        {
            m_pnt.x = -static_cast<int>(m_hscr.value());
            nana::API::refresh_window(*this);
        });

        this->events().click([&, this](const nana::arg_click& mouse_arg)
        {
            nana::point m_mouse = mouse_arg.mouse_args->pos - m_pnt;

            if(this->check_borders(m_mouse, gl.pxpermm))
            {
                if(gl.align)
                {
                    m_alignment = this->alignment(m_mouse, gl.alignradius);

                    if(m_alignment)
                    {
                        m_align_point.coordinates() = m_mouse;
                    }
                }
                this->modal_form(mouse_arg, gl);

                if(m_params.size.width*m_params.size.height)
                {
                    m_params.placement += m_mouse;

                    m_params.id = ++gl.id;
                    m_params.color = nana::colors::black;
                    m_params.visible = true;

                    m_shapes.push_back(new rect{m_params});
                    m_shapes[m_shapes.size()-1]->add_points(m_points, nana::colors::white);

                    elembox.at(0).append(m_params);
                    elembox.at(0).at(elembox.size_item(0)-1).check(true);
                }

                m_dw.update();
            }
        });

        elembox.events().checked([&](const nana::arg_listbox& arg)
        {
            m_shapes[arg.item.pos().item]->properties().visible = arg.item.checked() ? true : false;

            for(const auto& i : m_points)
            {
                if(m_shapes[arg.item.pos().item]->properties().id == i->id)
                {
                    i->visible = m_shapes[arg.item.pos().item]->properties().visible;
                }
            }

            for (const auto& i : elembox.selected())
            {
                elembox.at(0).at(i.item).check(arg.item.checked() ? true : false);
            }

            m_dw.update();
        });

        elembox.events().key_press([&](const nana::arg_keyboard& arg)
        {
            if(arg.key == 27)
            {
                for (const auto& i : elembox.selected())
                {
                    elembox.at(0).at(i.item).select(false);
                }

                ax.regenerate(m_points, m_imgsz);

                m_dw.update();
            }
            if(arg.key == 127)
            {
                auto elems = elembox.selected();
                elembox.erase(elems);

                for(size_t i = 0; i < elems.size(); ++i)
                {
                    for (size_t j = 0; j < m_points.size(); ++j)
                    {
                        if (m_points[j]->id == m_shapes[elems[i].item - i]->properties().id)
                        {
                            delete m_points[j];
                            m_points.erase(m_points.begin() + j);
                            --j;
                        }
                    }

                    delete m_shapes[elems[i].item - i];
                    m_shapes.erase(m_shapes.begin() + elems[i].item - i);
                }

                ax.regenerate(m_points, m_imgsz);

                m_dw.update();
            }
        });

        this->events().mouse_enter([&](const nana::arg_mouse & mouse_arg)
        {
            m_mouse = mouse_arg.pos - m_pnt;

            m_dynamic = dynamic(gl);

            if(this->check_borders(m_mouse, gl.pxpermm))
            {
                stat.caption(m_dynamic);
            }
            else
            {
                stat.caption(m_idle);
            }
        });

        this->events().mouse_leave([&]
        {
            stat.caption(m_idle);
        });

        this->events().mouse_move([&](const nana::arg_mouse& mouse_arg)
        {
            m_mouse = mouse_arg.pos - m_pnt;

            if(this->check_borders(m_mouse, gl.pxpermm))
            {
                m_dynamic = dynamic(gl);
                stat.caption(m_dynamic);

                if(gl.align)
                {
                    m_alignment = this->alignment(m_mouse, gl.alignradius);

                    if(m_alignment)
                    {
                        m_align_point.coordinates() = m_mouse;
                    }
                }
                
                m_dw.update();
            }
            else
            {
                stat.caption(m_idle);
            }
        });
    }


    inline std::string section_editor::dynamic(globals& gl)
    {
        return "Current position : " +
            std::to_string(m_mouse.x/gl.pxpermm) +
            "," +
            std::to_string(((m_mouse.x)%(gl.pxpermm))*10/gl.pxpermm) +
            " ; " +
            std::to_string((static_cast<int>(m_imgsz.height) - m_mouse.y)/gl.pxpermm) +
            "," +
            std::to_string(((static_cast<int>(m_imgsz.height) - m_mouse.y)%(gl.pxpermm)*10/gl.pxpermm));
    }

    inline bool section_editor::alignment(nana::point& point, int radius) const
    {
        for(const auto& i : m_points){
            if(i->visible){
                if(i->align(point, radius)){
                    return true;
                }
            }
        }
        return false;
    }

    inline bool section_editor::check_borders(const nana::point& point, unsigned scale) const
    {
        unsigned t_x = std::make_unsigned_t<int>(point.x);
        unsigned t_y = std::make_unsigned_t<int>(point.y);

        return
        (
            t_x <= m_imgsz.width - scale &&
            t_y <= m_imgsz.height - scale &&
            t_x >= scale &&
            t_y >= scale
        );
    }

    inline void section_editor::modal_form(const nana::arg_click& arg, globals& gl)
    {
        shape_constructor fm
        (
            arg.window_handle,
            nana::API::make_center(arg.window_handle, 550, 360),
            nana::appear::decorate
            <
                nana::appear::minimize,
                nana::appear::taskbar,
                nana::appear::floating
            >(),
            m_params,
            gl
        ); 

        nana::API::modal_window(fm); 
    }

    inline void section_editor::calculate(globals& gl)
    {
        xlnt::workbook wb;
        xlnt::worksheet ws = wb.active_sheet();
        ws.title(gl.xlsxsheetname);

        xlnt::alignment center;
        center.horizontal(xlnt::horizontal_alignment::center).vertical(xlnt::vertical_alignment::center);

        xlnt::alignment left;
        left.horizontal(xlnt::horizontal_alignment::left).vertical(xlnt::vertical_alignment::center);

        xlnt::alignment right;
        right.horizontal(xlnt::horizontal_alignment::right).vertical(xlnt::vertical_alignment::center);

        xlnt::border::border_property thin_border;
        thin_border.style(xlnt::border_style::thin).color(xlnt::color::black());

        xlnt::border all_borders;
        all_borders.side(xlnt::border_side::start, thin_border).side(xlnt::border_side::end, thin_border);
        all_borders.side(xlnt::border_side::top, thin_border).side(xlnt::border_side::bottom, thin_border);

        xlnt::border no_bottom;
        no_bottom.side(xlnt::border_side::start, thin_border).side(xlnt::border_side::end, thin_border);
        no_bottom.side(xlnt::border_side::top, thin_border);

        xlnt::border corner;
        corner.side(xlnt::border_side::start, thin_border).side(xlnt::border_side::top, thin_border);

        xlnt::number_format longer {"#,###0.000"};

        ws.cell("B2").value("№");
        ws.cell("C2").value("a, см");
        ws.cell("D2").value("b, см");
        ws.cell("E2").value("F, см2");
        ws.cell("F2").value("Y, см");
        ws.cell("G2").value("F*Y, см3");
        ws.cell("H2").value("F*Y2, см4");
        ws.cell("I2").value("Ic, см4");

        for (int i = 0; i < m_shapes.size(); ++i)
        {
            if(m_shapes[i]->properties().visible) m_shapes[i]->to_xlsx(ws, ax, i);
        }

        const int offset = static_cast<int>(m_shapes.size());

        if(offset)
        {
            ws.cell("B3").offset(0, offset).value("Сумма:");
            ws.cell("B3").offset(0, offset).border(corner);
            ws.cell("B3").offset(0, offset).alignment(left);

            ws.cell("E3").offset(0, offset).formula("=SUM(E3:E" + std::to_string(3 + offset -1) + ")");
            ws.cell("E3").offset(0, offset).number_format(longer);
            ws.cell("E3").offset(0, offset).border(no_bottom);
            ws.cell("E3").offset(0, offset).alignment(center);

            ws.cell("G3").offset(0, offset).formula("=SUM(G3:G" + std::to_string(3 + offset -1) + ")");
            ws.cell("G3").offset(0, offset).number_format(longer);
            ws.cell("G3").offset(0, offset).border(no_bottom);
            ws.cell("G3").offset(0, offset).alignment(center);

            ws.cell("H3").offset(0, offset).formula("=SUM(H3:H" + std::to_string(3 + offset -1) + ")");
            ws.cell("H3").offset(0, offset).number_format(longer);
            ws.cell("H3").offset(0, offset).border(no_bottom);
            ws.cell("H3").offset(0, offset).alignment(center);

            ws.cell("I3").offset(0, offset).formula("=SUM(I3:I" + std::to_string(3 + offset -1) + ")");
            ws.cell("I3").offset(0, offset).number_format(longer);
            ws.cell("I3").offset(0, offset).border(no_bottom);
            ws.cell("I3").offset(0, offset).alignment(center);

            for(int i = 0; i < offset + 1; ++i)
            {
                ws.range("B" + std::to_string(2 + i) + ":I" + std::to_string(2 + i)).alignment(center);
                ws.range("B" + std::to_string(2 + i) + ":I" + std::to_string(2 + i)).border(all_borders);
                if(i) ws.range("C" + std::to_string(2 + i) + ":I" + std::to_string(2 + i)).number_format(longer);
            }

            // Section center of gravity
            ws.cell("B3").offset(0, offset +2).value("Координаты центра тяжести:");
            ws.cell("B3").offset(0, offset +2).alignment(left);

            // Y0
            ws.cell("F3").offset(0, offset +2).value("Y0 =");
            ws.cell("F3").offset(0, offset +2).alignment(right);

            ws.cell("G3").offset(0, offset +2).formula("=G" + std::to_string(3 + offset) + "/E" + std::to_string(3 + offset));
            ws.cell("G3").offset(0, offset +2).number_format(longer);
            ws.cell("G3").offset(0, offset +2).alignment(center);

            ws.cell("H3").offset(0, offset +2).value("см;");
            ws.cell("H3").offset(0, offset +2).alignment(left);    

            // Y1
            ws.cell("F3").offset(0, offset +4).value("Y1 =");
            ws.cell("F3").offset(0, offset +4).alignment(right);

            ws.cell("G3").offset(0, offset +4).formula("=G" + std::to_string(3 + offset +6) + "-G" + std::to_string(3 + offset +2));
            ws.cell("G3").offset(0, offset +4).number_format(longer);
            ws.cell("G3").offset(0, offset +4).alignment(center);

            ws.cell("H3").offset(0, offset +4).value("см;");
            ws.cell("H3").offset(0, offset +4).alignment(left);
                
            // Section height
            ws.cell("B3").offset(0, offset +6).value("Высота сечения:");
            ws.cell("B3").offset(0, offset +6).alignment(left);

            ws.cell("F3").offset(0, offset +6).value("H =");
            ws.cell("F3").offset(0, offset +6).alignment(right);

            double h = static_cast<double>(ax.bottom() - ax.top())*10;
            ws.cell("G3").offset(0, offset +6).value(h/1000);
            ws.cell("G3").offset(0, offset +6).number_format(longer);
            ws.cell("G3").offset(0, offset +6).alignment(center);

            ws.cell("H3").offset(0, offset +6).value("см;");
            ws.cell("H3").offset(0, offset +6).alignment(left); 

            // Moment of inertia
            ws.cell("B3").offset(0, offset +8).value("Момент инерции сечения:");
            ws.cell("B3").offset(0, offset +8).alignment(left);

            ws.cell("F3").offset(0, offset +8).value("Ix =");
            ws.cell("F3").offset(0, offset +8).alignment(right);

            ws.cell("G3").offset(0, offset +8).formula
            (
                "=I" + std::to_string(3 + offset) +
                "+H" + std::to_string(3 + offset) +
                "-E" + std::to_string(3 + offset) +
                "*POWER(G" + std::to_string(3 + offset +2) + ",2)"
            );

            ws.cell("G3").offset(0, offset +8).number_format(longer);
            ws.cell("G3").offset(0, offset +8).alignment(center);

            ws.cell("H3").offset(0, offset +8).value("см4;");
            ws.cell("H3").offset(0, offset +8).alignment(left); 

            // Radius of inertia
            ws.cell("B3").offset(0, offset +10).value("Радиус инерции сечения:");
            ws.cell("B3").offset(0, offset +10).alignment(left);

            ws.cell("F3").offset(0, offset +10).value("ix =");
            ws.cell("F3").offset(0, offset +10).alignment(right);

            ws.cell("G3").offset(0, offset +10).formula
            (
                "=SQRT(G" + std::to_string(3 + offset +8) +
                "/E" + std::to_string(3 + offset) + ")"
            );

            ws.cell("G3").offset(0, offset +10).number_format(longer);
            ws.cell("G3").offset(0, offset +10).alignment(center);

            ws.cell("H3").offset(0, offset +10).value("см;");
            ws.cell("H3").offset(0, offset +10).alignment(left); 
        }

        wb.save(gl.xlsxname);

        nana::msgbox inf(this->handle(), "SGCC output");
        inf.icon(nana::msgbox::icon_t::icon_information);
        inf << "Success!";
        inf.show();
    }

    inline std::string section_editor::environment(globals& gl)
    {
        int i = 0, j = 0;
        if(gl.resolution.dimension().width >= 3200)
        {
            i = 6;
        } else if(gl.resolution.dimension().width >= 2400 && gl.resolution.dimension().width < 3200)
        {
            i = 5;
        } else if(gl.resolution.dimension().width >= 1800 && gl.resolution.dimension().width < 2400)
        {
            i = 4;
        } else if(gl.resolution.dimension().width >= 1200 && gl.resolution.dimension().width < 1800)
        {
            i = 3;
        } else if(gl.resolution.dimension().width >= 1000 && gl.resolution.dimension().width < 1200)
        {
            i = 2;
        } else if(gl.resolution.dimension().width < 1000)
        {
            i = 1;
            }
        if(gl.resolution.dimension().height >= 2400)
        {
            j = 6;
        } else if(gl.resolution.dimension().height >= 1800 && gl.resolution.dimension().height < 2400)
        {
            j = 5;
        } else if(gl.resolution.dimension().height >= 1200 && gl.resolution.dimension().height < 1800)
        {
            j = 4;
        } else if(gl.resolution.dimension().height >= 1000 && gl.resolution.dimension().height < 1200)
        {
            j = 3;
        } else if(gl.resolution.dimension().height >= 850 && gl.resolution.dimension().height < 1000)
        {
            j = 2;
        } else if(gl.resolution.dimension().height < 850)
        {
            j = 1;
        }

        i = i > j ? i : j;

        switch(i)
        {
            case 1:
                return {"field-800x600.bmp"};

            case 2:
                return {"field-1000x850.bmp"};

            case 3:
                return {"field-1200x1000.bmp"};

            case 4:
                return {"field-1800x1200.bmp"};

            case 5:
                return {"field-2400x1800.bmp"};

            case 6:
                return {"field-3200x2400.bmp"};

            default:
                return {"field-800x600.bmp"};
        }
    }

    inline section_editor::~section_editor()
    {
        for(size_t i = 0; i < m_shapes.size(); ++i)
        {
            delete m_shapes[i];
        }

        for(size_t i = 0; i < m_points.size(); ++i)
        {
            delete m_points[i];
        }
    }
}
#endif