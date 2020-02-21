// myserver.cpp

#include "myserver.h"

MyServer::MyServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, 1234))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

void MyServer::newConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();

	const int Timeout = 5* 1000;
	char data[20];
	socket->waitForReadyRead(Timeout);
	socket->read(data, 19);
	qDebug() << data;
//    socket->write(data);
 //   socket->flush();

  //  socket->waitForBytesWritten(3000);

    socket->close();
}
