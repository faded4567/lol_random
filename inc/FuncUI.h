#ifndef FUNCUI_H
#define FUNCUI_H

#include <QGroupBox>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
class FuncUIGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    FuncUIGroupBox(QWidget *p);

    void mousePressEvent(QMouseEvent *e) override;
signals:
    void SignalClickIndex(int);
};

#endif // FUNCUI_H
