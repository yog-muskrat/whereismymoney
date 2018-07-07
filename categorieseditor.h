#ifndef CATEGORIESEDITOR_H
#define CATEGORIESEDITOR_H

#include <QMap>
#include <QDialog>

class QMenu;
class QAction;
class QTreeWidgetItem;

namespace Ui {
class CategoriesEditor;
}

class CategoriesEditor : public QDialog
{
	Q_OBJECT

public:
	explicit CategoriesEditor(QWidget *parent = nullptr);
	~CategoriesEditor() override;

public slots:
	 void accept() override;

private slots:
	void on_pbUp_clicked();
	void on_pbDown_clicked();

	void onContextMenuRequested(const QPoint &point);

	void onAddCategory();
	void onRemove();
	void onAddGroup();

private:
	enum DataRole
	{
		IdRole = Qt::UserRole + 1,
		TypeRole
	};

	Ui::CategoriesEditor *ui;

	void createMenu();

	void fillGroups();
	void fillCategories(QTreeWidgetItem *groupItem);

	bool saveGroups();
	bool saveCategories(QTreeWidgetItem *groupItem);

	QMenu *pMenu;
	QMap<QString, QAction*> mAction;
};

#endif // CATEGORIESEDITOR_H
