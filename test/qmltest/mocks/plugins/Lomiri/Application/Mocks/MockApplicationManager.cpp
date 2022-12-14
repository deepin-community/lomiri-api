/*
 * Copyright 2013,2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Michael Zanetti <michael.zanetti@canonical.com>
 */

#include <Mocks/MockApplicationManager.h>
#include <Mocks/MockApplicationInfo.h>

using namespace lomiri::shell::application;

MockApplicationManager::MockApplicationManager(QObject* parent): ApplicationManagerInterface(parent)
{
    MockApplicationInfo *item = new MockApplicationInfo("phone-app", "Phone App", "Telephony application", QUrl("/usr/share/pixmaps/some/icon.png"), this);
    m_list.append(item);
    item = new MockApplicationInfo("camera-app", "Camera App", "Lets you take pictures with the camera.", QUrl("/usr/share/pixmaps/some/icon.png"), this);
    m_list.append(item);
    item = new MockApplicationInfo("calendar-app", "Calendar App", "5 missed reminders", QUrl("/usr/share/pixmaps/some/icon.png"), this);
    m_list.append(item);
}

MockApplicationManager::~MockApplicationManager()
{
    while (!m_list.empty())
    {
        m_list.takeFirst()->deleteLater();
    }
}

// cppcheck-suppress unusedFunction
int MockApplicationManager::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

QVariant MockApplicationManager::data(const QModelIndex& index, int role) const
{
    ApplicationInfoInterface *item = m_list.at(index.row());
    switch(role)
    {
    case RoleAppId:
        return item->appId();
    case RoleName:
        return item->name();
    case RoleComment:
        return item->comment();
    case RoleIcon:
        return item->icon();
    case RoleState:
        return item->state();
    case RoleFocused:
        return item->focused();
    case RoleIsTouchApp:
        return item->isTouchApp();
    case RoleExemptFromLifecycle:
        return item->exemptFromLifecycle();
    }

    return QVariant();
}

lomiri::shell::application::ApplicationInfoInterface *MockApplicationManager::get(int index) const
{
    if (index < 0 || index >= m_list.count())
    {
        return nullptr;
    }
    return m_list.at(index);
}

lomiri::shell::application::ApplicationInfoInterface *MockApplicationManager::findApplication(const QString &appId) const
{
    for (auto app : m_list)
    {
        if (app->appId() == appId)
        {
            return app;
        }
    }
    return nullptr;
}

QString MockApplicationManager::focusedApplicationId() const
{
    auto first = m_list.first();
    return (first) ? first->appId() : QString();
}

bool MockApplicationManager::requestFocusApplication(const QString &appId)
{
    Q_UNUSED(appId)
    return true;
}

bool MockApplicationManager::focusApplication(const QString &appId)
{
    Q_UNUSED(appId)
    return true;
}

void MockApplicationManager::unfocusCurrentApplication()
{
}

lomiri::shell::application::ApplicationInfoInterface *MockApplicationManager::startApplication(const QString &appId, const QStringList &arguments)
{
    Q_UNUSED(arguments)
    MockApplicationInfo *item = new MockApplicationInfo(appId, "name", "comment", QUrl());
    return item;
}

bool MockApplicationManager::stopApplication(const QString &appId)
{
    Q_UNUSED(appId)
    return true;
}
