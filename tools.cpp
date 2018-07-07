#include "tools.h"

#include <QLocale>

QString Tools::capitalize(const QString &string)
{
	if(string.isEmpty())
	{
		return string;
	}

	QString result = string.simplified();

	result[0] = result[0].toUpper();

	return result;
}

QString Tools::moneyString(double amount)
{
	return QLocale::system().toCurrencyString(amount, /*QChar(0x20bd)*/ " ");
}
