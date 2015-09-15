#include <QApplication>
#include <QQmlContext>
#include <QAbstractItemModel>
#include <QQmlApplicationEngine>
#include "dbmanager.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	DBManager dbm;
	QAbstractItemModel *model = dbm.categoriesModel();

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("catModel", model);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	return app.exec();
}
