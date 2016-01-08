#include "addmonthdialog.h"
#include "ui_addmonthdialog.h"
#include "tools.h"

#include <QDate>

AddMonthDialog::AddMonthDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddMonthDialog)
{
	ui->setupUi(this);

	ui->sbYear->setValue( QDate::currentDate().year());

	for(int i = 1; i <= 12; ++i)
	{
		QString monthName = QDate::longMonthName(i, QDate::StandaloneFormat);
		monthName = Tools::capitalize(monthName);
		ui->cbMonth->addItem( monthName, i);
	}
}

AddMonthDialog::~AddMonthDialog()
{
	delete ui;
}

int AddMonthDialog::year() const
{
	return ui->sbYear->value();
}

int AddMonthDialog::month() const
{
	return ui->cbMonth->itemData( ui->cbMonth->currentIndex() ).toInt();
}

QString AddMonthDialog::monthName() const
{
	return ui->cbMonth->currentText();
}
