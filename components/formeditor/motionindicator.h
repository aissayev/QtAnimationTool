#pragma once

#include "layeritem.h"
#include "formeditoritem.h"

#include <QPointer>

namespace QmlDesigner {
class MotionIndicator
{
public:
    MotionIndicator(LayerItem *layerItem);
    MotionIndicator();
    ~MotionIndicator();

    void show();
    void hide();

    void clear();

    void setItems(const QList<FormEditorItem *> &itemList);
    void updateItems(const QList<FormEditorItem *> &itemList);
private:
    QPointer<LayerItem> m_layerItem;
};
}
