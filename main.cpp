#include <QApplication>
#include "headers/todolistapp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ToDoListApp window;
    window.show();
    return app.exec();
}
