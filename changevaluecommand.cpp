#include "changevaluecommand.h"
#include "wimmmodel.h"
#include "tools.h"

#include <QDebug>

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

	WIMMModel::Column col = static_cast<WIMMModel::Column>(mIndex.column());
	if(col == WIMMModel::COL_FirstHalfEst)
	{
		mItemColumn = WIMMItem::FirstEst;
	}
	else if(col == WIMMModel::COL_FirstHalfIn)
	{
		mItemColumn = WIMMItem::FirstIn;
	}
	else if(col == WIMMModel::COL_FirstHalfOut)
	{
		mItemColumn = WIMMItem::FirstOut;
	}
	else if(col == WIMMModel::COL_SecondHalfEst)
	{
		mItemColumn = WIMMItem::SecondEst;
	}
	else if(col == WIMMModel::COL_SecondHalfIn)
	{
		mItemColumn = WIMMItem::SecondIn;
	}
	else if(col == WIMMModel::COL_SecondHalfOut)
	{
		mItemColumn = WIMMItem::SecondOut;
	}

	setText( QString("[%0 %1 %2] Было %3 стало %4.").arg(year).arg(month, category, mOldString, mNewString));
}

void ChangeValueCommand::undo()
{
	pItem->setValue(mItemColumn, mOld.toDouble());

	notifyDataChanged();
}

void ChangeValueCommand::redo()
{
	pItem->setValue(mItemColumn, mNew.toDouble());

	notifyDataChanged();
}

void ChangeValueCommand::notifyDataChanged()
{
	QModelIndex bottomRight = pModel->index(mIndex.row(), mIndex.column(), mIndex.parent());

	QModelIndex topLeft = bottomRight.parent().parent();
	pModel->emitDataChanged( topLeft, bottomRight);
}
