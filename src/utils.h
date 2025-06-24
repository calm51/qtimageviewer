#pragma execution_character_set("utf-8")

#ifndef QTIMAGEVIEWER_UTILS_H
#define QTIMAGEVIEWER_UTILS_H

#include <QWidget>


template<typename T>
T *findParentOfType(QWidget *child) {
    QWidget *parent = child->parentWidget();
    while (parent) {
        if (T *result = qobject_cast<T *>(parent))
            return result;
        parent = parent->parentWidget();
    }
    return nullptr;
}


#endif // QTIMAGEVIEWER_UTILS_H
