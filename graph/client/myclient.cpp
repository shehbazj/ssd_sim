//myclient.cpp

#include "myclient.h"

MyClient::MyClient(QObject *parent) :
    QObject(parent)
{
    QThreadPool::globalInstance()->setMaxThreadCount(5);
}

void MyClient::SetSocket(int Descriptor)
{
    // make a new socket
    socket = new QTcpSocket(this);

    qDebug() << "A new socket created!";

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket->setSocketDescriptor(Descriptor);

    qDebug() << " Client connected at " << Descriptor;
}


// asynchronous - runs separately from the thread we created
void MyClient::connected()
{
    qDebug() << "Client connected event";
}

// asynchronous
void MyClient::disconnected()
{
    qDebug() << "Client disconnected";
}

// Our main thread of execution
// This happening via main thread
// Our code running in our thread not in Qthread
// That's why we're getting the thread from the pool

void MyClient::readyRead()
{
    qDebug() << "MyClient::readyRead()";
    qDebug() << socket->readAll();

    // Time consumer
    MyTask *mytask = new MyTask();
    mytask->setAutoDelete(true);
    connect(mytask, SIGNAL(Result(int)), this, SLOT(TaskResult(int)), Qt::QueuedConnection);

    qDebug() << "Starting a new task using a thread from the QThreadPool";
    QThreadPool::globalInstance()->start(mytask);

}

// After a task performed a time consuming task.
// We grab the result here, and send it to client
void MyClient::TaskResult(int Number)
{
    QByteArray Buffer;
    Buffer.append("\r\nTask result = ");
    Buffer.append(QString::number(Number));

    socket->write(Buffer);
}
