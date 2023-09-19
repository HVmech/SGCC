#include "point.hpp"

namespace sgcc
{
    point::point(nana::point pnt, size_t i, nana::colors clr1, nana::colors clr2, bool vis) :
        m_point{pnt.x, pnt.y},
        id{i},
        color1{clr1},
        color2{clr2},
        visible{vis}
    {}

    nana::point& point::coordinates() {
        return m_point;
    }

    bool point::align(nana::point& point, int radius) const
    {
        if
        (
            m_point.x < point.x + radius &&
            m_point.x > point.x - radius &&
            m_point.y < point.y + radius &&
            m_point.y > point.y - radius
        )
        {
            point = m_point;
            return true;
        }
        else
        {
            return false;
        }
    }

    void point::draw(nana::point& base, nana::paint::graphics& graph, int weight) const {
        if (visible)
        {
            for(int i = -weight; i <= weight; ++i)
            {
                for(int j = -weight; j <= weight; ++j)
                {
                    if(i*i + j*j <= weight*weight)
                    {
                        if(i*i + j*j < (weight -1)*(weight -1))
                        {
                            graph.set_pixel(base.x + m_point.x + i, base.y + m_point.y + j, color2);
                        }
                        else
                        {
                            graph.set_pixel(base.x + m_point.x + i, base.y + m_point.y + j, color1);
                        }
                    }
                }
            }
        }
    }

    point::~point(){}
}