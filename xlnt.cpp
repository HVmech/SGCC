#include <xlnt/xlnt.hpp>
#include <iostream>

int main()
{
    xlnt::workbook wb;
    std::cout << "Step 1" << std::endl;

    xlnt::worksheet ws = wb.active_sheet();
    std::cout << "Step 2" << std::endl;

    ws.cell("A1").value(5);
    ws.cell("B2").value("string data");
    ws.cell("C3").formula("=RAND()");
    std::cout << "Step 3" << std::endl;

    ws.merge_cells("C3:C4");
    std::cout << "Step 4" << std::endl;

    ws.freeze_panes("B2");
    std::cout << "Step 5" << std::endl;

    const std::string example_filename{"example.xlsx"};
    //std::fstream file{filename};
    wb.save(example_filename);
    std::cout << "Step 6" << std::endl;

    return 0;
}
// compile with -std=c++14 -Ixlnt/include -lxlnt