#include "server.h"
#include "ui_server.h"

Server::Server(int nPort, QWidget *parent) :
    QWidget(parent),
    m_nNextBlockSize(0),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    m_ptcpServer = new QTcpServer(this);
    if(!m_ptcpServer->listen(QHostAddress::Any, nPort)){
        QMessageBox::critical(0,"server error","unable to start the server:"+m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
    connect(ui->pushButton,SIGNAL(clicked()),this, SLOT(slotSendToClient()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(slotSendToClient()));


    ui->chat->setReadOnly(true);
}

Server::~Server()
{
    delete ui;
}

void Server::slotNewConnection() {
    pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
            );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
            );
    sendToClient(pClientSocket, "Server Response: Connected!");
    ui->chat->append(pClientSocket->peerName());
}



void Server::slotReadClient()
{
    pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        QTime time;
        QString str;
        in >> time >> str;

        QString strMessage = time.toString() + " " + "Client: " + str;
        ui->chat->append(strMessage);

        m_nNextBlockSize = 0;

//        sendToClient(pClientSocket, "Server Response: Received \"" + str + "\"");
    }
}

void Server::slotSendToClient()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);
    //TODO whyyy version
    out<<quint16(0)<<QTime::currentTime()<<ui->lineEdit->text();
    out.device()->seek(0);
    out << quint16(arrBlock.size()- sizeof(quint16));
    pClientSocket->write(arrBlock);
    ui->chat->append(QTime::currentTime().toString()+" "+ui->lineEdit->text());
    ui->lineEdit->clear();
}

void Server::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}
