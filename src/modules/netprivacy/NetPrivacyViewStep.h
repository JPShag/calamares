/* SPDX-FileCopyrightText: 2025 JPShag
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef NETPRIVACYVIEWSTEP_H
#define NETPRIVACYVIEWSTEP_H

#include <libcalamares/viewpages/ViewStep.h>
#include <libcalamares/DllMacro.h>
#include <libcalamares/utils/PluginFactory.h>
#include <libcalamares/Job.h>

#include <QString>
#include <QVariant>
#include <QVariantList>

class QWidget;

struct VendorOUI
{
    QString id;
    QString name;
    QString oui;
};

class PLUGINDLLEXPORT NetPrivacyViewStep : public Calamares::ViewStep
{
    Q_OBJECT

    Q_PROPERTY( int macPolicy READ macPolicy WRITE setMacPolicy NOTIFY macPolicyChanged )
    Q_PROPERTY( QString macAddress READ macAddress WRITE setMacAddress NOTIFY macAddressChanged )
    Q_PROPERTY( QString selectedVendor READ selectedVendor WRITE setSelectedVendor NOTIFY selectedVendorChanged )
    Q_PROPERTY( QVariantList vendorList READ vendorList CONSTANT )
    Q_PROPERTY( bool ipv6Privacy READ ipv6Privacy WRITE setIpv6Privacy NOTIFY ipv6PrivacyChanged )
    Q_PROPERTY( int ipv6Mode READ ipv6Mode WRITE setIpv6Mode NOTIFY ipv6ModeChanged )

public:
    explicit NetPrivacyViewStep( QObject* parent = nullptr );
    ~NetPrivacyViewStep() override;

    QString prettyName() const override;
    QString prettyStatus() const override;
    QWidget* widget() override;

    bool isNextEnabled() const override;
    bool isBackEnabled() const override;
    bool isAtBeginning() const override;
    bool isAtEnd() const override;

    Calamares::JobList jobs() const override;

    void onActivate() override;
    void onLeave() override;
    void setConfigurationMap( const QVariantMap& configurationMap ) override;

    int macPolicy() const;
    QString macAddress() const;
    QString selectedVendor() const;
    QVariantList vendorList() const;
    bool ipv6Privacy() const;
    int ipv6Mode() const;

    void setMacPolicy( int policy );
    void setMacAddress( const QString& address );
    void setSelectedVendor( const QString& vendorId );
    void setIpv6Privacy( bool enable );
    void setIpv6Mode( int mode );

    Q_INVOKABLE QString generatePreviewMac() const;

Q_SIGNALS:
    void macPolicyChanged();
    void macAddressChanged();
    void selectedVendorChanged();
    void ipv6PrivacyChanged();
    void ipv6ModeChanged();

private:
    void initVendorDatabase();
    QString getVendorOUI( const QString& vendorId ) const;

    QWidget* m_widget = nullptr;
    int m_macPolicy = 0;
    QString m_macAddress;
    QString m_selectedVendor;
    bool m_ipv6Privacy = false;
    int m_ipv6Mode = 0;
    QList< VendorOUI > m_vendors;
};

CALAMARES_PLUGIN_FACTORY_DECLARATION( NetPrivacyViewStepFactory )

#endif
