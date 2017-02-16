#pragma once

#include "layeritem.h"
#include "formeditoritem.h"
#include "motionindicatorgraphicsitem.h"

#include <QPointer>

namespace QmlDesigner {

class FormEditorItem;
class MotionIndicatorGraphicsItem;

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
    QPointer<MotionIndicatorGraphicsItem> m_motionItem;
};
}
