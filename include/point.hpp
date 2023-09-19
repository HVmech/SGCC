#ifndef SGCC_POINT
#define SGCC_POINT

#include "headers.h"

namespace sgcc
{
    class point
    {
    public:
        point() = default;

        point(nana::point pnt, size_t i, nana::colors clr1, nana::colors clr2, bool vis);

        nana::point& coordinates();

        bool align(nana::point& point, int radius) const;

        void draw(nana::point& base, nana::paint::graphics& graph, int weight) const;

        virtual ~point();
        
        size_t id;
        nana::colors color1, color2;
        bool visible;
    private:
        nana::point m_point;
    };
}
#endif