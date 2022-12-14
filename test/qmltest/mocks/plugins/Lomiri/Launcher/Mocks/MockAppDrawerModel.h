/*
 * Copyright 2016 Canonical Ltd.
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
 */

#pragma once

#include <AppDrawerModelInterface.h>

class MockLauncherItem;

using namespace lomiri::shell::launcher;

class LOMIRI_API MockAppDrawerModel: public AppDrawerModelInterface
{
   Q_OBJECT

public:
    MockAppDrawerModel(QObject* parent = 0);
    ~MockAppDrawerModel();

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

private:
    QList<MockLauncherItem*> m_list;
};
