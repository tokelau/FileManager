#include "server.h"

Server::Server(quint16 nPort, QString pass, QWidget* pwt /*= 0*/) : QWidget (pwt), m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:"
                              + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    this->pass = pass;

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);

    d = new DirHandler;
    if (d->init()) {
        m_ptxt->append("Server: Root directory successfully created.");
    } else {
        m_ptxt->append("Server: Wrong. Can't create root directory. Check admin rights.");
    }
//    qDebug() << d->getSubDirs();
//    qDebug() << d->getSubDirs();
//    d->makeDir("empty");
//    if (!d->rmDir("rootempty")) {
//        m_ptxt->append("Server: Wrong. Folder does not exists.");
//    }
}

void Server::slotNewConnection() {
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    mToClient.insert("info", "Server Response: Connected!");

    sendToClient(pClientSocket);
}

void Server::slotReadClient() {
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);

    in.setVersion(QDataStream::Qt_5_11);
    for(;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if(pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        in >> mFromClient;

        if (mFromClient["auth"].toString() != pass) {
            mToClient.insert("auth", -2);
        } else {
            mToClient.insert("auth", 1);
            mToClient.insert("subfolders", d->getSubDirs());
            mToClient.insert("path", d->getDir());
        }
        if (!mFromClient["message"].toString().isEmpty()) {
            mToClient.insert("message", mFromClient["message"].toString());
        }

        QString strMessage;

        if (!mFromClient["auth"].toString().isEmpty()) {
            strMessage = mFromClient["time"].toString() + " Recieved password: " + mFromClient["auth"].toString();
        }
        if (!mFromClient["message"].toString().isEmpty()) {
            strMessage = mFromClient["time"].toString() + " Recieved password: " + mFromClient["message"].toString();
        }
        if (!mFromClient["action"].toString().isEmpty()) {
            if (mFromClient["action"].toString() == "remove") {
                d->rm(mFromClient["target"].toString());
                mToClient.insert("subfolders", d->getSubDirs());
                mToClient.insert("path", d->getDir());
            }
            if (mFromClient["action"].toString() == "goover") {
                QString target = mFromClient["target"].toString();
//                qDebug() << target;
                if (d->getType(target) == NULL) {
                    mToClient.insert("message", "Path does not exists.");
                }
                if (d->getType(target) == "folder") {
                    d->goover(mFromClient["target"].toString());
                    mToClient.insert("subfolders", d->getSubDirs());
                    mToClient.insert("path", d->getDir());
                }
                if (d->getType(target) == "file") {
                    mToClient.insert("title", target);
                    mToClient.insert("filecontent", d->getFileContent(target));
                }
            }
            if (mFromClient["action"].toString() == "save") {
                d->saveFile(mFromClient["title"].toString(), mFromClient["filecontent"].toString());
                mToClient.insert("message", "Successfully done.");
//                qDebug() << mFromClient["filecontent"].toString();
            }
            if (mFromClient["action"].toString() == "create") {
                if (!mFromClient["folder"].toString().isEmpty()) {
                    bool res = d->makeDir(mFromClient["folder"].toString());
                    QString message = res ? "Successfully create folder." : "Problem: "
                                                                            "The folder already exists or another.";
                    mToClient.insert("path", d->getDir());
                }
                if (!mFromClient["file"].toString().isEmpty()) {
                    bool res = d->makeFile(mFromClient["file"].toString() + ".txt");
                    QString message = res ? "Successfully create file." : "Problem: "
                                                                            "The file already exists or another.";
                }
                mToClient.insert("subfolders", d->getSubDirs());
//                qDebug() << "folder:" <<mFromClient["folder"].toString();
//                qDebug() << "file" << mFromClient["file"].toString() + ".txt";
            }
            strMessage = mFromClient["time"].toString() + " Recieved action: " + mFromClient["action"].toString();

        }
        m_ptxt->append(strMessage);

        m_nNextBlockSize = 0;

        sendToClient(pClientSocket);
        mToClient.clear();
//        sendToClient(pClientSocket, "Server response: Welcome!");
    }
}

void Server::sendToClient(QTcpSocket* pSocket) {
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);

    mToClient.insert("time", QTime::currentTime());

    out << quint16(0) << mToClient;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}
