#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class WIMMModel;

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
	void on_listView_clicked(const QModelIndex &index);

	void on_pbAddMonth_clicked();

	void on_pbRemoveMonth_clicked();

	void on_pbSave_clicked();

	void on_pbCancel_clicked();

private:
	Ui::MainWindow *ui;

	WIMMModel* pModel;
};

#endif // MAINWINDOW_H
