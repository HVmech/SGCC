#ifndef SGCC_SHAPES
#define SGCC_SHAPES

// sgcc headers
#include "parameters.hpp"
#include "axis.hpp"

#include "headers.h"

namespace sgcc
{
    class shape
    {
    public:
        virtual ~shape() = default;
        virtual void properties(parameters& params) = 0;
        virtual parameters& properties() = 0;
        virtual void draw(nana::point& base, nana::paint::graphics& graph, int weight) const = 0;
        virtual void add_points(std::vector<point*>& arr, nana::colors clr) const = 0;
        virtual void to_xlsx(xlnt::worksheet& ws, axis& ax, int ind) const = 0;
    };

    class rect : public shape
    {
    public:
        rect(const parameters& params);
        void properties(parameters& params) override;
        parameters& properties() override;
        void draw(nana::point& base, nana::paint::graphics& graph, int weight) const override;
        void add_points(std::vector<point*>& arr, nana::colors clr) const override;
        void to_xlsx(xlnt::worksheet& ws, axis& ax, int ind) const override;
        virtual ~rect() {}
    private:
        parameters m_params;
    };
}
#endif