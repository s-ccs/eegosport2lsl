#define EEGO_SDK_BIND_STATIC // How to bind


#include "impedancecheck_test.h"

#include <QApplication>
#include <QLabel>
#include <qlayout.h>
#include <iostream>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
	
    ImpedanceCheck_test w;
    w.show();
    return a.exec();
	
}


