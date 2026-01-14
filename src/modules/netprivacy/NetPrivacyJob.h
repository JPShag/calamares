/* SPDX-FileCopyrightText: 2025 JPShag
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef NETPRIVACYJOB_H
#define NETPRIVACYJOB_H

#include <libcalamares/CppJob.h>
#include <libcalamares/DllMacro.h>
#include <QString>

class PLUGINDLLEXPORT NetPrivacyJob : public Calamares::CppJob
{
    Q_OBJECT

public:
    explicit NetPrivacyJob( int macPolicy,
                            const QString& macAddress,
                            const QString& vendorOUI,
                            bool ipv6Privacy,
                            int ipv6Mode,
                            QObject* parent = nullptr );
    ~NetPrivacyJob() override;

    QString prettyName() const override;
    QString prettyDescription() const override;
    QString prettyStatusMessage() const override;
    Calamares::JobResult exec() override;

private:
    Calamares::JobResult writeMacConfig( const QString& root ) const;
    Calamares::JobResult writeIpv6Config( const QString& root ) const;
    Calamares::JobResult writeSystemdLinkConfig( const QString& root ) const;

    bool ensureDirectory( const QString& path ) const;
    bool writeFile( const QString& path, const QString& content ) const;

    int m_macPolicy;
    QString m_macAddress;
    QString m_vendorOUI;
    bool m_ipv6Privacy;
    int m_ipv6Mode;
};

#endif
