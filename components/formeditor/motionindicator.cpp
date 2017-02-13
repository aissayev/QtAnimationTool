#include "motionindicator.h"

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
}

void MotionIndicator::hide()
{
}

void MotionIndicator::clear()
{
}

void MotionIndicator::setItems(const QList<FormEditorItem *> &itemList)
{
 clear();
}

void MotionIndicator::updateItems(const QList<FormEditorItem *> &itemList)
{
}
}
