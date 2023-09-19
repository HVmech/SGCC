#pragma once

#include "general_form.hpp"
#include "axis_option_group.hpp"
#include "section_editor.hpp"
#include "shape_constructor.hpp"
#include "elementbox.hpp"
#include "shapes.hpp"
#include "info.hpp"
#include "axis.hpp"
#include "globals.hpp"
#include "point.hpp"
#include "parameters.hpp"
#include "headers.h"

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