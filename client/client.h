#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QDebug>
#include <QHash>
#include <QJsonObject>
#include <QFile>
#include <QLatin1String>
#include <QListView>
#include <QItemSelectionModel>
#include <QContextMenuEvent>
#include <QCloseEvent>
#include <QSignalMapper>

#include "layout.h"
#include "fileeditor.h"

class Client : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    quint16 m_nNextBlockSize;
    QLabel* info;
    QLineEdit* pass;
    QPushButton* pcmd;
    QItemSelectionModel* itemSelectionModel;
    QStringListModel* model;
    QMenu* menu;
    QListView* listView;
    QVBoxLayout* layout;
    Layout* layoutHandler;
    QList<QVariant>* names;


    QHash<QString, QVariant> mToServer;
    QHash<QString, QVariant> mFromServer;
public:
    Client(const QString& strHost, quint16 nPort, QWidget* pwt = 0);
protected:
    virtual void contextMenuEvent(QContextMenuEvent* ре) {
        itemSelectionModel = listView->selectionModel();
        if(itemSelectionModel) {
            menu->exec(ре->globalPos());
        }
    }
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError err);
    void slotSendToServer();
    void slotConnected();
    void slotCheckAuth();
    void slotActivated(QAction* pAction) {
//        qDebug() << itemSelectionModel->currentIndex().row();
//        qDebug() << names->takeAt(itemSelectionModel->currentIndex().row());
        QString action = pAction->text().remove("&");

        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);

        QVariant name = itemSelectionModel->model()->data(itemSelectionModel->currentIndex());
        if (action == "Удалить" && name.toString() != "..") {
            mToServer.insert("action", "remove");
            mToServer.insert("target", name);

        } else if (action == "Перейти") {
            mToServer.insert("action", "goover");
            mToServer.insert("target", name);
        }
        mToServer.insert("time", QTime::currentTime());

        out << quint16(0) << mToServer;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));

        m_pTcpSocket->write(arrBlock);
//        QString strColor = pAction->text().remove("&");
//        this->setStyleSheet(QString("background: %1").arg(strColor));
    }
public slots:
    void slotSaveContent() {
//        fileContent = te->toPlainText();
        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);

        mToServer.insert("action", "save");
        mToServer.insert("title", layoutHandler->title);
        mToServer.insert("filecontent", layoutHandler->te->toPlainText());

        mToServer.insert("time", QTime::currentTime());

        out << quint16(0) << mToServer;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));

        m_pTcpSocket->write(arrBlock);
//        qDebug() << layoutHandler->te->toPlainText();
    }
    void slotCreateFolder() {
        bool ok;
        layoutHandler->title = QInputDialog::getText(this, tr("Название папки"),
                                tr("Введите название:"), QLineEdit::Normal,
                                "Новая папка", &ok);

        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);

        mToServer.insert("action", "create");
        mToServer.insert("folder", layoutHandler->title);

        mToServer.insert("time", QTime::currentTime());

        out << quint16(0) << mToServer;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));

        m_pTcpSocket->write(arrBlock);
        mToServer.clear();
//        qDebug() << layoutHandler->te->toPlainText();
    }
    void slotCreateFile() {
        bool ok;
        layoutHandler->title = QInputDialog::getText(this, tr("Название файла"),
                                tr("Введите название (без расширения):"), QLineEdit::Normal,
                                "Новый документ", &ok);

        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);

        mToServer.insert("action", "create");
        mToServer.insert("file", layoutHandler->title);

        mToServer.insert("time", QTime::currentTime());

        out << quint16(0) << mToServer;

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));

        m_pTcpSocket->write(arrBlock);
        mToServer.clear();
//        qDebug() << layoutHandler->te->toPlainText();
    }
};

#endif // CLIENT_H
