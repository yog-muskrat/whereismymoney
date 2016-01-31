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

	virtual void undo();
	virtual void redo();

private:
	WIMMModel *pModel;
	QVariant mOld;
	QVariant mNew;
	QPersistentModelIndex mIndex;
	CategoryItem *pItem;
	WIMMItem::Column mItemColumn;
};

#endif // CHANGEVALUECOMMAND_H
