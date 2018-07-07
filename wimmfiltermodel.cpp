#include "wimmfiltermodel.h"
#include "wimmmodel.h"

#include <QSize>
#include <QColor>

WIMMFilterModel::WIMMFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void WIMMFilterModel::setMonthsFilter(QList<int> monthsIds)
{
	mMonths = std::move(monthsIds);
	invalidateFilter();
}

void WIMMFilterModel::clearMonthsFilter()
{
	setMonthsFilter(QList<int>());
}

QVariant WIMMFilterModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::BackgroundRole)
	{
		QModelIndex src = mapToSource(index);
		if(src.isValid() && !src.parent().isValid())
		{
			auto color = src.data(role).value<QColor>();
			return color.darker(110);
		}
	}

	return QSortFilterProxyModel::data(index, role);
}

bool WIMMFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	auto *model = qobject_cast<WIMMModel*>(sourceModel());
	if(model == nullptr)
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
