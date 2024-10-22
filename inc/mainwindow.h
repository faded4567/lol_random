#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QTimer>
#include <QTime>
#include <QRandomGenerator>
#include <QProcess>
#include <windows.h>
#include <shellapi.h>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextCodec>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QIcon>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLayout>
#include "FuncUI.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
struct HeroInfo{
    int id;
    QString name;
};

struct SelectInfo {
    int heroId;             // 英雄ID
    QJsonObject talent;     // 天赋
    SelectInfo(){
        heroId = -1;
        talent = QJsonObject();
    }
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int get_random_number();
    void handle_relase();
    void InitLCU();
    void InitUI();
    QString RunLcuCmd(QStringList cmd );
    QJsonValue GetJsonStr(QString src, QString key);
    QJsonDocument StrToJsondoc(QString src);

    int select_champ();
    int select_talent();

    QWidget* findChildWidgetByName(QWidget *parent, const QString &name);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void select_history(int num);

private:
    Ui::MainWindow *ui;
    QMap<int,HeroInfo> m_heroinfo;
    QString m_port;     // LCU访问端口号
    QString m_token;    // LCU所用token
    int m_heroId;
    QList<int> m_myHeroIdList;
    int m_randomNum;
    QList<SelectInfo> m_selectInfoList;
    QList<QGroupBox*> m_boxList;
    int m_selectNum;
};
#endif // MAINWINDOW_H
