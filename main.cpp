#include "smileyikblogtool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SmileYikBlogTool w;
    w.show();
    return a.exec();
}
