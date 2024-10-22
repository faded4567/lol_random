#include "FuncUI.h"


FuncUIGroupBox::FuncUIGroupBox(QWidget *p) : QGroupBox(p)
{

}

void FuncUIGroupBox::mousePressEvent(QMouseEvent *e)
{
    for (QWidget *child : this->parent()->findChildren<QWidget *>()) {
        // 检查子控件的名称是否匹配
        if (child->objectName() == this->objectName()) {
            emit SignalClickIndex(this->objectName().right(1).toInt() - 1);
            break;
        }
    }
}
