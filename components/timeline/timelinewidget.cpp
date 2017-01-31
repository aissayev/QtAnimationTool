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

namespace QmlDesigner {

TimelineWidget::TimelineWidget(TimelineView *view) :
        QQuickWidget(),
        m_timelineView(view)
{
    engine()->addImportPath(qmlSourcesPath());

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    rootContext()->setContextProperty(QLatin1String("creatorTheme"), Theming::theme()); 
    Theming::registerIconProvider(engine());

    setWindowTitle(tr("Timeline", "Title of timeline view"));
    reloadQmlSource();
}

QString TimelineWidget::qmlSourcesPath() {
    return Core::ICore::resourcePath() + QStringLiteral("/qmldesigner/timelineQmlSources");
}

void TimelineWidget::reloadQmlSource()
{
    QString statesListQmlFilePath = qmlSourcesPath() + QStringLiteral("/timeline.qml");
    QTC_ASSERT(QFileInfo::exists(statesListQmlFilePath), return);
    engine()->clearComponentCache();
    setSource(QUrl::fromLocalFile(statesListQmlFilePath));

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
