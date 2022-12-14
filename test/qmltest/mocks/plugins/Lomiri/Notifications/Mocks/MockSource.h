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


#ifndef MOCKSOURCE_H
#define MOCKSOURCE_H

#include <lomiri/SymbolExport.h>

#include <lomiri/shell/notifications/SourceInterface.h>

#include <QtCore/QVariantMap>

namespace lomiri
{
namespace shell
{
namespace notifications
{

class ModelInterface;

} // namespace notifications
} // namespace shell
} // namespace lomiri

class MockModel;

using namespace lomiri::shell::notifications;

class LOMIRI_API MockSource : public SourceInterface
{
    Q_OBJECT
public:
    explicit MockSource(QObject *parent = 0);

    ModelInterface* model() const override;

    void setModel(ModelInterface* model) override;

    Q_INVOKABLE void send(QVariantMap data);

private Q_SLOTS:
    void onCompleted();

private:
    MockModel* m_model;
};

#endif // MOCKSOURCE_H
