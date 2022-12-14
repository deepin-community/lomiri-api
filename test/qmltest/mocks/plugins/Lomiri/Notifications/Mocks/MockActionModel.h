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
 *      Michał Sawicz <michal.sawicz@canonical.com>
 */


#ifndef MOCKACTIONMODEL_H
#define MOCKACTIONMODEL_H

#include <lomiri/SymbolExport.h>

#include <QtCore/QAbstractListModel>

class MockNotification;

class LOMIRI_API MockActionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MockActionModel(QObject *parent = 0);

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum RoleEnum
    {
        Label = Qt::DisplayRole,
        Id = Qt::UserRole
    };
    Q_ENUM(RoleEnum)

private:
    QHash<int, QByteArray> m_roles;
    MockNotification* m_notification;
};

Q_DECLARE_METATYPE(MockActionModel*)

#endif // MOCKACTIONMODEL_H
