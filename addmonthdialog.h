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
	explicit AddMonthDialog(QWidget *parent = nullptr);
	~AddMonthDialog() override;

	int year() const ;
	int month() const;
	QString monthName() const;
	bool copyPreviousEst() const;

private:
	Ui::AddMonthDialog *ui{nullptr};
};

#endif // ADDMONTHDIALOG_H
