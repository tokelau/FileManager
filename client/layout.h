#ifndef LAYOUT_H
#define LAYOUT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QListView>
#include <QStringListModel>
#include <QMenu>
#include <QFile>
#include <QDir>
#include <QContextMenuEvent>
#include <QDebug>
#include <QInputDialog>

class Layout : public QWidget
{
    Q_OBJECT
private:
    QLabel* info;
    QVBoxLayout* layout;
    QListView* listView;
    QString selected;
    QPushButton* crFolder;
    QPushButton* crFile;

public:
    QString title;
    QTextEdit* te;
    Layout(QVBoxLayout* _layout, QWidget* widget = 0) : QWidget(widget) {
        layout = _layout;
        info = new QLabel("<center>Insert password: </center>");
    }
    void setStartLayout(QLineEdit* pass, QTextEdit* m_ptxtInfo, QPushButton* pcmd);
    void removeStartLayout(QLineEdit* pass, QTextEdit* m_ptxtInfo, QPushButton* pcmd);
    QListView* setListLayout(QList<QVariant>* names, QString path,
                             QPushButton* _crFolder, QPushButton* _crFile);
    void removeListLayout();
    void setStyle(QWidget* widget) {
        QFile file(QDir::currentPath() + "\\style\\style.qss");
        file.open(QFile::ReadOnly);
        QString strCSS = file.readAll();
        widget->setStyleSheet(strCSS);
    }
    void setFileLayout(QString name, QString content, QPushButton* send) {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout;
        l->addWidget(new QLabel("<H1>" + name + "</H1>"));
        te = new QTextEdit;
        te->setText(content);
        l->addWidget(te);
        te->setMinimumHeight(480);
        te->setMinimumWidth(600);
        te->moveCursor(QTextCursor::End);
        title = name;

        l->addWidget(send);
//        l->addWidget(close);
        w->setLayout(l);
        w->show();
    }
private slots:
    void slotInputDialog() {}
};

#endif // LAYOUT_H
