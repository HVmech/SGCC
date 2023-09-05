/* //Include nana/gui.hpp header file to enable Nana C++ Library
//for the program.
#include <nana/gui.hpp>
//Include a label widget, we will use it in this example.
#include <nana/gui/widgets/label.hpp>
int main()
{
    //All names of Nana is in the namespace nana;
    using namespace nana;
    //Define a form object, class form will create a window
    //when a form instance is created.
    //The new window default visibility is false.
    form fm;
    //Define a label on the fm(form) with a specified area,
    //and set the caption.
    label lb{ fm, rectangle{ 10, 10, 100, 100 } };
    lb.caption("Hello, world!");
    //Expose the form.
    fm.show();
    //Pass the control of the application to Nana's event
    //service. It blocks the execution for dispatching user
    //input until the form is closed.
    exec();
} */


#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <xlnt/xlnt.hpp>
#include <iostream>

void foo() {
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

    //const std::string example_filename{"example.xlsx"};
    //std::fstream file{filename};
    wb.save("example.xlsx");//example_filename);
    std::cout << "Step 6" << std::endl;
}

int main()
{
    using namespace nana;

    //Define a form.
    form fm;

    //Define a label and display a text.
    label lab{fm, "Hello, <bold blue size=16>Nana C++ Library</>"};
    lab.format(true);

    //Define a button and answer the click event.
    button btn{fm, ":D"};
    btn.events().click([&fm]{
        foo();
        fm.close();
    });

    //Layout management
    fm.div("vert <><<><weight=80% text><>><><weight=24<><button><>><>");
    fm["text"]<<lab;
    fm["button"] << btn;
    fm.collocate();
	
    //Show the form
    fm.show();

    //Start to event loop process, it blocks until the form is closed.
    exec();
}