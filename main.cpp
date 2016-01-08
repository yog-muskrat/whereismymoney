#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

#ifdef WIN32
	QFont f = app.font();
	f.setPointSize(12);
	f.setFamily("Calibri");
#endif

	MainWindow mainwindow;
	mainwindow.show();

	return app.exec();
}
