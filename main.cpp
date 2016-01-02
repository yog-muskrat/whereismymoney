#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "mainwindow.h"
#include "wimmmodel.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

#ifdef QMLVERSION
	QList<QObject*> model = DataLoader::loadData();

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("wimmmodel", QVariant::fromValue(model));
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
#else
	MainWindow mainwindow;
	mainwindow.show();
#endif

	return app.exec();
}
