#ifndef WIMMFILTERMODEL_H
#define WIMMFILTERMODEL_H

#include <QSortFilterProxyModel>

class WIMMFilterModel : public QSortFilterProxyModel
{
public:
	explicit WIMMFilterModel(QObject *parent = nullptr);

	void setMonthsFilter(QList<int> monthsIds);
	void clearMonthsFilter();

	QVariant data(const QModelIndex &index, int role) const override;

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
	QList<int> mMonths;
};

#endif // WIMMFILTERMODEL_H
