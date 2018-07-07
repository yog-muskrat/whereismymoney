#ifndef CHANGEVALUECOMMAND_H
#define CHANGEVALUECOMMAND_H

#include "structs.h"
#include <QVariant>
#include <QUndoCommand>
#include <QPersistentModelIndex>

class WIMMModel;
class QModelIndex;

class ChangeValueCommand : public QUndoCommand
{
public:
	ChangeValueCommand(const QModelIndex &index, QVariant data, WIMMModel *model);

	void undo() override;
	void redo() override;

private:
	void notifyDataChanged();

	WIMMModel *pModel{nullptr};
	QVariant mOld;
	QVariant mNew;
	QPersistentModelIndex mIndex;
	CategoryItem *pItem{nullptr};
	WIMMItem::Column mItemColumn{WIMMItem::Column::FirstIn};
};

#endif // CHANGEVALUECOMMAND_H
