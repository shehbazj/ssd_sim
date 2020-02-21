#include <QApplication>
#include "digitalclock.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	DigitalClock d;
	d.show();
	
    return app.exec();
}
