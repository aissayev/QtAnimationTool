#pragma once

#include <QGraphicsObject>

namespace QmlDesigner {

class FormEditorItem;

class MotionIndicatorGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
public:
    MotionIndicatorGraphicsItem(QGraphicsItem *parent);

//    void setFormEditorItem(FormEditorItem *formEditorItem);
//    FormEditorItem *formEditorItem() const;
    void readMotionPoints();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QList<QPointF> m_motionPoints;
    FormEditorItem *m_formEditorItem;
};
}
