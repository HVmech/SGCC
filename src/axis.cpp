#include "axis.hpp"

namespace sgcc
{
    axis::~axis() {}

    void axis::regenerate(std::vector<point*>& arr, nana::size& size)
    {
        if(visible)
        {
            if(arr.size())
            {
                m_br.x = 0;
                m_br.y = 0;
                m_tl.x = static_cast<int>(size.width);
                m_tl.y = static_cast<int>(size.height);
                
                for(const auto& i : arr)
                {
                    if(i->visible)
                    {
                        if(m_br.x < i->coordinates().x) m_br.x = i->coordinates().x;
                        if(m_br.y < i->coordinates().y) m_br.y = i->coordinates().y;
                        if(m_tl.x > i->coordinates().x) m_tl.x = i->coordinates().x;
                        if(m_tl.y > i->coordinates().y) m_tl.y = i->coordinates().y;
                    }
                }

                if(m_br.x && m_br.y)
                {
                    m_visible = true;
                }
                else
                {
                    m_visible = false;
                }
            }
            else
            {
                m_visible = false;
            }
        }
    }

    void axis::draw(nana::point& base, nana::paint::graphics& graph, int weight, int gap)
    {
        if(visible)
        {
            if (placement)
            {
                draw_axis(m_tl + base, nana::point(m_br.x + base.x, m_tl.y + base.y), graph, weight, gap);
            }
            else
            {
                draw_axis(nana::point(m_tl.x + base.x, m_br.y + base.y), m_br + base, graph, weight, gap);
            }
        }
    }

    int axis::top() const
    {
        return m_tl.y;
    }

    int axis::bottom() const
    {
        return m_br.y;
    }

    void axis::draw_axis(const nana::point& pnt1, const nana::point& pnt2, nana::paint::graphics& graph, int weight, int gap)
    {
        if(m_visible)
        {
            nana::point temp {pnt1.x -2*gap, pnt1.y};

            while(temp.x < pnt2.x + 2*gap)
            {
                for(int i = -weight; i <= weight; ++i)
                {
                    graph.line_begin(temp.x, temp.y + i);
                    graph.line_to(nana::point(temp.x + gap , temp.y + i), nana::colors::black);
                }
                
                temp.x += 2*gap;
            }
        }
    }
}