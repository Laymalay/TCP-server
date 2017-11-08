#pragma once
#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTime>
#include <QTcpSocket>
#include <QWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
class QTcpServer;
class QTextEdit;
class QTcpSocket;

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(int nPort,QWidget *parent = 0);
    ~Server();
public slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void slotSendToClient();
private:
    QTcpServer* m_ptcpServer;
    QTextEdit* m_ptxt;
    QLineEdit* lineEdit;
    QTcpSocket* pClientSocket;
    quint16 m_nNextBlockSize;
    Ui::Server *ui;
private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

};

#endif // SERVER_H
