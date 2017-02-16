#include "motionindicator.h"
#include "motionindicatorgraphicsitem.h"

#include <QGraphicsScene>
#include <QGraphicsPathItem>

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
    if (itemList.count() == 1) {
        m_motionItem = new MotionIndicatorGraphicsItem(m_layerItem.data());
    }
}

void MotionIndicator::updateItems(const QList<FormEditorItem *> &itemList)
{
}
}
