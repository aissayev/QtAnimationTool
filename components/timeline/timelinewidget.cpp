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
#include "metainfo.h"
#include "timelinenavigatorentity.h"
#include "timelinewidget.h"
#include "timelineview.h"
#include "qmldesignerconstants.h"
#include "qmldesignericons.h"

#include <theming.h>

#include <invalidqmlsourceexception.h>

#include <coreplugin/icore.h>

#include <utils/qtcassert.h>
#include <utils/stylehelper.h>

#include <QApplication>

#include <QFileInfo>
#include <QShortcut>
#include <QBoxLayout>
#include <QKeySequence>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QDebug>


namespace QmlDesigner {

TimelineWidget::TimelineWidget(TimelineView *view) :
    QQuickWidget(),
    m_timelineView(view),
    m_qmlSourceUpdateShortcut(0)
{
    engine()->addImportPath(qmlSourcesPath());

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_qmlSourceUpdateShortcut = new QShortcut(QKeySequence(Qt::Key_J), this);
    connect(m_qmlSourceUpdateShortcut, SIGNAL(activated()), this, SLOT(reloadQmlSource()));

    QStringList dataList;
    connect(view, SIGNAL(signalModelAttached()), this, SLOT(handleItemChanged()));

    rootContext()->setContextProperty(QLatin1String("modelTree"), QVariant::fromValue(dataList));
    rootContext()->setContextProperty(QLatin1String("creatorTheme"), Theming::theme());
    Theming::registerIconProvider(engine());

    setWindowTitle(tr("Timeline", "Title of timeline view"));
    reloadQmlSource();
}

void TimelineWidget::handleItemChanged() {
    //Fill list with model names
    QList<QObject*> dataList;
    ModelNode root = m_timelineView->rootModelNode();
    fillDataList(&dataList, root, 1);

    qDebug() << "Root Icon Path: [" << ((TimelineNavigatorEntity*)dataList[0])->iconPath() << "]";
    //Link datalist to QML
    rootContext()->setContextProperty(QLatin1String("modelTree"), QVariant::fromValue(dataList));
}

void TimelineWidget::fillDataList(QList<QObject*>*dataList, ModelNode parent, int depth) {
    // Build TimelineNavigatorEntity for parent
    QString name;
    if(parent.hasId())
        name = parent.id();
    else
        name = parent.simplifiedTypeName();
    const ModelNode constParent = parent;
    TimelineNavigatorEntity *data = new TimelineNavigatorEntity(name,getTypeIcon(constParent),depth);
    dataList->append(data);

    QList<ModelNode> children = parent.directSubModelNodes();
    int i;
    for(i=0; i<children.size(); i++){
        ModelNode node = children[i];

        if(node.metaInfo().isGraphicalItem())
            fillDataList(dataList, children[i], depth+1);
    }
}

QString TimelineWidget::qmlSourcesPath() {
    return Core::ICore::resourcePath() + QStringLiteral("/qmldesigner/timelineQmlSources");
}

void TimelineWidget::reloadQmlSource()
{
    QString timelineQmlFilePath = qmlSourcesPath() + QStringLiteral("/QtQuick/timeline.qml");
    qDebug() << timelineQmlFilePath;
    QTC_ASSERT(QFileInfo::exists(timelineQmlFilePath), return);
    engine()->clearComponentCache();
    setSource(QUrl::fromLocalFile(timelineQmlFilePath));

    QTC_ASSERT(rootObject(), return);
    setFixedHeight(initialSize().height());

    connect(rootObject(), SIGNAL(expandedChanged()), this, SLOT(changeHeight()));
}

void TimelineWidget::changeHeight()
{
    setFixedHeight(rootObject()->height());
}

QString TimelineWidget::contextHelpId() const
{
    if (timelineView())
        return  timelineView()->contextHelpId();

    return QString();
}

TimelineView *TimelineWidget::timelineView() const
{
    return m_timelineView.data();
}

}
