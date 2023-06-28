#include "Widget.h"
#include "SocketClient.h"
#include "SocketServer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget widget;

    widget.show();
    return a.exec();
}
