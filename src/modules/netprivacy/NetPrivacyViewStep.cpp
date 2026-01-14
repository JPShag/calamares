/* SPDX-FileCopyrightText: 2025 JPShag
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "NetPrivacyViewStep.h"
#include "NetPrivacyJob.h"

#include <libcalamares/GlobalStorage.h>
#include <libcalamares/JobQueue.h>
#include <libcalamares/utils/Logger.h>
#include <libcalamares/utils/Variant.h>

#include <QQmlContext>
#include <QQuickWidget>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QVBoxLayout>

CALAMARES_PLUGIN_FACTORY_DEFINITION( NetPrivacyViewStepFactory, registerPlugin< NetPrivacyViewStep >(); )

NetPrivacyViewStep::NetPrivacyViewStep( QObject* parent )
    : Calamares::ViewStep( parent )
{
    initVendorDatabase();
    if ( !m_vendors.isEmpty() )
        m_selectedVendor = m_vendors.first().id;
}

NetPrivacyViewStep::~NetPrivacyViewStep() {}

void
NetPrivacyViewStep::initVendorDatabase()
{
    m_vendors = {
        { "intel",      "Intel Corporation",       "00:1B:21" },
        { "intel_wifi", "Intel Wi-Fi",             "7C:B0:C2" },
        { "realtek",    "Realtek Semiconductor",   "00:E0:4C" },
        { "apple",      "Apple, Inc.",             "A4:83:E7" },
        { "samsung",    "Samsung Electronics",     "00:26:37" },
        { "dell",       "Dell Inc.",               "00:14:22" },
        { "hp",         "Hewlett-Packard",         "00:1E:0B" },
        { "lenovo",     "Lenovo",                  "00:1A:6B" },
        { "asus",       "ASUSTek Computer",        "00:1D:60" },
        { "qualcomm",   "Qualcomm Atheros",        "00:03:7F" },
        { "broadcom",   "Broadcom Inc.",           "00:10:18" },
        { "mediatek",   "MediaTek Inc.",           "00:0C:E7" },
        { "tp_link",    "TP-Link Technologies",    "54:E6:FC" },
        { "cisco",      "Cisco Systems",           "00:1A:A1" },
        { "netgear",    "Netgear Inc.",            "00:1E:2A" },
        { "huawei",     "Huawei Technologies",     "00:E0:FC" },
        { "xiaomi",     "Xiaomi Communications",   "64:B4:73" },
        { "microsoft",  "Microsoft Corporation",   "00:15:5D" },
        { "vmware",     "VMware, Inc.",            "00:50:56" },
        { "generic",    "Generic (Local Admin)",   "02:00:00" },
    };
}

QString
NetPrivacyViewStep::getVendorOUI( const QString& vendorId ) const
{
    for ( const auto& v : m_vendors )
        if ( v.id == vendorId )
            return v.oui;
    return QStringLiteral( "02:00:00" );
}

QString NetPrivacyViewStep::prettyName() const { return tr( "Network Privacy" ); }

QString
NetPrivacyViewStep::prettyStatus() const
{
    QString status;
    switch ( m_macPolicy )
    {
    case 0: status = tr( "MAC spoofing disabled" ); break;
    case 1: status = tr( "Random MAC address" ); break;
    case 2:
    {
        QString vendorName;
        for ( const auto& v : m_vendors )
            if ( v.id == m_selectedVendor ) { vendorName = v.name; break; }
        status = tr( "Vendor-random MAC (%1)" ).arg( vendorName );
        break;
    }
    case 3: status = tr( "Fixed MAC: %1" ).arg( m_macAddress ); break;
    }
    if ( m_ipv6Mode == 1 )
        status += QStringLiteral( "; " ) + tr( "IPv6 privacy enabled" );
    else if ( m_ipv6Mode == 2 )
        status += QStringLiteral( "; " ) + tr( "IPv6 disabled" );
    return status;
}

QWidget*
NetPrivacyViewStep::widget()
{
    if ( m_widget )
        return m_widget;

    m_widget = new QWidget;
    auto* layout = new QVBoxLayout( m_widget );
    layout->setContentsMargins( 0, 0, 0, 0 );

    auto* qw = new QQuickWidget( m_widget );
    qw->setResizeMode( QQuickWidget::SizeRootObjectToView );
    qw->engine()->rootContext()->setContextProperty( QStringLiteral( "config" ), this );
    qw->setSource( QUrl( QStringLiteral( "qrc:/netprivacy/View.qml" ) ) );

    if ( qw->status() == QQuickWidget::Error )
        cError() << "NetPrivacy QML error:" << qw->errors();

    layout->addWidget( qw );
    return m_widget;
}

bool
NetPrivacyViewStep::isNextEnabled() const
{
    if ( m_macPolicy == 3 )
    {
        static const QRegularExpression re( QStringLiteral( "^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$" ) );
        return re.match( m_macAddress ).hasMatch();
    }
    return true;
}

bool NetPrivacyViewStep::isBackEnabled() const { return true; }
bool NetPrivacyViewStep::isAtBeginning() const { return true; }
bool NetPrivacyViewStep::isAtEnd() const { return true; }

Calamares::JobList
NetPrivacyViewStep::jobs() const
{
    Calamares::JobList list;
    if ( m_macPolicy > 0 || m_ipv6Mode != 0 )
    {
        QString vendorOUI = ( m_macPolicy == 2 ) ? getVendorOUI( m_selectedVendor ) : QString();
        list.append( Calamares::job_ptr(
            new NetPrivacyJob( m_macPolicy, m_macAddress, vendorOUI, m_ipv6Privacy, m_ipv6Mode ) ) );
    }
    return list;
}

void
NetPrivacyViewStep::onActivate()
{
    auto* gs = Calamares::JobQueue::instance()->globalStorage();
    if ( gs->contains( QStringLiteral( "netprivacy_macPolicy" ) ) )
    {
        m_macPolicy = gs->value( QStringLiteral( "netprivacy_macPolicy" ) ).toInt();
        m_macAddress = gs->value( QStringLiteral( "netprivacy_macAddress" ) ).toString();
        m_selectedVendor = gs->value( QStringLiteral( "netprivacy_selectedVendor" ) ).toString();
        m_ipv6Mode = gs->value( QStringLiteral( "netprivacy_ipv6Mode" ) ).toInt();
        Q_EMIT macPolicyChanged();
        Q_EMIT macAddressChanged();
        Q_EMIT selectedVendorChanged();
        Q_EMIT ipv6ModeChanged();
    }
}

void
NetPrivacyViewStep::onLeave()
{
    auto* gs = Calamares::JobQueue::instance()->globalStorage();
    gs->insert( QStringLiteral( "netprivacy_macPolicy" ), m_macPolicy );
    gs->insert( QStringLiteral( "netprivacy_macAddress" ), m_macAddress );
    gs->insert( QStringLiteral( "netprivacy_selectedVendor" ), m_selectedVendor );
    gs->insert( QStringLiteral( "netprivacy_ipv6Mode" ), m_ipv6Mode );
    if ( m_macPolicy == 2 )
        gs->insert( QStringLiteral( "netprivacy_vendorOUI" ), getVendorOUI( m_selectedVendor ) );
}

void
NetPrivacyViewStep::setConfigurationMap( const QVariantMap& cfg )
{
    m_macPolicy = Calamares::getInteger( cfg, "macPolicy", 0 );
    m_macAddress = Calamares::getString( cfg, "macAddress" );
    m_selectedVendor = Calamares::getString( cfg, "selectedVendor" );
    m_ipv6Privacy = Calamares::getBool( cfg, "ipv6Privacy", false );
    m_ipv6Mode = Calamares::getInteger( cfg, "ipv6Mode", 0 );

    if ( m_selectedVendor.isEmpty() && !m_vendors.isEmpty() )
        m_selectedVendor = m_vendors.first().id;

    if ( cfg.contains( "customVendors" ) )
    {
        for ( const auto& item : cfg.value( "customVendors" ).toList() )
        {
            auto map = item.toMap();
            if ( map.contains( "id" ) && map.contains( "name" ) && map.contains( "oui" ) )
            {
                VendorOUI v;
                v.id = map.value( "id" ).toString();
                v.name = map.value( "name" ).toString();
                v.oui = map.value( "oui" ).toString();
                m_vendors.append( v );
            }
        }
    }
}

int NetPrivacyViewStep::macPolicy() const { return m_macPolicy; }
QString NetPrivacyViewStep::macAddress() const { return m_macAddress; }
QString NetPrivacyViewStep::selectedVendor() const { return m_selectedVendor; }
bool NetPrivacyViewStep::ipv6Privacy() const { return m_ipv6Privacy; }
int NetPrivacyViewStep::ipv6Mode() const { return m_ipv6Mode; }

QVariantList
NetPrivacyViewStep::vendorList() const
{
    QVariantList result;
    for ( const auto& v : m_vendors )
    {
        QVariantMap entry;
        entry[ "id" ] = v.id;
        entry[ "name" ] = v.name;
        entry[ "oui" ] = v.oui;
        result.append( entry );
    }
    return result;
}

void NetPrivacyViewStep::setMacPolicy( int p )
{
    if ( m_macPolicy != p ) { m_macPolicy = p; Q_EMIT macPolicyChanged(); Q_EMIT nextStatusChanged( isNextEnabled() ); }
}

void NetPrivacyViewStep::setMacAddress( const QString& a )
{
    if ( m_macAddress != a ) { m_macAddress = a; Q_EMIT macAddressChanged(); Q_EMIT nextStatusChanged( isNextEnabled() ); }
}

void NetPrivacyViewStep::setSelectedVendor( const QString& v )
{
    if ( m_selectedVendor != v ) { m_selectedVendor = v; Q_EMIT selectedVendorChanged(); }
}

void NetPrivacyViewStep::setIpv6Privacy( bool e )
{
    if ( m_ipv6Privacy != e ) { m_ipv6Privacy = e; Q_EMIT ipv6PrivacyChanged(); }
}

void NetPrivacyViewStep::setIpv6Mode( int m )
{
    if ( m_ipv6Mode != m ) { m_ipv6Mode = m; Q_EMIT ipv6ModeChanged(); }
}

QString
NetPrivacyViewStep::generatePreviewMac() const
{
    auto* rng = QRandomGenerator::global();
    QString oui;

    if ( m_macPolicy == 1 )
        oui = QStringLiteral( "%1:%2:%3" )
                  .arg( ( rng->generate() % 256 ) | 0x02, 2, 16, QChar( '0' ) )
                  .arg( rng->generate() % 256, 2, 16, QChar( '0' ) )
                  .arg( rng->generate() % 256, 2, 16, QChar( '0' ) );
    else if ( m_macPolicy == 2 )
        oui = getVendorOUI( m_selectedVendor );
    else
        return QString();

    QString nic = QStringLiteral( "%1:%2:%3" )
                      .arg( rng->generate() % 256, 2, 16, QChar( '0' ) )
                      .arg( rng->generate() % 256, 2, 16, QChar( '0' ) )
                      .arg( rng->generate() % 256, 2, 16, QChar( '0' ) );

    return ( oui + QStringLiteral( ":" ) + nic ).toUpper();
}
