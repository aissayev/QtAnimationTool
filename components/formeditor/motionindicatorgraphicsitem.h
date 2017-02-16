#pragma once

#include <QGraphicsObject>

namespace QmlDesigner {

class FormEditorItem;

class MotionIndicatorGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit MotionIndicatorGraphicsItem(QGraphicsItem *parent = 0);

//    void setFormEditorItem(FormEditorItem *formEditorItem);
//    FormEditorItem *formEditorItem() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    void readMotionPoints();
    void addMotionPoint(QPointF point);
private:
    QList<QPointF> m_motionPoints;
    QRectF m_boundingRect;
    FormEditorItem *m_formEditorItem;
};
}
