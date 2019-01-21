#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QMessageBox>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QDebug>
#include <QHash>
#include <QJsonObject>

#include "filemanager.h"


//сделай обязательно деструктор
class Server : public QWidget
{
    Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit* m_ptxt;
    quint16 m_nNextBlockSize;
    QString pass;

    FileManager* fm;

    QHash<QString, QVariant> mToClient;
    QHash<QString, QVariant> mFromClient;
private:
    void sendToClient(QTcpSocket* pSocket);
protected:
    virtual void closeEvent(QCloseEvent *ev) {
        delete m_ptcpServer;
        delete m_ptxt;
        fm->~FileManager();
    }
public:
    Server(quint16 nPort, QString pass, QWidget* pwt = 0);
public slots:
    virtual void slotNewConnection();
    void slotReadClient();
};

#endif // SERVER_H
