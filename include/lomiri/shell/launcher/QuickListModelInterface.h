/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef LOMIRI_SHELL_LAUNCHER_QUICKLISTMODELINTERFACE_H
#define LOMIRI_SHELL_LAUNCHER_QUICKLISTMODELINTERFACE_H

#include <lomiri/SymbolExport.h>

#include <QtCore/QAbstractListModel>

namespace lomiri
{
namespace shell
{
namespace launcher
{

/**
  * @brief A model containing QuickList actions for an application in the launcher.
  *
  * The model has the following roles:
  * - RoleLabel (label): The text entry in the QuickList menu (QString).
  * - RoleIcon (icon): The icon to be shown for this entry (QString).
  * - RoleClickable (clickable): Determines if the entry can be triggered or is just a static text (boolean)
  * - RoleHasSeparator (hasSeparator): Determines if the entry has a separator (boolean)
  * - RoleIsPrivate (isPrivate): Determines whether the entry should be shown or not in locked mode (e.g. greeter is locked)
  */
class LOMIRI_API QuickListModelInterface: public QAbstractListModel
{
    Q_OBJECT

protected:
    /// @cond
    explicit QuickListModelInterface(QObject *parent = 0) : QAbstractListModel(parent) {
        m_roleNames.insert(RoleLabel, "label");
        m_roleNames.insert(RoleIcon, "icon");
        m_roleNames.insert(RoleClickable, "clickable");
        m_roleNames.insert(RoleHasSeparator, "hasSeparator");
        m_roleNames.insert(RoleIsPrivate, "isPrivate");
    }
    /// @endcond
public:
    /**
     * @brief The Roles supported by the model
     *
     * See class description for details.
     */
    enum Roles {
        RoleLabel,
        RoleIcon,
        RoleClickable,
        RoleHasSeparator,
        RoleIsPrivate
    };
    Q_ENUM(Roles)

    /// @cond
    virtual ~QuickListModelInterface() {}
    /// @endcond

    /// @cond
    QHash<int, QByteArray> roleNames() const override {
        return m_roleNames;
    }
    /// @endcond

protected:
    /// @cond
    QHash<int, QByteArray> m_roleNames;
    /// @endcond

};

} // launcher
} // shell
} // lomiri

#endif // LOMIRI_SHELL_LAUNCHER_QUICKLISTMODELINTERFACE_H
