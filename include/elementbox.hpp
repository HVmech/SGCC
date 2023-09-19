#ifndef SGCC_ELEMENTBOX
#define SGCC_ELEMENTBOX

// sgcc headers
#include "parameters.hpp"

#include "headers.h"

namespace sgcc
{
    class elementbox :
        public nana::listbox
    {
    public:
        elementbox(nana::window wb);
    };

    inline elementbox::elementbox(nana::window wb) :
        nana::listbox(wb, nana::rectangle(10, 10, 500, 500))
    {
        this->append_header("Name", 100);
        this->append_header("Type", 60);
        this->append_header("Parameters", 200);
        this->checkable(true);
    }

    elementbox::oresolver& operator<< (elementbox::oresolver& orr , parameters& params)
    {
        orr << params.name;

        std::string temp;

        switch(params.type)
        {
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

        return orr;
    }
}
#endif


