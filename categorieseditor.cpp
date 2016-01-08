#include "categorieseditor.h"
#include "ui_categorieseditor.h"

#include "sqltools.h"

#include <QMenu>
#include <QDebug>
#include <QSqlRecord>
#include <QInputDialog>

CategoriesEditor::CategoriesEditor(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CategoriesEditor)
{
	ui->setupUi(this);

	ui->treeWidget->setContextMenuPolicy( Qt::CustomContextMenu);

	connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));

	createMenu();

	fillGroups();
}

CategoriesEditor::~CategoriesEditor()
{
	delete ui;
}

void CategoriesEditor::accept()
{
	if(!saveGroups())
	{
		return;
	}

	QDialog::accept();
}

void CategoriesEditor::fillGroups()
{
	QSqlQuery query = SqlTools::emptyQuery();
	query.prepare("SELECT id, name FROM category_groups ORDER BY pos");

	if(!SqlTools::execQuery(query))
	{
		return;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
		item->setText(0, rec.value("name").toString());
		item->setData(0, IdRole, rec.value("id"));
		item->setData(0, TypeRole, "group");
		item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		QFont f = item->font(0);
		f.setBold(true);
		item->setFont(0, f);


		fillCategories(item);
	}
}

void CategoriesEditor::fillCategories(QTreeWidgetItem *groupItem)
{
	Q_ASSERT(groupItem);

	int groupId = groupItem->data(0, IdRole).toInt();

	QSqlQuery query = SqlTools::emptyQuery();
	query.prepare("SELECT id, name FROM categories WHERE category_group=:group_id ORDER BY pos");
	query.bindValue(":group_id", groupId);

	if(!SqlTools::execQuery(query))
	{
		return;
	}

	while(query.next())
	{
		QSqlRecord rec = query.record();

		QTreeWidgetItem *item = new QTreeWidgetItem(groupItem);
		item->setText(0, rec.value("name").toString());
		item->setData(0, IdRole, rec.value("id"));
		item->setData(0, TypeRole, "category");
		item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
	}
}

bool CategoriesEditor::saveGroups()
{
	for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *groupItem = ui->treeWidget->topLevelItem(i);

		QSqlQuery query = SqlTools::emptyQuery();

		int id = groupItem->data(0, IdRole).toInt();

		if(groupItem->isHidden() && id > 0)
		{
			query.prepare("DELETE FROM category_groups WHERE id=:id");
			query.bindValue(":id", id);

			if(!SqlTools::execQuery(query))
			{
				return false;
			}
		}
		else
		{
			if(id > 0)
			{
				query.prepare("UPDATE category_groups SET name=:name, pos=:pos WHERE id=:id");
				query.bindValue(":id", id);
			}
			else
			{
				query.prepare("INSERT INTO category_groups (name, pos) VALUES (:name, :pos)");
			}

			query.bindValue(":name", groupItem->text(0));
			query.bindValue(":pos", i);

			if(!SqlTools::execQuery(query))
			{
				return false;
			}

			if(id < 0)
			{
				groupItem->setData(0, IdRole, query.lastInsertId());
			}

			if(!saveCategories(groupItem))
			{
				return false;
			}
		}
	}

	return true;
}

bool CategoriesEditor::saveCategories(QTreeWidgetItem *groupItem)
{
	Q_ASSERT(groupItem && groupItem->data(0, TypeRole).toString() == "group");

	for(int i = 0; i < groupItem->childCount(); ++i)
	{
		QTreeWidgetItem *categoryItem = groupItem->child(i);

		Q_ASSERT(categoryItem->data(0, TypeRole).toString() == "category");

		QSqlQuery query = SqlTools::emptyQuery();

		int id = categoryItem->data(0, IdRole).toInt();

		if(categoryItem->isHidden() && id > 0)
		{
			query.prepare("DELETE FROM categories WHERE id=:id");
			query.bindValue(":id", id);

			if(!SqlTools::execQuery(query))
			{
				return false;
			}
		}
		else
		{
			if(id > 0)
			{
				query.prepare("UPDATE categories SET name=:name, pos=:pos, category_group=:group WHERE id=:id");
				query.bindValue(":id", id);
			}
			else
			{
				query.prepare("INSERT INTO categories (name, pos, category_group) VALUES (:name, :pos, :group)");
			}

			query.bindValue(":name", categoryItem->text(0));
			query.bindValue(":pos", i);
			query.bindValue(":group", groupItem->data(0, IdRole));

			if(!SqlTools::execQuery(query))
			{
				return false;
			}
		}
	}

	return true;
}

void CategoriesEditor::on_pbUp_clicked()
{
	QTreeWidgetItem *item = ui->treeWidget->currentItem();

	if(!item)
	{
		return;
	}

	QTreeWidgetItem *parent = item->parent();

	if(parent)
	{
		int idx = parent->indexOfChild( item );
		if(idx > 0)
		{
			item = parent->takeChild(idx);
			parent->insertChild(idx-1, item );
		}
	}
	else
	{
		int idx = ui->treeWidget->indexOfTopLevelItem(item);
		if(idx > 0)
		{
			bool expanded = item->isExpanded();
			item = ui->treeWidget->takeTopLevelItem(idx);
			ui->treeWidget->insertTopLevelItem(idx - 1, item);
			item->setExpanded(expanded);
		}
	}

	ui->treeWidget->setCurrentItem( item );
}

void CategoriesEditor::on_pbDown_clicked()
{
	QTreeWidgetItem *item = ui->treeWidget->currentItem();

	if(!item)
	{
		return;
	}
	QTreeWidgetItem *parent = item->parent();

	if(parent)
	{
		int idx = parent->indexOfChild( item );
		if(idx < parent->childCount()-1 )
		{
			item = parent->takeChild(idx);
			parent->insertChild(idx+1, item );
		}
	}
	else
	{
		int idx = ui->treeWidget->indexOfTopLevelItem(item);
		if(idx < ui->treeWidget->topLevelItemCount()-1)
		{
			bool expanded = item->isExpanded();
			item = ui->treeWidget->takeTopLevelItem(idx);
			ui->treeWidget->insertTopLevelItem(idx + 1, item);
			item->setExpanded(expanded);
		}
	}

	ui->treeWidget->setCurrentItem( item );
}

void CategoriesEditor::onContextMenuRequested(const QPoint &point)
{
	QTreeWidgetItem *item = ui->treeWidget->itemAt(point);

	mAction["remove"]->setVisible( item );
	mAction["add_category"]->setVisible( item );

	pMenu->exec( ui->treeWidget->mapToGlobal(point) );
}

void CategoriesEditor::onAddCategory()
{
	QTreeWidgetItem *parent = ui->treeWidget->currentItem();
	Q_ASSERT(parent);

	if(parent->data(0, TypeRole).toString() == "category")
	{
		parent = parent->parent();
	}

	QString name = QInputDialog::getText(this, "Новая категория", "Укажите название категории затрат");
	if(name.isEmpty())
	{
		return;
	}

	QTreeWidgetItem *item = new QTreeWidgetItem(parent);
	item->setText(0, name);
	item->setData(0, IdRole, -1);
	item->setData(0, TypeRole, "category");
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);

	parent->setExpanded(true);
	ui->treeWidget->setCurrentItem(item);
}

void CategoriesEditor::onRemove()
{
	QTreeWidgetItem *item = ui->treeWidget->currentItem();

	Q_ASSERT(item);

	ui->treeWidget->setItemHidden(item, true);
}

void CategoriesEditor::onAddGroup()
{
	QString name = QInputDialog::getText(this, "Новая группа", "Укажите название группы");
	if(name.isEmpty())
	{
		return;
	}

	QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
	item->setText(0, name);
	item->setData(0, IdRole, -1);
	item->setData(0, TypeRole, "group");
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	QFont f = item->font(0);
	f.setBold(true);
	item->setFont(0, f);

	ui->treeWidget->setCurrentItem( item );
}

void CategoriesEditor::createMenu()
{
	pMenu = new QMenu(this);
	mAction["add_group"] = pMenu->addAction("Добавить группу", this, SLOT(onAddGroup()));
	mAction["add_category"] = pMenu->addAction("Добавить категорию", this, SLOT(onAddCategory()));
	mAction["remove"] = pMenu->addAction("Удалить", this, SLOT(onRemove()));
}
