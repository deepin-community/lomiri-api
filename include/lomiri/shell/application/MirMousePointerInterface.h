/*
 * Copyright (C) 2015-2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MIR_MOUSE_POINTER_INTERFACE_H
#define MIR_MOUSE_POINTER_INTERFACE_H

#include <QQuickItem>

/**
 * @brief The QML mouse pointer
 *
 * FIXME: Get this out of lomiri-api
 *
 * This QML item drives the position of the Mir mouse pointer on the scene
 */
class MirMousePointerInterface : public QQuickItem {
    Q_OBJECT

    /**
     * @brief Name of the cursor
     * Defines the look of the mouse pointer along with themeName
     */
    Q_PROPERTY(QString cursorName READ cursorName NOTIFY cursorNameChanged)

    /**
     * @brief Name of the cursor theme
     * Defines the look of the mouse pointer along with cursorName
     * Its default value is "default".
     */
    Q_PROPERTY(QString themeName READ themeName NOTIFY themeNameChanged)

public:
    /**
     * @brief The constructor
     */
    MirMousePointerInterface(QQuickItem *parent = nullptr) : QQuickItem(parent) {}

    /// @cond
    virtual void setCursorName(const QString &cursorName) = 0;
    virtual QString cursorName() const = 0;

    virtual void setThemeName(const QString &themeName) = 0;
    virtual QString themeName() const = 0;

    virtual void moveTo(const QPoint& position) = 0;
    /// @endcond

    /**
     * @brief Sets the custom cursor
     *
     * If it's not a pixmap cursor it will be ignored.
     *
     * To use it, cursorName must be set to "custom". themeName is ignored in this case.
     */
    virtual void setCustomCursor(const QCursor &) = 0;

Q_SIGNALS:
    /// @cond
    void cursorNameChanged(QString name);
    void themeNameChanged(QString name);
    /// @endcond
};

#endif // MIR_MOUSE_POINTER_INTERFACE_H
