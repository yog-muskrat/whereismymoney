#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "tools.h"
#include "structs.h"
#include "sqltools.h"
#include "wimmmodel.h"
#include "addmonthdialog.h"
#include "categorieseditor.h"

#include <QDate>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->listView->setAlternatingRowColors(true);
	ui->treeView->header()->setStretchLastSection(true);

	if(!SqlTools::openDb())
	{
		QMessageBox::critical(this, "Ошибка открытия БД", "Убедитесь, что файл БД существует и доступен для чтения.", QMessageBox::Close);
		return;
	}

	pModel = SqlTools::loadModel();
	pModel->setParent(this);

	ui->listView->setModel(pModel);
	ui->listView->setModelColumn(WIMMModel::COL_Title);

	ui->treeView->setModel(pModel);
	ui->treeView->setColumnHidden(WIMMModel::COL_DbId, true);
	ui->treeView->resizeColumnToContents(WIMMModel::COL_Title);
}

MainWindow::~MainWindow()
{
	SqlTools::closeDb();
	delete ui;
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
	ui->treeView->setRootIndex(index);
	ui->treeView->resizeColumnToContents( WIMMModel::COL_Title);

	for(int i = 0; i < pModel->rowCount(index); ++i)
	{
		ui->treeView->expand( index.child(i, 0));
	}
}

void MainWindow::on_pbAddMonth_clicked()
{
	AddMonthDialog amd;
	if(!amd.exec())
	{
		return;
	}

	int year = amd.year();
	int month = amd.month();

	if(SqlTools::monthRecordExists(year, month))
	{
		QMessageBox::information(this, "Новый месяц", "Заданный месяц уже существует", QMessageBox::Ok);
		return;
	}

	MonthItem *item = SqlTools::addMonthRecord(year, month);

	if(!item)
	{
		return;
	}

	pModel->addMonth( item );
}

void MainWindow::on_pbRemoveMonth_clicked()
{
	QModelIndex idx = ui->listView->currentIndex();
	if(!idx.isValid())
	{
		return;
	}

	int btn = QMessageBox::question(this,
									"Удаление месяца",
									"Выбранный месяц и будет удален из БД. Продолжить?",
									QMessageBox::Yes,
									QMessageBox::No);

	if(btn == QMessageBox::No)
	{
		return;
	}

	int id = pModel->index(idx.row(), WIMMModel::COL_DbId, idx.parent()).data().toInt();

	if(!SqlTools::deleteMonthRecord(id))
	{
		QMessageBox::critical(this,
							  "Ошибка БД",
							  "При удалении записи из БД произошла ошибка. Обратитесь к администритору.",
							  QMessageBox::Close);
		return;
	}

	pModel->removeMonthById(id);
}

void MainWindow::on_pbEditCategories_clicked()
{
	CategoriesEditor ed;
	if(! ed.exec())
	{
		return;
	}

	///TODO: Перезагрузить модель.
}
