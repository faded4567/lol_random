#include "mainwindow.h"
#include "ui_mainwindow.h"

#define LCU_FILE "./lcu.py"

#define RANDOM_NUM 3    // 随机次数

QTextCodec* g_gbkCodec = QTextCodec::codecForName("GBK");

QRandomGenerator* g_handle = nullptr;

QString g_talent[] = {  ("强攻"),  ("致命节奏"),  ("征服者"),  ("迅捷步伐"),  ("不灭之握"),  ("余震"),  ("守护者")
                        ,  ("电刑"),  ("黑暗收割"),  ("丛刃"),  ("艾黎"),  ("相位猛冲"),  ("奥术彗星"),  ("冰川增幅")
                        ,  ("先攻"),  ("冰川增幅"),  ("启封秘籍")};

QMap<int,QString> g_MainTalent = {
    {8000,"精密"},
    {8100,"主宰"},
    {8200,"巫术"},
    {8300,"启迪"},
    {8400,"坚决"}
};


int g_talent1[][4] = {
    {8005,8008,8021,8010},
    {9101,9111,8009,-1},
    {9104,9105,9103,-1},
    {8014,8017,8299,-1},
};
int g_talent2[][3] = {
    {8112,8128,9923},
    {8126,8139,8143},
    {8136,8120,8138},
    {8135,8105,8106},
};
int g_talent3[][3] = {
    {8214,8229,8230},
    {8224,8226,8275},
    {8210,8234,8233},
    {8237,8232,8236},
};
int g_talent4[][3] = {
    {8437,8439,8465},
    {8446,8463,8401},
    {8429,8444,8473},
    {8451,8453,8242},
    };
int g_talent5[][3] = {
    {8351,8360,8369},
    {8306,8304,8321},
    {8313,8352,8345},
    {8347,8410,8316},
    };
int g_talent6[][3] = {
    {5008,5005,5007},
    {5008,5010,5001},
    {5011,5013,5001},
    };


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitLCU();
    InitUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::get_random_number()
{
    // 获取随机数句柄
    if(g_handle == nullptr)
    {
        g_handle = new QRandomGenerator(QDateTime::currentMSecsSinceEpoch());
    }
    // 防止随机数出现负数的错误
    int rand_init = g_handle->generate();
    while(rand_init < 0)
        rand_init = g_handle->generate();
    return rand_init;
}

void MainWindow::handle_relase()
{
    if(g_handle != nullptr)
    {
        delete g_handle;
        g_handle = nullptr;
    }
}

void MainWindow::InitLCU()
{
    QProcess process;
    QStringList list;
    // 使用cmd命令查看启动参数
    list<<"/c"<<"wmic"<<"PROCESS"<<"WHERE"<<"name='LeagueClientUx.exe'"<<"GET"<<"commandline";
    process.start("C:\\Windows\\System32\\cmd.exe",list);
    process.setWorkingDirectory("C:/Windows/System32");
    process.setEnvironment(QStringList() << "PATH=C:/Windows/System32");
    process.waitForFinished();
    QByteArray tmp = process.readAllStandardOutput();
    QString str = QString::fromLocal8Bit(tmp);

    // 分割字符串
    QStringList outList = str.split(" ",Qt::SkipEmptyParts);
    // 查找进程ID token port
    for(int i=0;i<outList.size();i++)
    {
        int num = 0;
        // --remoting-auth-token
        if(outList.at(i).contains("--remoting-auth-token"))
        {
            m_token = outList.at(i).mid(23,22);
        }
        // --app-port
        else if(outList.at(i).contains("--app-port"))
        {
            num = outList.at(i).size() - 13;
            m_port = outList.at(i).mid(12,num);
        }
        // --app-pid
        else if(outList.at(i).contains("--app-pid"))
        {
            num = outList.at(i).size() - 12;
            QString pid = outList.at(i).mid(11,num);
            ui->label_4->setText(QString("初始化游戏进程：%1").arg(pid));
        }
    }
    qDebug()<<"port:"<<m_port<<"token:"<<m_token;

    // 获取当前账户拥有的英雄信息
    QString res = RunLcuCmd(QStringList()<<"lol-champions/v1/owned-champions-minimal"<<"0");
    // 解析全部英雄信息获取
    QJsonArray array = StrToJsondoc(res).array();
    for(int i=0;i<array.size();i++)
    {
        if(array[i].isObject())
        {
            QJsonObject obj = array[i].toObject();
            if(!obj["id"].isNull())
                m_myHeroIdList.append(obj["id"].toInt());
        }
    }
    ui->label->setText(QString("选择英雄(共%1个)").arg(m_myHeroIdList.size()));
}

void MainWindow::InitUI()
{
    setWindowTitle("LOL Random v1.1");

    setWindowIcon(QIcon("./icon.ico"));

    setAttribute(Qt::WA_StyledBackground,true);

    // 获取当前登录用户
    QString usrname = GetJsonStr(RunLcuCmd(QStringList()<<"lol-summoner/v1/current-summoner"<<"0"),"gameName").toString();
    ui->lineEdit_3->setText(usrname);

    m_randomNum = RANDOM_NUM;
    ui->pushButton->setText(QString("开始随机(剩余%1次)").arg(m_randomNum));

    ui->pushButton_2->setEnabled(false);

    m_boxList.append(ui->groupBox_1);
    m_boxList.append(ui->groupBox_2);
    m_boxList.append(ui->groupBox_3);

    connect(ui->groupBox_1,&FuncUIGroupBox::SignalClickIndex,this,&MainWindow::select_history);
    connect(ui->groupBox_2,&FuncUIGroupBox::SignalClickIndex,this,&MainWindow::select_history);
    connect(ui->groupBox_3,&FuncUIGroupBox::SignalClickIndex,this,&MainWindow::select_history);
}

QString MainWindow::RunLcuCmd(QStringList cmd)
{
    QString result;
    QProcess process;
    QStringList list;
    list<<"/c"<<"lcu.exe"<<m_token<<m_port<<cmd;
    process.start("cmd.exe",list);
    process.waitForFinished();
    result = g_gbkCodec->toUnicode(process.readAllStandardOutput());
//    qDebug()<<cmd<<"result:"<<result;
    return result;
}

QJsonValue MainWindow::GetJsonStr(QString src,QString key)
{
    qDebug()<<"json key:"<<key;
    QJsonDocument jsonDoc = StrToJsondoc(src);
    if(jsonDoc.isObject())
    {
        QJsonObject obj = jsonDoc.object();
        if(obj.contains(key))
        {
            QJsonValue val = obj[key];
            qDebug()<<key<<val;

            return val;
        }
    }
    return QJsonValue("");
}

QJsonDocument MainWindow::StrToJsondoc(QString src)
{
    QJsonParseError jsonError;
    src.replace("'","\"");
    src.replace("False","false");
    src.replace("True","true");
    src.replace("None","null");
    QJsonDocument jsonDoc = QJsonDocument::fromJson(src.toUtf8(),&jsonError);
    if(jsonDoc.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        qDebug()<<"json error:"<<jsonError.errorString();
        return QJsonDocument();
    }
    return jsonDoc;
}

int MainWindow::select_champ()
{
    if(m_selectNum >= m_selectInfoList.size() || m_selectNum < 0)
        return -1;
    // 获取队伍信息 /lol-champ-select/v1/session
    QJsonDocument teamDoc = StrToJsondoc(RunLcuCmd(QStringList()<<"lol-champ-select/v1/session"<<"0"));
    QJsonArray myteamArray = teamDoc.object()["myTeam"].toArray();
    QJsonArray actionArray = teamDoc.object()["actions"].toArray();
    QJsonValue summonerId = GetJsonStr(RunLcuCmd(QStringList()<<"lol-login/v1/session"<<"0"),"summonerId");

    QJsonValue cellId ,id ;
    for( int i=0;i<myteamArray.size();i++)
    {
        // 匹配summonerId
        if(myteamArray[i].toObject()["summonerId"] == summonerId)
        {
            cellId = myteamArray[i].toObject()["cellId"];
            break;
        }
    }
    // 根据匹配summonerId 查找 actions 中的 actorCellId
    QJsonObject myAct;
    if(actionArray.size() <= 0)
        return -1;
    for(int i=0;i<actionArray[0].toArray().size();i++)
    {
        // cellId
        if(actionArray[0].toArray()[i].toObject()["actorCellId"] == cellId)
        {
            id = actionArray[0].toArray()[i].toObject()["id"];
            myAct = actionArray[0].toArray()[i].toObject();
            myAct["championId"] = m_selectInfoList.at(m_selectNum).heroId;
            break;
        }
    }
    // 新的json存入文件
    QFile file("./champ.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return -1;
    file.write(QJsonDocument(myAct).toJson());
    file.close();
    // 发送选择英雄请求
    RunLcuCmd(QStringList()<<QString("lol-champ-select/v1/session/actions/%1").arg(id.toDouble())<<"3");
    return 0;
}

int MainWindow::select_talent()
{
    if(m_selectNum >= m_selectInfoList.size() || m_selectNum < 0)
        return -1;
    // 将选择的天赋页存入文件
    QFile perk("./perk.txt");
    if(!perk.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return -1;
    perk.write(QJsonDocument(m_selectInfoList.at(m_selectNum).talent).toJson());
    perk.close();

    // 应用天赋
    RunLcuCmd(QStringList()<<"lol-perks/v1/pages"<<"2");
    RunLcuCmd(QStringList()<<"lol-perks/v1/pages"<<"1");
    return 0;
}

void MainWindow::select_history(int num)
{
    m_selectNum = num;
    for(int i=0;i<m_boxList.size();i++)
    {
        if( i == num)
            m_boxList.at(i)->setStyleSheet("QGroupBox"
                                           "{"
                                           "border: 3px solid red;"
                                           "border-radius:8px;"
                                           "margin-top:6px;"
                                           "}"
                                           "QGroupBox:title"
                                           "{"
                                           "color:red;"
                                           "subcontrol-origin: margin;"
                                           "left: 10px;"
                                           "}");
        else
            m_boxList.at(i)->setStyleSheet("QGroupBox"
                                           "{"
                                           "border: 1px solid black;"
                                           "border-radius:8px;"
                                           "margin-top:6px;"
                                           "}"
                                           "QGroupBox:title"
                                           "{"
                                           "color:black;"
                                           "subcontrol-origin: margin;"
                                           "left: 10px;"
                                           "}");
    }
}
QWidget* MainWindow::findChildWidgetByName(QWidget *parent, const QString &name) {
    if (!parent) return nullptr;

    // 遍历父控件的所有子控件
    const auto children = parent->findChildren<QWidget*>();
    for (QWidget *child : children) {
        // 检查子控件的名称是否匹配
        if (child->objectName() == name) {
            return child; // 如果找到匹配的控件，返回它
        }

        // 递归地在子控件的子控件中查找
        QWidget *foundChild = findChildWidgetByName(child, name);
        if (foundChild) {
            return foundChild; // 如果在子控件的子控件中找到，返回它
        }
    }

    return nullptr; // 如果没有找到匹配的控件，返回 nullptr
}
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton_2->setEnabled(true);
    SelectInfo select;
    // 获取随机数
    int rand_hero = get_random_number();
    int infoNum = m_heroinfo.size();
    if(m_heroinfo.isEmpty())
    {
        // 读取信息数据
        QFile file("lolinfo.csv");
        if(!file.open(QIODevice::ReadOnly))
        {
            return ;
        }
        QTextStream stream(&file);
        QStringList list;
        // 跳过第一行
        stream.readLine();
        while(!stream.atEnd())
        {
            QString str = stream.readLine();
            list = str.split(",",Qt::SkipEmptyParts);
            if(list.size() > 3)
            {
                HeroInfo info;
                info.id = list.at(1).toInt();
                info.name = (list.at(2) + " " + list.at(3));
                m_heroinfo.insert(infoNum,info);
            }
            infoNum++;
        }
        file.close();
    }

    // 随机英雄
    int rand = rand_hero % infoNum;
    while(!m_myHeroIdList.contains(m_heroinfo.value(rand).id))
    {
        rand_hero = get_random_number();
        rand = rand_hero % infoNum;
    }
    qDebug()<<"init "<<rand_hero<<"current rand"<<rand;
    select.heroId = m_heroinfo.value(rand).id;
    ui->lineEdit->setText(m_heroinfo.value(rand).name);

    // 随机天赋 写入文件
    QFile perk("./perk.txt");
    if(!perk.open(QIODevice::ReadOnly))
        return ;
    QByteArray array = perk.readAll();
    perk.close();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(array,&jsonError);
    if(jsonDoc.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        qDebug()<<"json error "<<jsonError.errorString();
        return;
    }

    QJsonObject obj = jsonDoc.object();
    // 设置为当前选择的
    obj["current"] = true;
    // 天赋页名称
    obj["name"] = "随机天赋-" + m_heroinfo.value(rand).name;
    // 天赋随机 共5个天赋 取两个作为天赋
    int first = g_handle->bounded(0,4),second = g_handle->bounded(0,4);
    // 防止重复
    while(first == second)
        second = g_handle->bounded(0,4);
    obj["primaryStyleId"] = g_MainTalent.keys().at(first);
    obj["subStyleId"] = g_MainTalent.keys().at(second);
    ui->lineEdit_2->setText(g_MainTalent.values().at(first) + "+" +g_MainTalent.values().at(second));
    QJsonArray json;
    int rand_perk= get_random_number();
    // 主天赋
    switch(first)
    {
    case 0:
        json.append(QJsonValue(g_talent1[0][rand_perk % 4]));
        break;
    case 1:
        json.append(QJsonValue(g_talent2[0][rand_perk % 3]));
        break;
    case 2:
        json.append(QJsonValue(g_talent3[0][rand_perk % 3]));
        break;
    case 3:
        json.append(QJsonValue(g_talent5[0][rand_perk % 3]));
        break;
    case 4:
        json.append(QJsonValue(g_talent4[0][rand_perk % 3]));
        break;
    }

    for(int i=0;i<3;i++)
    {
        rand_perk= get_random_number();
        switch(first)
        {
        case 0:
            json.append(QJsonValue(g_talent1[i+1][rand_perk % 3]));
            break;
        case 1:
            json.append(QJsonValue(g_talent2[i+1][rand_perk % 3]));
            break;
        case 2:
            json.append(QJsonValue(g_talent3[i+1][rand_perk % 3]));
            break;
        case 3:
            json.append(QJsonValue(g_talent5[i+1][rand_perk % 3]));
            break;
        case 4:
            json.append(QJsonValue(g_talent4[i+1][rand_perk % 3]));
            break;
        }
    }
    // 副天赋
    int val1;
    for(int i=0;i<2;i++)
    {
        rand_perk= get_random_number();
        int tmp = g_handle->bounded(1,3);
        while(i == 1 && tmp == val1)
            tmp = g_handle->bounded(1,3);
        switch(second)
        {
        case 0:
            json.append(QJsonValue(g_talent1[tmp][rand_perk % 3]));
            break;
        case 1:
            json.append(QJsonValue(g_talent2[tmp][rand_perk % 3]));
            break;
        case 2:
            json.append(QJsonValue(g_talent3[tmp][rand_perk % 3]));
            break;
        case 3:
            json.append(QJsonValue(g_talent5[tmp][rand_perk % 3]));
            break;
        case 4:
            json.append(QJsonValue(g_talent4[tmp][rand_perk % 3]));
            break;
        }
        val1 = tmp;
    }
    // 属性天赋
    for(int i=0;i<3;i++)
    {
        rand_perk= get_random_number();
        json.append(QJsonValue(g_talent6[i][rand_perk % 3]));
    }
    obj["selectedPerkIds"] = json;
    select.talent = obj;

    // 随机装备

    // 更新界面
    int index = RANDOM_NUM - m_randomNum + 1;
    if(index == 1)
    {
        ui->label_5->setText(ui->lineEdit->text());
        ui->label_6->setText(ui->lineEdit_2->text());
    }
    else if(index == 2)
    {
        ui->label_7->setText(ui->lineEdit->text());
        ui->label_8->setText(ui->lineEdit_2->text());
    }
    else if(index == 3)
    {
        ui->label_9->setText(ui->lineEdit->text());
        ui->label_10->setText(ui->lineEdit_2->text());
    }

    select_history(RANDOM_NUM - m_randomNum);

    // 随机次数减少
    m_randomNum--;
    ui->pushButton->setText(QString("开始随机(剩余%1次)").arg(m_randomNum));
    if(m_randomNum == 0)
    {
        QMessageBox::information(this, "Infomation", "本局对战中随机次数已用完..");
        ui->pushButton->setEnabled(false);
    }

    // 存储每次随机的信息
    m_selectInfoList.append(select);

    // 资源释放
    handle_relase();

}

void MainWindow::on_pushButton_2_clicked()
{
    // 选择英雄
    int ret = select_champ();
    if(ret)
    {
        QMessageBox::information(this, "Infomation", "应用英雄失败.");
        goto INIT_;
    }

    // 天赋应用
    ret = select_talent();
    if(ret)
    {
        QMessageBox::information(this, "Infomation", "应用天赋失败.");
        goto INIT_;
    }


INIT_:
    // 恢复随机次数
    m_randomNum = RANDOM_NUM;
    ui->pushButton->setText(QString("开始随机(剩余%1次)").arg(m_randomNum));
    ui->pushButton->setEnabled(true);

    // 清空
    m_selectInfoList.clear();
    // 清空历史随机
    ui->label_5->setText("");
    ui->label_6->setText("");

    ui->label_7->setText("");
    ui->label_8->setText("");

    ui->label_9->setText("");
    ui->label_10->setText("");

    m_selectNum = -1;
}


