#include "wimmfiltermodel.h"
#include "wimmmodel.h"

WIMMFilterModel::WIMMFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void WIMMFilterModel::setMonthsFilter(QList<int> monthsIds)
{
	mMonths = monthsIds;
	invalidateFilter();
}

void WIMMFilterModel::clearMonthsFilter()
{
	setMonthsFilter(QList<int>());
}

bool WIMMFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	WIMMModel *model = qobject_cast<WIMMModel*>(sourceModel());
	if(!model)
	{
		return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
	}

	/// Пока считаем пустой список равным отсутствию фильтрации
	if(mMonths.isEmpty())
	{
		return true;
	}

	int id = model->monthId( model->index(source_row, 0, source_parent) );
	return mMonths.contains(id);
}
