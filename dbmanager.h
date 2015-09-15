#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
class QAbstractItemModel;


class DBManager : public QObject
{
	Q_OBJECT
//	Q_PROPERTY(QAbstractItemModel* categoriesModel READ categoriesModel )
//	Q_PROPERTY(QAbstractItemModel* dataModel READ dataModel )
public:
	explicit DBManager(QObject *parent = 0);

	QAbstractItemModel *categoriesModel();
	QAbstractItemModel *dataModel();

signals:

public slots:
};

#endif // DBMANAGER_H
