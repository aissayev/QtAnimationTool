#include "motionindicator.h"
#include "motionindicatorgraphicsitem.h"

#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QDebug>

#include "bindingproperty.h"
#include "variantproperty.h"

namespace QmlDesigner {

MotionIndicator::MotionIndicator(LayerItem *layerItem)
    : m_layerItem(layerItem)
{
}

MotionIndicator::MotionIndicator()
{
}

MotionIndicator::~MotionIndicator()
{
    clear();
}

void MotionIndicator::show()
{
    if (m_motionItem) {
        m_motionItem->show();
    }
}

void MotionIndicator::hide()
{
    if (m_motionItem) {
        m_motionItem->hide();
    }
}

void MotionIndicator::clear()
{
    delete m_motionItem;
}

void MotionIndicator::setItems(const QList<FormEditorItem *> &itemList)
{
    clear();
    QPointF prev;

    if (itemList.count() == 1) {
        FormEditorItem *first = itemList.first();
        ModelNode node = first->qmlItemNode().modelNode();
        qDebug() << "node:" << node.id();
        m_motionItem = new MotionIndicatorGraphicsItem(m_layerItem.data());
        prev = first->center();
        m_motionItem->addMotionPoint(prev);
        qDebug() << "prev:" << prev;

        foreach(ModelNode child, node.directSubModelNodes()) {
            if(child.isSubclassOf("QtQuick.PropertyAnimation")) {
                BindingProperty property = child.bindingProperty("property");
                float change = child.variantProperty("to").value().toFloat();
                if (property.isValid() && change) {
                    if (property.expression() == "x") {
                        QPointF newPoint = QPointF(prev.x() + change, prev.y());
                        m_motionItem->addMotionPoint(newPoint);
                        prev = newPoint;
                        qDebug() << "prev:" << prev;

                    }
                    else if (property.expression() == "y") {
                        QPointF newPoint = QPointF(prev.x(), prev.y() + change);
                        m_motionItem->addMotionPoint(newPoint);
                        prev = newPoint;
                        qDebug() << "prev:" << prev;

                    }
                }
            }
        }
    }
}

void MotionIndicator::updateItems(const QList<FormEditorItem *> &itemList)
{
    setItems(itemList);
}
}
