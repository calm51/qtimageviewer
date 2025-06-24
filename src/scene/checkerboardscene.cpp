#pragma execution_character_set("utf-8")

#include "checkerboardscene.h"

#include <cmath>
#include "../qtimageviewer.h"
#include <QPainter>

CheckerboardScene::CheckerboardScene(QObject *parent)
    : QGraphicsScene{parent} {}

void CheckerboardScene::removeElement(QGraphicsItem *element) {
    this->viewer->elements.removeAll(element);
    this->removeItem(element);
    delete element;
}

void CheckerboardScene::drawBackground(QPainter *painter, const QRectF &rect) {
    int gridSize = 20;

    int left = static_cast<int>(std::floor(rect.left() / gridSize)) * gridSize;
    int top = static_cast<int>(std::floor(rect.top() / gridSize)) * gridSize;

    for (int x = left; x < rect.right(); x += gridSize) {
        for (int y = top; y < rect.bottom(); y += gridSize) {
            bool even = ((x / gridSize) + (y / gridSize)) % 2 == 0;
            painter->fillRect(QRectF(x, y, gridSize, gridSize), even ? color1 : color2);
        }
    }
}
