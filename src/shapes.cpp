#include "shapes.hpp"

namespace sgcc
{
    rect::rect(const parameters& params) :
        m_params{params}
    {}

    void rect::properties(parameters& params)
    {
        m_params = params;
    }

    parameters& rect::properties()
    {
        return m_params;
    }
        
    void rect::draw(nana::point& base, nana::paint::graphics& graph, int weight) const
    {
        if (m_params.visible)
        {
            for(int i = -weight; i <= weight; ++i)
            {
                for(int j = -weight; j <= weight; ++j)
                {
                    if(i*i + j*j <= weight*weight)
                    {
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
    
    void rect::add_points(std::vector<point*>& arr, nana::colors clr) const
    {
        arr.push_back(new point
            {
                m_params.placement,
                m_params.id, m_params.color, clr, m_params.visible
            }
        );
        arr.push_back(new point
            {
                nana::point{
                    m_params.placement.x + static_cast<int>(m_params.size.width),
                    m_params.placement.y
                }, m_params.id, m_params.color, clr, m_params.visible
            }
        );
        arr.push_back(new point
            {
                nana::point{
                    m_params.placement.x + static_cast<int>(m_params.size.width),
                    m_params.placement.y + static_cast<int>(m_params.size.height)
                }, m_params.id, m_params.color, clr, m_params.visible
            }
        );
        arr.push_back(new point
            {
                nana::point{
                    m_params.placement.x,
                    m_params.placement.y + static_cast<int>(m_params.size.height)
                }, m_params.id, m_params.color, clr, m_params.visible
            }
        );            
    }
    
    void rect::to_xlsx(xlnt::worksheet& ws, axis& ax, int ind) const
    {
        double a = static_cast<double>(m_params.size.width)*10;
        double b = static_cast<double>(m_params.size.height)*10;
        double y;

        if (ax.placement)
        {
            y = static_cast<double>(m_params.placement.y - ax.top())*10 + b/2;
        }
        else
        {
            y = static_cast<double>(ax.bottom() - m_params.placement.y)*10 - b/2;
        }

        ws.cell("B3").offset(0, ind).value(ind+1);
        ws.cell("C3").offset(0, ind).value(a/1000);
        ws.cell("D3").offset(0, ind).value(b/1000);
        ws.cell("F3").offset(0, ind).value(y/1000);
        std::string r = std::to_string(3+ind);
        ws.cell("E3").offset(0, ind).formula(std::string{"=C" + r + "*D" + r});
        ws.cell("G3").offset(0, ind).formula(std::string{"=E" + r + "*F" + r});
        ws.cell("H3").offset(0, ind).formula(std::string{"=G" + r + "*F" + r});
        ws.cell("I3").offset(0, ind).formula(std::string{"=C" + r + "*POWER(D" + r + ",3)/12"});
    }
}