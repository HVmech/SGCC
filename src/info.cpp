#include "info.hpp"

namespace sgcc
{
    info::info(nana::window owner, globals& gl) :
        nana::form
        {
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
}