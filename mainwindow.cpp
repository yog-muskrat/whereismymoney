#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "tools.h"
#include "structs.h"
#include "sqltools.h"
#include "wimmmodel.h"
#include "addmonthdialog.h"
#include "wimmfiltermodel.h"
#include "categorieseditor.h"

#include <QMenu>
#include <QDate>
#include <QDebug>
#include <QTimer>
#include <QLocale>
#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>
#include <QFontDialog>

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

	pFilterModel = new WIMMFilterModel(this);
	pFilterModel->setSourceModel(pModel);
	pFilterModel->setDynamicSortFilter(true);

	ui->listView->setModel(pModel);
	ui->listView->setModelColumn(WIMMModel::COL_Title);
	ui->listView->setSelectionBehavior( QListView::SelectRows );
	ui->listView->setSelectionMode( QListView::ExtendedSelection );

	ui->treeView->setModel(pFilterModel);
	ui->treeView->setColumnHidden(WIMMModel::COL_DbId, true);
	ui->treeView->resizeColumnToContents(WIMMModel::COL_Title);
	ui->treeView->setContextMenuPolicy( Qt::CustomContextMenu );

	QPalette p = ui->lblIncome->palette();
	p.setColor( QPalette::Foreground, QColor(0, 84, 0));
	ui->lblIncome->setPalette(p);

	p.setColor( QPalette::Foreground, QColor(84, 0, 0));
	ui->lblOutcome->setPalette(p);

	p.setColor( QPalette::Foreground, QColor(0, 0, 84));
	ui->lblEst->setPalette(p);

	calcTotals();

	createMenu();

	connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged()));
	connect(pFilterModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(calcTotals()));
	connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onMenuRequested(QPoint)));

	int year = QDate::currentDate().year();
	int month = QDate::currentDate().month();

	QModelIndex idx = pModel->monthIndex(year, month);

	if(idx.isValid())
	{
		ui->listView->setCurrentIndex(idx);
	}
}

MainWindow::~MainWindow()
{
	SqlTools::closeDb();
	delete ui;
}

void MainWindow::onSelectionChanged()
{

	QModelIndexList idxs = ui->listView->selectionModel()->selectedRows(WIMMModel::COL_Title);

	QList<int> filter;

	foreach(const QModelIndex &idx, idxs)
	{
		filter << pModel->monthId(idx);
	}

	pFilterModel->setMonthsFilter(filter);
	ui->treeView->expandAll();
	for(int i = 0; i < ui->treeView->model()->columnCount(); ++i)
	{
		if(ui->treeView->isColumnHidden(i))
		{
			continue;
		}

		ui->treeView->resizeColumnToContents(i);
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

	pModel->removeMonth(id);
}

void MainWindow::calcTotals()
{
	double income = 0;
	double outcome = 0;

	ui->summaryTree->clear();
	ui->summaryTree->setColumnCount(4);
	ui->summaryTree->setHeaderLabels( QStringList() << "Категория" << QChar(0x002b) << QChar(0x2212) << "=");
	QFont f = qApp->font();
	f.setBold(true);
	ui->summaryTree->headerItem()->setFont(0, f);
	ui->summaryTree->headerItem()->setFont(1, f);
	ui->summaryTree->headerItem()->setFont(2, f);
	ui->summaryTree->headerItem()->setFont(3, f);
	ui->summaryTree->headerItem()->setTextAlignment(0, Qt::AlignCenter);
	ui->summaryTree->headerItem()->setTextAlignment(1, Qt::AlignCenter);
	ui->summaryTree->headerItem()->setTextAlignment(2, Qt::AlignCenter);
	ui->summaryTree->headerItem()->setTextAlignment(3, Qt::AlignCenter);

	QList<GroupItem*> items = SqlTools::loadSummary();

	foreach(GroupItem* group, items)
	{
		QTreeWidgetItem *groupTreeItem = new QTreeWidgetItem(ui->summaryTree);
		groupTreeItem->setText(0, group->name());

		double groupIncome = group->value(WIMMItem::FirstIn) + group->value(WIMMItem::SecondIn);
		double groupOutcome = group->value(WIMMItem::FirstOut) + group->value(WIMMItem::SecondOut);
		double groupEst= groupIncome - groupOutcome;

		income += groupIncome;
		outcome += groupOutcome;

		groupTreeItem->setText(1, Tools::moneyString( groupIncome));
		groupTreeItem->setText(2, Tools::moneyString( groupOutcome));
		groupTreeItem->setText(3, Tools::moneyString( groupEst));

		groupTreeItem->setBackgroundColor(1, QColor(200, 255, 200));
		groupTreeItem->setBackgroundColor(2, QColor(255, 200, 200));
		groupTreeItem->setBackgroundColor(3, QColor(220, 250, 255));

		QFont f = groupTreeItem->font(0);
		f.setBold(true);
		groupTreeItem->setFont(0, f);
		groupTreeItem->setFont(1, f);
		groupTreeItem->setFont(2, f);
		groupTreeItem->setFont(3, f);

		foreach(CategoryItem* category, group->categories())
		{
			QTreeWidgetItem *categoryTreeItem = new QTreeWidgetItem(groupTreeItem);
			categoryTreeItem->setText(0, category->name());

			double categoryIncome = category->value(WIMMItem::FirstIn) + category->value(WIMMItem::SecondIn);
			double categoryOutcome = category->value(WIMMItem::FirstOut) + category->value(WIMMItem::SecondOut);
			double categoryEst = categoryIncome - categoryOutcome;

			categoryTreeItem->setText(1, Tools::moneyString( categoryIncome ));
			categoryTreeItem->setText(2, Tools::moneyString( categoryOutcome ));
			categoryTreeItem->setText(3, Tools::moneyString( categoryEst ));

			categoryTreeItem->setBackgroundColor(1, QColor(200, 255, 200));
			categoryTreeItem->setBackgroundColor(2, QColor(255, 200, 200));
			categoryTreeItem->setBackgroundColor(3, QColor(220, 250, 255));
		}
	}

	double left = income - outcome;

	ui->lblIncome->setText(QChar(0x002b)+Tools::moneyString(income));
	ui->lblOutcome->setText(QChar(0x2212)+Tools::moneyString(outcome));
	ui->lblEst->setText(Tools::moneyString(left));

	ui->summaryTree->expandAll();
	ui->summaryTree->resizeColumnToContents(0);
	ui->summaryTree->resizeColumnToContents(1);
	ui->summaryTree->resizeColumnToContents(2);

	qDeleteAll(items);
	items.clear();
}

void MainWindow::onMenuRequested(const QPoint &p)
{
	QModelIndex idx = ui->treeView->indexAt(p);


	if(!idx.isValid())
	{
		return;
	}

	QModelIndex parentIndex = pFilterModel->mapToSource(idx);

	if(parentIndex.isValid() && pModel->hasComment(parentIndex))
	{
		ui->treeView->setCurrentIndex(idx);
		pMenu->exec( ui->treeView->mapToGlobal(p) );
	}
}

void MainWindow::onEditComment()
{
	QModelIndex idx = pFilterModel->mapToSource( ui->treeView->currentIndex() );

	Q_ASSERT(pModel->hasComment(idx));

	QString oldComment = pModel->comment(idx);

	bool ok = false;
	QString newComment = QInputDialog::getText(this, "Комментарий", "Укажите комментарий", QLineEdit::Normal, oldComment, &ok);

	if(!ok || newComment == oldComment)
	{
		return;
	}

	pModel->setComment(idx, newComment);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	QSettings set("mudbay", "wimm");
	set.setValue("geometry", saveGeometry());
	set.setValue("splitter", ui->splitter->saveState());
	set.setValue("summary", ui->pbSummary->isChecked());
	set.setValue("font", qApp->font());

	QMainWindow::closeEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
	QSettings set("mudbay", "wimm");
	restoreGeometry( set.value("geometry").toByteArray());
	ui->splitter->restoreState(set.value("splitter").toByteArray());
	ui->pbSummary->setChecked( set.value("summary", true).toBool());

	QFont f = set.value("font", qApp->font()).value<QFont>();
	qApp->setFont(f);

	QMainWindow::showEvent(e);

}

void MainWindow::createMenu()
{
	pMenu = new QMenu(this);
	pMenu->addAction(QIcon(":comment"), "Комментарий", this, SLOT(onEditComment()));
}

void MainWindow::on_action_categories_triggered()
{
	CategoriesEditor ed;
	if(! ed.exec())
	{
		return;
	}

	QModelIndex idx = ui->listView->currentIndex();

	int id = pModel->index(idx.row(), WIMMModel::COL_DbId, idx.parent()).data().toInt();

	pModel->clear();
	pModel->addMonths( SqlTools::loadMonths() );

	if(id > 0)
	{
		QModelIndex newIndex = pModel->monthIndex( id);
		if(!newIndex.isValid())
		{
			return;
		}
		ui->listView->setCurrentIndex( newIndex );
	}
}

void MainWindow::on_action_fonts_triggered()
{
	QFont f = qApp->font();
	bool ok = false;
	QFont newFont = QFontDialog::getFont(&ok, f, this, "Настройка шрифта");

	if(!ok)
	{
		return;
	}

	qApp->setFont(newFont);
}
