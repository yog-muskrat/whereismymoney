#include "changevaluecommand.h"
#include "wimmmodel.h"
#include "tools.h"

ChangeValueCommand::ChangeValueCommand(const QModelIndex &index, QVariant data, WIMMModel *model) :
	QUndoCommand()
{
	mIndex = QPersistentModelIndex(index);
	mOld = index.data(Qt::EditRole);
	mNew = data;
	pModel = model;

	WIMMItem *item = model->itemForIndex(index);

	pItem = static_cast<CategoryItem*>(item);

	Q_ASSERT(pItem && pItem->level() == Category);

	int year = pItem->group()->month()->year();
	QString month = pItem->group()->month()->monthName();
	QString category = pItem->name();

	QString mOldString = Tools::moneyString( mOld.toDouble());
	QString mNewString = Tools::moneyString( mOld.toDouble());

	setText( QString("%0 %1 %2. Было %3 стало %4.").arg(year).arg(month, category, mOldString, mNewString));
}

void ChangeValueCommand::undo()
{
	WIMMModel::Column col = static_cast<WIMMModel::Column>(mIndex.column());

	if(col == WIMMModel::COL_FirstHalfEst)
	{
		pItem->setFirstHalfEstimated(mOld.toDouble());
	}
	else if(col == WIMMModel::COL_FirstHalfIn)
	{
		pItem->setFirstHalfIncome(mOld.toDouble());
	}
	else if(col == WIMMModel::COL_FirstHalfOut)
	{
		pItem->setFirstHalfOutcome(mOld.toDouble());
	}
	else if(col == WIMMModel::COL_SecondHalfEst)
	{
		pItem->setSecondHalfEstimated(mOld.toDouble());
	}
	else if(col == WIMMModel::COL_SecondHalfIn)
	{
		pItem->setSecondHalfIncome(mOld.toDouble());
	}
	else if(col == WIMMModel::COL_SecondHalfOut)
	{
		pItem->setSecondHalfOutcome(mOld.toDouble());
	}

	pModel->emitDataChanged( pModel->index(mIndex.row(), mIndex.column(), mIndex.parent()));
}

void ChangeValueCommand::redo()
{
	WIMMModel::Column col = static_cast<WIMMModel::Column>(mIndex.column());

	if(col == WIMMModel::COL_FirstHalfEst)
	{
		pItem->setFirstHalfEstimated(mNew.toDouble());
	}
	else if(col == WIMMModel::COL_FirstHalfIn)
	{
		pItem->setFirstHalfIncome(mNew.toDouble());
	}
	else if(col == WIMMModel::COL_FirstHalfOut)
	{
		pItem->setFirstHalfOutcome(mNew.toDouble());
	}
	else if(col == WIMMModel::COL_SecondHalfEst)
	{
		pItem->setSecondHalfEstimated(mNew.toDouble());
	}
	else if(col == WIMMModel::COL_SecondHalfIn)
	{
		pItem->setSecondHalfIncome(mNew.toDouble());
	}
	else if(col == WIMMModel::COL_SecondHalfOut)
	{
		pItem->setSecondHalfOutcome(mNew.toDouble());
	}

	pModel->emitDataChanged( pModel->index(mIndex.row(), mIndex.column(), mIndex.parent()));
}
