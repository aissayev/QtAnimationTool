/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "timelineview.h"
#include "timelinewidget.h"
#include "qmldesignerconstants.h"
#include "qmldesignericons.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>

#include <utils/icon.h>
#include <utils/utilsicons.h>

#include <bindingproperty.h>
#include <designmodecontext.h>
#include <nodeproperty.h>
#include <nodelistproperty.h>
#include <variantproperty.h>
#include <QHeaderView>
#include <qmlitemnode.h>

static inline void setScenePos(const QmlDesigner::ModelNode &modelNode,const QPointF &pos)
{

}

namespace QmlDesigner {

TimelineView::TimelineView(QObject* parent) :
        AbstractView(parent),
        m_blockSelectionChangedSignal(false),
        m_widget(new TimelineWidget(this))
{
    Internal::TimelineContext *timelineContext = new Internal::TimelineContext(m_widget.data());
    Core::ICore::addContextObject(timelineContext);

}

TimelineView::~TimelineView()
{
    if (m_widget && !m_widget->parent())
        delete m_widget.data();
}

bool TimelineView::hasWidget() const
{
    return true;
}

WidgetInfo TimelineView::widgetInfo()
{
    return createWidgetInfo(m_widget.data(),
                            new WidgetInfo::ToolBarWidgetDefaultFactory<TimelineWidget>(m_widget.data()),
                            QStringLiteral("Timeline"),
                            WidgetInfo::LeftPane,
                            0);
}

void TimelineView::modelAttached(Model *model)
{
  
}

void TimelineView::modelAboutToBeDetached(Model *model)
{

}

void TimelineView::importsChanged(const QList<Import> &/*addedImports*/, const QList<Import> &/*removedImports*/)
{

}

void TimelineView::bindingPropertiesChanged(const QList<BindingProperty> & propertyList, PropertyChangeFlags /*propertyChange*/)
{

}

void TimelineView::nodeAboutToBeRemoved(const ModelNode &removedNode)
{
}

void TimelineView::nodeReparented(const ModelNode &node, const NodeAbstractProperty & newPropertyParent, const NodeAbstractProperty & /*oldPropertyParent*/, AbstractView::PropertyChangeFlags /*propertyChange*/)
{

}

void TimelineView::nodeIdChanged(const ModelNode& node, const QString & /*newId*/, const QString & /*oldId*/)
{

}

void TimelineView::propertiesAboutToBeRemoved(const QList<AbstractProperty>& propertyList)
{

}

void TimelineView::propertiesRemoved(const QList<AbstractProperty> &propertyList)
{

}

void TimelineView::rootNodeTypeChanged(const QString & /*type*/, int /*majorVersion*/, int /*minorVersion*/)
{
}

void TimelineView::auxiliaryDataChanged(const ModelNode &modelNode, const PropertyName & name, const QVariant & /*data*/)
{

}

void TimelineView::instanceErrorChange(const QVector<ModelNode> &errorNodeList)
{

}

void TimelineView::nodeOrderChanged(const NodeListProperty &listProperty, const ModelNode &node, int /*oldIndex*/)
{

}

void TimelineView::changeToComponent(const QModelIndex &index)
{

}

void TimelineView::leftButtonClicked()
{

}

void TimelineView::rightButtonClicked()
{

}

void TimelineView::upButtonClicked()
{

}

void TimelineView::downButtonClicked()
{

}

void TimelineView::changeSelection(const QItemSelection & /*newSelection*/, const QItemSelection &/*deselected*/)
{

}

void TimelineView::selectedNodesChanged(const QList<ModelNode> &/*selectedNodeList*/, const QList<ModelNode> &/*lastSelectedNodeList*/)
{
    updateItemSelection();
}

void TimelineView::updateItemSelection()
{

}

QTreeView *TimelineView::treeWidget()
{

}

// along the lines of QObject::blockSignals
bool TimelineView::blockSelectionChangedSignal(bool block)
{

}

void TimelineView::expandRecursively(const QModelIndex &index)
{

}

} // namespace QmlDesigner
