#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "wimmmodel.h"
#include "addmonthdialog.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	pModel = DataLoader::loadModel();
	pModel->setParent(this);

	ui->listView->setModel(pModel);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
	WIMMObject *obj = pModel->objectAt(index);
	if(!obj)
	{
		return;
	}

	ui->tableView->setModel(obj->moneyModel());
	ui->tableView->setColumnHidden( MoneyModel::COL_RecordId, true);
}

void MainWindow::on_pbAddMonth_clicked()
{
	AddMonthDialog amd;
	if(!amd.exec())
	{
		return;
	}

	WIMMObject *obj = new WIMMObject();
	obj->setYear(amd.year());
	obj->setMonth(amd.month());

	pModel->appendObject( obj );
}

void MainWindow::on_pbRemoveMonth_clicked()
{

}

void MainWindow::on_pbSave_clicked()
{

}

void MainWindow::on_pbCancel_clicked()
{

}
