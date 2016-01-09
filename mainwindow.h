#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
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
	void on_action_categories_triggered();
	void on_action_fonts_triggered();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);

private:
	void createMenu();

	void fillTotalsTree(QList<int> months);

	Ui::MainWindow *ui;

	WIMMModel *pModel;
	WIMMFilterModel *pFilterModel;
	QMenu *pMenu;
};

#endif // MAINWINDOW_H
