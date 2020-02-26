#ifndef DIGITALCLOCK_H
#define DIGITALCLOCK_H

#include <QWidget>
#include <QDebug>
#include <QtWidgets>
#include <QObject>
#include <QSizePolicy>

class DigitalClock : public QWidget
{
	Q_OBJECT

public:
    DigitalClock(QWidget *parent = 0);
public slots:
    void showTimee();
private:
	QLabel *label;
};
#endif // DIGITALCLOCK_H
