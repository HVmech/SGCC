#pragma once

// nana headers
#include <nana/gui.hpp>
#include <nana/gui/widgets/scroll.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/menubar.hpp>

// xlnt headers
#include <xlnt/workbook/workbook.hpp>
#include <xlnt/worksheet/worksheet.hpp>
#include <xlnt/worksheet/range.hpp>
#include <xlnt/cell/cell.hpp>
#include <xlnt/styles/alignment.hpp>
#include <xlnt/styles/border.hpp>
#include <xlnt/styles/number_format.hpp>

// std headers
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <type_traits>
#include <algorithm>
#include <sstream>

namespace sgcc
{
    enum class type
    {
        Rect
        // TODO: implement other shape types
    };

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

    class point
    {
    public:
        point() : m_point{} {}

        point(nana::point pnt, size_t i, nana::colors clr1, nana::colors clr2, bool vis) :
            m_point{pnt.x, pnt.y}, id{i}, color1{clr1}, color2{clr2}, visible{vis} {}

        nana::point& coordinates() {
            return m_point;
        }

        bool align(nana::point& point, int radius) const {
            if(
                m_point.x < point.x + radius && m_point.x > point.x - radius &&
                m_point.y < point.y + radius && m_point.y > point.y - radius
            ){
                point = m_point;
                return true;
            }
            else{
                return false;
            }
        }
        void draw(nana::point& base, nana::paint::graphics& graph, int weight) const {
            if (visible) {
                for(int i = -weight; i <= weight; ++i) {
                    for(int j = -weight; j <= weight; ++j) {
                        if(i*i + j*j <= weight*weight){
                            if(i*i + j*j < (weight -1)*(weight -1)){
                                graph.set_pixel(base.x + m_point.x + i, base.y + m_point.y + j, color2);
                            }
                            else{
                                graph.set_pixel(base.x + m_point.x + i, base.y + m_point.y + j, color1);
                            }
                        }
                    }
                }
            }
        }
        ~point(){
            std::cout << "Point (" << m_point.x << " , " << m_point.y << ") destruct" << std::endl;
        }
        size_t id;
        nana::colors color1, color2;
        bool visible;
    private:
        nana::point m_point;
    };

    class axis
    {
    public:
        axis() = default;

        ~axis() {};

        void regenerate(std::vector<point*>& arr, nana::size& size) {
            if(visible){
                if(arr.size()){
                    m_br.x = 0;
                    m_br.y = 0;
                    m_tl.x = static_cast<int>(size.width);
                    m_tl.y = static_cast<int>(size.height);
                    for(const auto& i : arr){
                        if(i->visible){
                            if(m_br.x < i->coordinates().x) m_br.x = i->coordinates().x;
                            if(m_br.y < i->coordinates().y) m_br.y = i->coordinates().y;
                            if(m_tl.x > i->coordinates().x) m_tl.x = i->coordinates().x;
                            if(m_tl.y > i->coordinates().y) m_tl.y = i->coordinates().y;
                        }
                    }
                    if(m_br.x && m_br.y){
                        m_visible = true;
                    }
                    else{
                        m_visible = false;
                    }
                }
                else{
                    m_visible = false;
                }
            }
        }

        void draw(nana::point& base, nana::paint::graphics& graph, int weight, int gap) {
            if(visible){
                if (placement){
                    draw_axis(m_tl + base, nana::point(m_br.x + base.x, m_tl.y + base.y), graph, weight, gap);
                }
                else{
                    draw_axis(nana::point(m_tl.x + base.x, m_br.y + base.y), m_br + base, graph, weight, gap);
                }
            }
        }

        int top() const {
            return m_tl.y;
        }

        int bottom() const {
            return m_br.y;
        }

        bool placement = true, visible = true;
        nana::colors color = nana::colors::black;

    private:
        void draw_axis(const nana::point& pnt1, const nana::point& pnt2, nana::paint::graphics& graph, int weight, int gap) {
            if(m_visible){
                nana::point temp {pnt1.x -2*gap, pnt1.y};
                while(temp.x < pnt2.x + 2*gap) {
                    for(int i = -weight; i <= weight; ++i) {
                        graph.line_begin(temp.x, temp.y + i);
                        graph.line_to(nana::point(temp.x + gap , temp.y + i), nana::colors::black);
                    }
                    temp.x += 2*gap;
                }
            }
        }
        bool m_visible = false;
        nana::point m_tl, m_br;
    };

    class shape
    {
    public:
        virtual ~shape() {}
        virtual void properties(parameters& params) = 0;
        virtual parameters& properties() = 0;
        virtual void draw(nana::point& base, nana::paint::graphics& graph, int weight) const = 0;
        virtual void add_points(std::vector<point*>& arr, nana::colors clr) const = 0;
        virtual void to_xlsx(xlnt::worksheet& ws, axis& ax, int ind) const = 0;
    };

    class rect : public shape
    {
    public:
        rect(const parameters& params) : m_params{params} {}
        void properties(parameters& params) override {
            m_params = params;
        }
        parameters& properties() override {
            return m_params;
        }
        void draw(nana::point& base, nana::paint::graphics& graph, int weight) const override {
            if (m_params.visible) {
                for(int i = -weight; i <= weight; ++i) {
                    for(int j = -weight; j <= weight; ++j) {
                        if(i*i + j*j <= weight*weight){
                            graph.rectangle(
                                nana::rectangle{
                                    (m_params.placement + base).x +i,
                                    (m_params.placement + base).y +j,
                                    m_params.size.width + 1 -2*i,
                                    m_params.size.height + 1 -2*j
                                },
                                false,
                                m_params.color
                            );
                        }
                    }
                }
            }
        }
        void add_points(std::vector<point*>& arr, nana::colors clr) const override {
            arr.push_back(new point{m_params.placement, m_params.id, m_params.color, clr, m_params.visible});
            arr.push_back(new point{
                nana::point{
                    m_params.placement.x + static_cast<int>(m_params.size.width),
                    m_params.placement.y
                }, m_params.id, m_params.color, clr, m_params.visible
            });
            arr.push_back(new point{
                nana::point{
                    m_params.placement.x + static_cast<int>(m_params.size.width),
                    m_params.placement.y + static_cast<int>(m_params.size.height)
                }, m_params.id, m_params.color, clr, m_params.visible
            });
            arr.push_back(new point{
                nana::point{
                    m_params.placement.x,
                    m_params.placement.y + static_cast<int>(m_params.size.height)
                }, m_params.id, m_params.color, clr, m_params.visible
            });            
        }
        void to_xlsx(xlnt::worksheet& ws, axis& ax, int ind) const override {
            //transfer width and height of element to float [in millimeters]
            int a = m_params.size.width*10;
            int b = m_params.size.height*10;
            int y;
            if (ax.placement) {
                y = (m_params.placement.y - ax.top())*10 + b/2;
            }
            else {
                y = (ax.bottom() - m_params.placement.y)*10 - b/2;
            }

            ws.cell("B3").offset(0, ind).value(ind+1);
            ws.cell("C3").offset(0, ind).value(std::to_string(a/1000) + "," + std::to_string(a%1000));
            ws.cell("D3").offset(0, ind).value(std::to_string(b/1000) + "," + std::to_string(b%1000));
            ws.cell("F3").offset(0, ind).value(std::to_string(y/1000) + "," + std::to_string(y%1000));
            std::string r = std::to_string(3+ind);
            ws.cell("E3").offset(0, ind).formula(std::string{"=C" + r + "*D" + r});
            ws.cell("G3").offset(0, ind).formula(std::string{"=E" + r + "*F" + r});
            ws.cell("H3").offset(0, ind).formula(std::string{"=G" + r + "*F" + r});
            ws.cell("I3").offset(0, ind).formula(std::string{"=C" + r + "*POWER(D" + r + ",3)/12"});
        }

        virtual ~rect() {
            std::cout << "Rect id = " << properties().id << " destruct" << std::endl;
        }
    private:
        parameters m_params;
    };

    // SGCC Section editor widget
    // Inheritance from standart nana panel with HasBackground parameter <true>
    class shape_constructor :
        public nana::form
    {
    public:
        shape_constructor(
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
                if ((this->get_width()) && (this->get_height()) && (this->get_name())) {
                    this->m_params.type = type::Rect;
                    this->close();
                } else {
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
    private:
        bool get_width(){
            std::string str;
            if (m_a.getline(0, str)) {
                if(str.empty()) return false;
                for (size_t i = 0; i < str.size(); ++i) {
                    if (!std::isdigit(str[i])) {
                        if(i == 0 && str[i] == '-') continue;
                        return false;
                    }
                }
                int w {std::stoi(str)};
                if (w < 0) {
                    m_params.placement.x = w;
                    m_params.size.width = std::make_unsigned_t<int>(-w);
                }
                else {
                    m_params.placement.x = 0;
                    m_params.size.width = std::make_unsigned_t<int>(w);
                }
                return true;
            } else {
                return false;
            }
        }

        bool get_height(){
            std::string str;
            if (m_b.getline(0, str)) {
                if(str.empty()) return false;
                for (size_t i = 0; i < str.size(); ++i) {
                    if (!std::isdigit(str[i])) {
                        if(i == 0 && str[i] == '-') continue;
                        return false;
                    }
                }
                int h {std::stoi(str)};
                if (h < 0) {
                    m_params.placement.y = 0;
                    m_params.size.height = std::make_unsigned_t<int>(-h);
                }
                else {
                    m_params.placement.y = -h;
                    m_params.size.height = std::make_unsigned_t<int>(h);
                }
                return true;
            } else {
                return false;
            }
        }
        
        bool get_name(){
            m_n.getline(0, m_params.name);
            if(m_params.name.empty()) {
                return false;
            }
            else {
                return true;
            }
        }
        
        parameters m_params;
        nana::textbox m_n, m_a, m_b;
        nana::button m_ok, m_cancel;
        nana::label m_tlabel, m_nlabel, m_alabel, m_blabel;
        nana::picture m_picture;
        nana::paint::image m_figure;
        nana::place m_plc;
    };

    class elementbox :
        public nana::listbox
    {
    public:
        elementbox(nana::window wb) :
            nana::listbox(wb, nana::rectangle(10, 10, 500, 500))
        {
            this->append_header("Name", 100);
            this->append_header("Type", 60);
            this->append_header("Parameters", 200);
            this->checkable(true);
        }
    };

    class section_editor :
        public nana::panel<true>
    {
    public:
        // Delegating c-tor :
        // Inherited widget c-tor,
        // Class nana::drawing c-tor,
        // Vertical scroll nana::scroll<true> c-tor,
        // Horizontal scroll nana::scroll<false> c-tor,
        // Class nana::paint::image c-tor
        section_editor(nana::window wd, globals& gl, elementbox& elembox, nana::label& stat) :
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
                    if (scr_size.width < m_imgsz.width)
                        scr_size.height -= 16;
                }
                else if (scr_size.width < m_imgsz.width)
                {
                    scr_size.height -= 16;
                    if (scr_size.height < m_imgsz.height)
                        scr_size.width -= 16;
                }

                if (m_pnt.x + m_imgsz.width < scr_size.width)
                {
                    m_pnt.x = static_cast<int>(scr_size.width) - static_cast<int>(m_imgsz.width);
                    if (m_pnt.x > 0)
                        m_pnt.x = 0;
                }

                if (m_pnt.y + m_imgsz.height < scr_size.height)
                {
                    m_pnt.y = static_cast<int>(scr_size.height) - static_cast<int>(m_imgsz.height);
                    if (m_pnt.y > 0)
                        m_pnt.y = 0;
                }

                m_need_vscr = scr_size.height < m_imgsz.height;
                m_plc.field_display("vert_scroll", m_need_vscr);

                if (m_need_vscr)
                {
                    m_vscr.range(scr_size.height);
                    m_vscr.amount(m_imgsz.height);
                    m_vscr.value(-m_pnt.y);
                }
                else
                    m_pnt.y = 0;

                m_need_hscr = scr_size.width < m_imgsz.width;
                m_plc.field_display("h_bottom", m_need_hscr);

                if (m_need_hscr)
                {
                    m_hscr.range(scr_size.width);
                    m_hscr.amount(m_imgsz.width);
                    m_hscr.value(-m_pnt.x);
                }
                else
                    m_pnt.x = 0;
                m_plc.field_display("corner", m_need_vscr && m_need_hscr);

                m_img.paste(graph, m_pnt);

                ax.regenerate(m_points, m_imgsz);
                ax.draw(m_pnt, graph, gl.axisweight, gl.axisgap);

                for (const auto& i : m_shapes) {
                    i->draw(m_pnt, graph, gl.lineweight);
                }

                for (const auto& i : m_points) {
                    i->draw(m_pnt, graph, gl.pointweight);
                }

                if(gl.align){
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

            this->events().click([&, this](const nana::arg_click& mouse_arg){

                nana::point m_mouse = mouse_arg.mouse_args->pos - m_pnt;
                if(this->check_borders(m_mouse, gl.pxpermm)){

                    if(gl.align){
                        m_alignment = this->alignment(m_mouse, gl.alignradius);
                        if(m_alignment) {
                            m_align_point.coordinates() = m_mouse;
                        }
                    }
                    this->modal_form(mouse_arg, gl);

                    if(m_params.size.width*m_params.size.height) {
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

            elembox.events().checked([&](const nana::arg_listbox& arg){
                m_shapes[arg.item.pos().item]->properties().visible = arg.item.checked() ? true : false;
                for(const auto& i : m_points){
                    if(m_shapes[arg.item.pos().item]->properties().id == i->id){
                        i->visible = m_shapes[arg.item.pos().item]->properties().visible;
                    }
                }
                for (const auto& i : elembox.selected()) {
                    elembox.at(0).at(i.item).check(arg.item.checked() ? true : false);
                }
                m_dw.update();
            });

            elembox.events().key_press([&](const nana::arg_keyboard& arg){
                if(arg.key == 27) {
                    for (const auto& i : elembox.selected()){
                        elembox.at(0).at(i.item).select(false);
                    }
                    ax.regenerate(m_points, m_imgsz);
                    m_dw.update();
                }
                if(arg.key == 127) {
                    auto elems = elembox.selected();
                    elembox.erase(elems);
                    for(size_t i = 0; i < elems.size(); ++i){
                        for (size_t j = 0; j < m_points.size(); ++j) {
                            if (m_points[j]->id == m_shapes[elems[i].item - i]->properties().id) {
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

            this->events().mouse_enter([&](const nana::arg_mouse & mouse_arg){
                m_mouse = mouse_arg.pos - m_pnt;
                m_dynamic = dynamic(gl);
                if(this->check_borders(m_mouse, gl.pxpermm)){
                    stat.caption(m_dynamic);
                }
                else{
                    stat.caption(m_idle);
                }
            });

            this->events().mouse_leave([&]{
                stat.caption(m_idle);
            });

            this->events().mouse_move([&](const nana::arg_mouse& mouse_arg){
                m_mouse = mouse_arg.pos - m_pnt;
                if(this->check_borders(m_mouse, gl.pxpermm)){
                    m_dynamic = dynamic(gl);
                    stat.caption(m_dynamic);

                    if(gl.align){
                        m_alignment = this->alignment(m_mouse, gl.alignradius);
                        if(m_alignment) {
                            m_align_point.coordinates() = m_mouse;
                        }
                    }

                    m_dw.update();
                }
                else{
                    stat.caption(m_idle);
                }
            });
        }

        std::string dynamic(globals& gl) {
            return "Current position : " +
                std::to_string(m_mouse.x/gl.pxpermm) +
                "," +
                std::to_string(((m_mouse.x)%(gl.pxpermm))*10/gl.pxpermm) +
                " ; " +
                std::to_string((static_cast<int>(m_imgsz.height) - m_mouse.y)/gl.pxpermm) +
                "," +
                std::to_string(((static_cast<int>(m_imgsz.height) - m_mouse.y)%(gl.pxpermm)*10/gl.pxpermm));
        }

        bool alignment(nana::point& point, int radius) const {
            for(const auto& i : m_points){
                if(i->visible){
                    if(i->align(point, radius)){
                        return true;
                    }
                }
            }
            return false;
        }

        bool check_borders(const nana::point& point, unsigned scale) const {
            unsigned t_x = std::make_unsigned_t<int>(point.x);
            unsigned t_y = std::make_unsigned_t<int>(point.y);
            return (
                t_x <= m_imgsz.width - scale &&
                t_y <= m_imgsz.height - scale &&
                t_x >= scale &&
                t_y >= scale
            );
        }

        void modal_form(const nana::arg_click& arg, globals& gl){
            shape_constructor fm(
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

        //WARNING: made for rect class only
        void calculate(globals& gl) {
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

            for (int i = 0; i < m_shapes.size(); ++i) {
                m_shapes[i]->to_xlsx(ws, ax, i);
            }

            const int offset = static_cast<int>(m_shapes.size());

            if(offset){
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

                for(int i = 0; i < offset + 1; ++i){
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

                int h = (ax.bottom() - ax.top())*10;
                ws.cell("G3").offset(0, offset +6).value(std::to_string(h/1000) + "," + std::to_string(h%1000));
                ws.cell("G3").offset(0, offset +6).number_format(longer);
                ws.cell("G3").offset(0, offset +6).alignment(center);

                ws.cell("H3").offset(0, offset +6).value("см;");
                ws.cell("H3").offset(0, offset +6).alignment(left); 

                // Moment of inertia
                ws.cell("B3").offset(0, offset +8).value("Момент инерции сечения:");
                ws.cell("B3").offset(0, offset +8).alignment(left);

                ws.cell("F3").offset(0, offset +8).value("Ix =");
                ws.cell("F3").offset(0, offset +8).alignment(right);

                ws.cell("G3").offset(0, offset +8).formula(
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

                ws.cell("G3").offset(0, offset +10).formula(
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

        std::string environment(globals& gl) {
            int i = 0, j = 0;
            if(gl.resolution.dimension().width >= 3200){
                i = 6;
            } else if(gl.resolution.dimension().width >= 2400 && gl.resolution.dimension().width < 3200) {
                i = 5;
            } else if(gl.resolution.dimension().width >= 1800 && gl.resolution.dimension().width < 2400){
                i = 4;
            } else if(gl.resolution.dimension().width >= 1200 && gl.resolution.dimension().width < 1800){
                i = 3;
            } else if(gl.resolution.dimension().width >= 1000 && gl.resolution.dimension().width < 1200){
                i = 2;
            } else if(gl.resolution.dimension().width < 1000){
                i = 1;
            }
            if(gl.resolution.dimension().height >= 2400){
                j = 6;
            } else if(gl.resolution.dimension().height >= 1800 && gl.resolution.dimension().height < 2400) {
                j = 5;
            } else if(gl.resolution.dimension().height >= 1200 && gl.resolution.dimension().height < 1800){
                j = 4;
            } else if(gl.resolution.dimension().height >= 1000 && gl.resolution.dimension().height < 1200){
                j = 3;
            } else if(gl.resolution.dimension().height >= 850 && gl.resolution.dimension().height < 1000){
                j = 2;
            } else if(gl.resolution.dimension().height < 850){
                j = 1;
            }
            i = i > j ? i : j;
            switch(i){
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

        virtual ~section_editor() {
            for(size_t i = 0; i < m_shapes.size(); ++i) {
                delete m_shapes[i];
            }
            for(size_t i = 0; i < m_points.size(); ++i) {
                delete m_points[i];
            }
        }

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

    class axis_option_group :
        public nana::group
    {
    public:
        axis_option_group(nana::window wb, section_editor& sec) :
            nana::group(wb)
        {
            this->caption("Axis placement:");
            this->add_option("Top").events().checked([&](const nana::arg_checkbox& arg){
                if(arg.widget->checked()){
                    sec.ax.placement = true;
                    nana::API::refresh_window(sec.handle());
                } 
            });
            this->add_option("Bottom").events().checked([&](const nana::arg_checkbox& arg){
                if(arg.widget->checked()){
                    sec.ax.placement = false;
                    nana::API::refresh_window(sec.handle());
                } 
            });
            this->add_option("Hidden").events().checked([&](const nana::arg_checkbox& arg){
                if(arg.widget->checked()){
                    sec.ax.placement = true;
                    sec.ax.visible = false;
                    nana::API::refresh_window(sec.handle());
                } 
                else{
                    sec.ax.visible = true;
                    nana::API::refresh_window(sec.handle());
                }
            });
            this->radio_mode(true);
            this->option_check(0);
        }
    };

    class info :
        public nana::form
    {
    public:
        info(nana::window owner, globals& gl) :
        nana::form{
            owner, nana::API::make_center(owner, 250, 370), 
            nana::appear::decorate <nana::appear::minimize, nana::appear::taskbar, nana::appear::floating>()
        },
        m_version{*this, "SGCC v0.4"},
        m_info{*this},
        m_str{}
        {
            nana::API::window_icon(this->handle(), nana::paint::image(gl.icon_filename));

            m_str << "Weights:" << '\n';
            m_str << '\t' << "Point: " << gl.pointweight << '\n';
            m_str << '\t' << "Line: " << gl.lineweight << '\n';
            m_str << '\t' << "Axis: " << gl.axisweight << '\n';
            m_str << '\n';
            m_str << "Axis gap: " << gl.axisgap << " px" << '\n';
            m_str << '\n';
            m_str << "Scale: " << gl.pxpermm << " px/mm" << '\n';
            m_str << '\n';
            m_str << "Alignment: " << '\n';
            m_str << '\t' << "Active: " << gl.align << '\n';
            m_str << '\t' << "Radius: " << gl.alignradius << '\n';
            m_str << '\n';
            m_str << "Output table name: " << gl.xlsxname << '\n';

            m_info.caption(m_str.str());
            //m_info.text_align(nana::align::center, nana::align_v::center);
            m_version.text_align(nana::align::center, nana::align_v::center);

            m_plc.bind(*this);
            m_plc.div( R"(<vertical margin=10
                <version weight=30>
                <information>
            )");

            m_plc["version"] << m_version;
            m_plc["information"] << m_info;

            m_plc.collocate();
        }
        virtual ~info() {}
    private:
        std::stringstream m_str;
        nana::label m_version, m_info;
        nana::place m_plc;
    };

    elementbox::oresolver& operator << (elementbox::oresolver& orr , parameters& params)
    {
        orr << params.name;
        std::string temp;
        switch(params.type){
            case type::Rect :
                orr << "Rect";
                temp = "width = " + std::to_string(params.size.width) + ", height = " + std::to_string(params.size.height);
                orr << temp;
                break;
            default :
                orr << "Unknown";
                orr << "Unknown";
                std::cout << "Error!" << std::endl;
                break;
        }
        return orr ;
    }

    class general_form :
        public nana::form
    {
    public:
        general_form(globals& gl) : 
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
                        <axis_options_panel margin=5 gap=10 weight=100>
                        <control button margin=5 gap=10 weight=50>
                    >
                > 
                <tatus_panel weight = 50>
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
    private:
        nana::place m_plc;
        nana::label m_cursor_status;
        nana::button m_calculate;
        nana::menubar m_menu;
        elementbox m_elements_list;
        section_editor m_section_editor;
        axis_option_group m_axis_options;
    };

}

int main() {
    sgcc::globals cn;
    cn.lineweight = 1;
    cn.pointweight = 3;
    cn.pxpermm = 10;
    cn.align = true;
    cn.alignradius = 10;
    cn.axisgap = 20;
    cn.axisweight = 0;
    cn.id = 0;
    cn.xlsxsheetname = {"sgcc output"};
    cn.xlsxname = {"result.xlsx"};
    cn.screen = nana::screen();
    cn.resolution = cn.screen.get_primary().workarea();

    sgcc::general_form sgcc(cn);

    nana::exec();
}