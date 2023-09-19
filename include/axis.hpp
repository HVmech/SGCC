#ifndef SGCC_AXIS
#define SGCC_AXIS

// sgcc headers
#include "point.hpp"

#include "headers.h"

namespace sgcc
{
    class axis
    {
    public:
        axis() = default;

        virtual ~axis();

        void regenerate(std::vector<point*>& arr, nana::size& size);

        void draw(nana::point& base, nana::paint::graphics& graph, int weight, int gap);

        int top() const;

        int bottom() const;

        bool placement = true, visible = true;
        nana::colors color = nana::colors::black;

    private:
        void draw_axis(const nana::point& pnt1, const nana::point& pnt2, nana::paint::graphics& graph, int weight, int gap);
        bool m_visible = false;
        nana::point m_tl, m_br;
    };
}
#endif