#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QDebug>


#include "wimmmodel.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QList<QObject*> model = DataLoader::loadData();

	QQmlApplicationEngine engine;

	engine.rootContext()->setContextProperty("wimmmodel", QVariant::fromValue(model));

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
