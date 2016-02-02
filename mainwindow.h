#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>
#include <QMap>

class QMenu;
class QAction;
class WIMMModel;
class WIMMFilterModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void onSelectionChanged();
	void on_pbAddMonth_clicked();
	void on_pbRemoveMonth_clicked();
	void calcTotals();
	void onMenuRequested(const QPoint &p);
	void onEditComment();
	void onAddMoney();
	void on_action_categories_triggered();
	void on_action_fonts_triggered();
	void on_treeView_clicked(const QModelIndex &index);
	void setDirty(bool dirty = true);
	void onSave();

	void onCopyValue();
	void onPasteValue();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);

private:
	void createMenu();

	Ui::MainWindow *ui;

	WIMMModel *pModel;
	WIMMFilterModel *pFilterModel;
	QMenu *pMenu;
	QMap<QString, QAction*> mActions;
	bool mDirty;

	QVariant mBuffer;
};

#endif // MAINWINDOW_H
