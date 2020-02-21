#include "digitalclock.hpp"

DigitalClock::DigitalClock(QWidget *parent)
    : QWidget(parent)
{
	this->label = new QLabel(this);

    QTimer *timer2 = new QTimer(this);
    QObject::connect(timer2, SIGNAL(timeout()), this, SLOT(showTimee()));
    timer2->start(1000);
}

void DigitalClock::showTimee(){
        QTime time = QTime::currentTime();
        QString text = time.toString("ss");
        qDebug() << text;

	label->clear();
	/*
	QSize q = label-> minimumSizeHint();
	if(!q.isNull() && q.isValid()) {
		qDebug() << q.height() << q.width();
	}
	q.setHeight(q.height() * 2);
	q.setWidth(q.width() * 2);
	*/
	label->setAlignment(Qt::AlignCenter);
	label->setIndent(20);
	label->setText(text);
}
