/*
 * Copyright 2013,2016 Canonical Ltd.
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

#include <Mocks/MockApplicationInfo.h>

using namespace lomiri::shell::application;

MockApplicationInfo::MockApplicationInfo(const QString &appId, const QString& comment, const QString& name, const QUrl& icon, QObject* parent):
    ApplicationInfoInterface(appId, parent),
    m_appId(appId),
    m_name(name),
    m_comment(comment),
    m_icon(icon),
    m_state(Running),
    m_focused(false),
    m_exemptFromLifecycle(false)
{

}

QString MockApplicationInfo::appId() const
{
    return m_appId;
}

QString MockApplicationInfo::comment() const
{
    return m_comment;
}

QString MockApplicationInfo::name() const
{
    return m_name;
}

QUrl MockApplicationInfo::icon() const
{
    return m_icon;
}

ApplicationInfoInterface::State MockApplicationInfo::state() const
{
    return m_state;
}

void MockApplicationInfo::setState(ApplicationInfoInterface::State state)
{
    if (m_state != state)
    {
        m_state = state;
        Q_EMIT stateChanged(m_state);
    }
}

bool MockApplicationInfo::focused() const
{
    return m_focused;
}

void MockApplicationInfo::setFocused(bool focused)
{
    if (m_focused != focused)
    {
        m_focused = focused;
        Q_EMIT focusedChanged(focused);
    }
}

QString MockApplicationInfo::splashTitle() const
{
    return "Foo Bar";
}

QUrl MockApplicationInfo::splashImage() const
{
    return QUrl();
}

bool MockApplicationInfo::splashShowHeader() const
{
    return true;
}

QColor MockApplicationInfo::splashColor() const
{
    return QColor(0,0,0,0);
}

QColor MockApplicationInfo::splashColorHeader() const
{
    return QColor(0,0,0,0);
}

QColor MockApplicationInfo::splashColorFooter() const
{
    return QColor(0,0,0,0);
}

ApplicationInfoInterface::RequestedState MockApplicationInfo::requestedState() const
{
    return RequestedRunning;
}

void MockApplicationInfo::setRequestedState(RequestedState)
{
}

Qt::ScreenOrientations MockApplicationInfo::supportedOrientations() const
{
    return Qt::PortraitOrientation | Qt::LandscapeOrientation;
}

bool MockApplicationInfo::rotatesWindowContents() const
{
    return false;
}

bool MockApplicationInfo::isTouchApp() const
{
    return true;
}

bool MockApplicationInfo::exemptFromLifecycle() const
{
    return m_exemptFromLifecycle;
}

void MockApplicationInfo::setExemptFromLifecycle(bool exemptFromLifecycle)
{
    if (m_exemptFromLifecycle != exemptFromLifecycle)
    {
        m_exemptFromLifecycle = exemptFromLifecycle;
        Q_EMIT exemptFromLifecycleChanged(m_exemptFromLifecycle);
    }
}

int MockApplicationInfo::surfaceCount() const
{
    return 1;
}
