#include "motionindicatorgraphicsitem.h"

#include <nodelistproperty.h>
#include <formeditorscene.h>
#include <formeditorview.h>

#include <QPainter>
#include <QPainterPath>

#include <QGraphicsScene>
#include <QGraphicsView>

namespace QmlDesigner {

MotionIndicatorGraphicsItem::MotionIndicatorGraphicsItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
}

QRectF MotionIndicatorGraphicsItem::boundingRect() const
{
    return m_boundingRect;
}

void MotionIndicatorGraphicsItem::readMotionPoints()
{
//    ModelNode modelNode = m_formEditorItem->qmlItemNode().modelNode();
//    foreach (ModelNode child, modelNode.directSubModelNodes()) {
//        if (child.simplifiedTypeName().contains("Animation")) {

//        }
//    }
}

void MotionIndicatorGraphicsItem::addMotionPoint(QPointF point) {
    m_motionPoints.append(point);
}

void MotionIndicatorGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /* option */, QWidget * /* widget */)
{
    painter->save();
    QPen linePen(QColor(255,255,0,150));
    linePen.setCosmetic(true);
    linePen.setDashPattern(QVector<double>() << 3. << 2.);

    painter->setPen(linePen);

    if (!m_motionPoints.isEmpty()) {
    QList<QPointF> points = m_motionPoints;
    QPointF prevPoint = points.takeFirst();

    foreach (QPointF point, points) {
            painter->drawRect(QRect(prevPoint.x() - 5, prevPoint.y() - 5, 10, 10));
            painter->drawLine(prevPoint, point);
            prevPoint = point;
        }
    painter->drawRect(QRect(prevPoint.x() - 5, prevPoint.y() - 5, 10, 10));
    }

    painter->restore();

}


}
