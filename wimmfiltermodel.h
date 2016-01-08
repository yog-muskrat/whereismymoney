#ifndef WIMMFILTERMODEL_H
#define WIMMFILTERMODEL_H

#include <QSortFilterProxyModel>

class WIMMFilterModel : public QSortFilterProxyModel
{
public:
	WIMMFilterModel(QObject *parent = 0);

	void setMonthsFilter(QList<int> monthsIds);
	void clearMonthsFilter();

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
	QList<int> mMonths;
};

#endif // WIMMFILTERMODEL_H
