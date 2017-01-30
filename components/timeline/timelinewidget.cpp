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

#include <QBoxLayout>
#include <QToolButton>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QtDebug>
#include <utils/fileutils.h>

namespace QmlDesigner {

TimelineWidget::TimelineWidget(TimelineView *view) :
        QFrame(),
        m_timelineView(view)
{

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    setLayout(layout);

    setWindowTitle(tr("Timeline", "Title of timeline view"));

    setStyleSheet(Theming::replaceCssColors(QString::fromUtf8(Utils::FileReader::fetchQrc(QLatin1String(":/qmldesigner/stylesheet.css")))));
}

void TimelineWidget::setTreeModel(QAbstractItemModel* model)
{
}

QList<QToolButton *> TimelineWidget::createToolBarWidgets()
{
    QList<QToolButton *> buttons;

    buttons.append(new QToolButton());
    buttons.last()->setIcon(Icons::PLAY.icon());
    buttons.last()->setToolTip(tr("Play the sequence (CTRL + J)."));
    buttons.last()->setShortcut(QKeySequence(Qt::Key_J | Qt::CTRL));
    connect(buttons.last(), SIGNAL(clicked()), this, SIGNAL(leftButtonClicked()));

    return buttons;
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
