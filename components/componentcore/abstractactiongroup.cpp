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

#include "abstractactiongroup.h"

#include <QMenu>

namespace QmlDesigner {

AbstractActionGroup::AbstractActionGroup(const QString &displayName) :
    m_displayName(displayName),
    m_menu(new QMenu)
{
    m_menu->setTitle(displayName);
    m_action = m_menu->menuAction();
}

ActionInterface::Type AbstractActionGroup::type() const
{
    return ActionInterface::Menu;
}

QAction *AbstractActionGroup::action() const
{
    return m_action;
}

QMenu *AbstractActionGroup::menu() const
{
    return m_menu.data();
}

SelectionContext AbstractActionGroup::selectionContext() const
{
    return m_selectionContext;
}

void AbstractActionGroup::currentContextChanged(const SelectionContext &selectionContext)
{
    m_selectionContext = selectionContext;
    updateContext();
}

void AbstractActionGroup::updateContext()
{
    if (m_selectionContext.isValid()) {
        m_action->setEnabled(isEnabled(m_selectionContext));
        m_action->setVisible(isVisible(m_selectionContext));
    }
}

} // namespace QmlDesigner
