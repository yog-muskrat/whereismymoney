#ifndef ADDMONTHDIALOG_H
#define ADDMONTHDIALOG_H

#include <QDialog>

namespace Ui {
class AddMonthDialog;
}

class AddMonthDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AddMonthDialog(QWidget *parent = 0);
	~AddMonthDialog();

	int year() const ;
	int month() const;
	QString monthName() const;

private:
	Ui::AddMonthDialog *ui;
};

#endif // ADDMONTHDIALOG_H
