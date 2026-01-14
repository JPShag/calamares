/* SPDX-FileCopyrightText: 2025 JPShag
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#fafafa"

    readonly property int fontTitle: 20
    readonly property int fontSection: 15
    readonly property int fontBody: 13
    readonly property int fontCaption: 11

    readonly property color accentColor: "#3498db"
    readonly property color errorColor: "#e74c3c"
    readonly property color successColor: "#27ae60"
    readonly property color textColor: "#2c3e50"
    readonly property color subtextColor: "#7f8c8d"
    readonly property color cardBg: "#ffffff"
    readonly property color cardBorder: "#e0e0e0"

    property string previewMac: ""

    function updatePreview() {
        if (config.macPolicy === 1 || config.macPolicy === 2)
            previewMac = config.generatePreviewMac()
        else
            previewMac = ""
    }

    Connections {
        target: config
        function onMacPolicyChanged() { updatePreview() }
        function onSelectedVendorChanged() { updatePreview() }
    }

    Component.onCompleted: updatePreview()

    Flickable {
        anchors.fill: parent
        anchors.margins: 20
        contentWidth: width
        contentHeight: mainCol.height
        clip: true

        ColumnLayout {
            id: mainCol
            width: parent.width
            spacing: 24

            // Header
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6
                Text { text: qsTr("Network Privacy"); font.pixelSize: fontTitle; font.bold: true; color: textColor }
                Text { text: qsTr("Configure how your device identifies itself on networks."); font.pixelSize: fontBody; color: subtextColor; wrapMode: Text.Wrap; Layout.fillWidth: true }
            }

            // MAC Card
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: macCol.implicitHeight + 28
                radius: 8
                color: cardBg
                border.color: cardBorder

                ColumnLayout {
                    id: macCol
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    Text { text: qsTr("MAC Address Spoofing"); font.pixelSize: fontSection; font.weight: Font.DemiBold; color: textColor }
                    Text { text: qsTr("Your MAC address identifies your hardware. Spoofing prevents tracking."); font.pixelSize: fontCaption; color: subtextColor; wrapMode: Text.Wrap; Layout.fillWidth: true }

                    ButtonGroup { id: macGroup }

                    RowLayout { spacing: 10
                        RadioButton { checked: config.macPolicy === 0; ButtonGroup.group: macGroup; onClicked: config.macPolicy = 0 }
                        ColumnLayout { spacing: 2
                            Text { text: qsTr("Disabled"); font.pixelSize: fontBody; font.weight: Font.Medium; color: textColor }
                            Text { text: qsTr("Use hardware MAC address"); font.pixelSize: fontCaption; color: subtextColor }
                        }
                    }

                    RowLayout { spacing: 10
                        RadioButton { checked: config.macPolicy === 1; ButtonGroup.group: macGroup; onClicked: config.macPolicy = 1 }
                        ColumnLayout { spacing: 2
                            Text { text: qsTr("Random"); font.pixelSize: fontBody; font.weight: Font.Medium; color: textColor }
                            Text { text: qsTr("Random address on each connection"); font.pixelSize: fontCaption; color: subtextColor }
                        }
                    }

                    RowLayout { spacing: 10
                        RadioButton { id: radioVendor; checked: config.macPolicy === 2; ButtonGroup.group: macGroup; onClicked: config.macPolicy = 2 }
                        ColumnLayout { spacing: 2
                            Text { text: qsTr("Vendor"); font.pixelSize: fontBody; font.weight: Font.Medium; color: textColor }
                            Text { text: qsTr("Mimic a specific manufacturer"); font.pixelSize: fontCaption; color: subtextColor }
                        }
                    }

                    RowLayout { visible: radioVendor.checked; Layout.leftMargin: 36; spacing: 10
                        Text { text: qsTr("Vendor:"); font.pixelSize: fontCaption; color: subtextColor }
                        ComboBox {
                            Layout.preferredWidth: 200
                            model: config.vendorList
                            textRole: "name"
                            valueRole: "id"
                            font.pixelSize: fontCaption
                            currentIndex: { var l = config.vendorList; for (var i = 0; i < l.length; i++) if (l[i].id === config.selectedVendor) return i; return 0; }
                            onActivated: { config.selectedVendor = currentValue; updatePreview() }
                        }
                    }

                    RowLayout { spacing: 10
                        RadioButton { id: radioFixed; checked: config.macPolicy === 3; ButtonGroup.group: macGroup; onClicked: config.macPolicy = 3 }
                        ColumnLayout { spacing: 2
                            Text { text: qsTr("Fixed"); font.pixelSize: fontBody; font.weight: Font.Medium; color: textColor }
                            Text { text: qsTr("Set a specific MAC address"); font.pixelSize: fontCaption; color: subtextColor }
                        }
                    }

                    TextField {
                        visible: radioFixed.checked
                        Layout.leftMargin: 36
                        Layout.preferredWidth: 200
                        placeholderText: "XX:XX:XX:XX:XX:XX"
                        text: config.macAddress
                        font.family: "monospace"
                        font.pixelSize: fontBody
                        onTextChanged: config.macAddress = text
                        validator: RegularExpressionValidator { regularExpression: /^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$/ }
                        color: acceptableInput || text.length === 0 ? textColor : errorColor
                    }

                    Rectangle {
                        visible: config.macPolicy === 1 || config.macPolicy === 2
                        Layout.fillWidth: true
                        implicitHeight: 44
                        radius: 6
                        color: Qt.rgba(0.2, 0.6, 0.86, 0.1)
                        border.color: Qt.rgba(0.2, 0.6, 0.86, 0.3)

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 10
                            Text { text: qsTr("Preview:"); font.pixelSize: fontCaption; font.bold: true; color: accentColor }
                            Text { text: previewMac; font.family: "monospace"; font.pixelSize: fontBody; font.bold: true; color: accentColor }
                            Item { Layout.fillWidth: true }
                            Rectangle {
                                width: refreshTxt.width + 12; height: refreshTxt.height + 6; radius: 4
                                color: "transparent"; border.color: accentColor
                                Text { id: refreshTxt; anchors.centerIn: parent; text: qsTr("Refresh"); font.pixelSize: fontCaption; color: accentColor }
                                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: updatePreview() }
                            }
                        }
                    }
                }
            }

            // IPv6 Card
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: ipv6Col.implicitHeight + 28
                radius: 8
                color: cardBg
                border.color: cardBorder

                ColumnLayout {
                    id: ipv6Col
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    Text { text: qsTr("IPv6 Configuration"); font.pixelSize: fontSection; font.weight: Font.DemiBold; color: textColor }
                    Text { text: qsTr("Control IPv6 behavior for privacy and compatibility."); font.pixelSize: fontCaption; color: subtextColor; wrapMode: Text.Wrap; Layout.fillWidth: true }

                    ButtonGroup { id: ipv6Group }

                    RowLayout { spacing: 20
                        RadioButton { text: qsTr("Standard"); font.pixelSize: fontBody; checked: config.ipv6Mode === 0; ButtonGroup.group: ipv6Group; onClicked: config.ipv6Mode = 0 }
                        RadioButton { text: qsTr("Privacy"); font.pixelSize: fontBody; checked: config.ipv6Mode === 1; ButtonGroup.group: ipv6Group; onClicked: config.ipv6Mode = 1 }
                        RadioButton { text: qsTr("Disabled"); font.pixelSize: fontBody; checked: config.ipv6Mode === 2; ButtonGroup.group: ipv6Group; onClicked: config.ipv6Mode = 2 }
                    }

                    Text { visible: config.ipv6Mode === 1; text: "✓ " + qsTr("Uses temporary addresses (RFC 4941)"); font.pixelSize: fontCaption; color: successColor }
                    Text { visible: config.ipv6Mode === 2; text: "⚠ " + qsTr("IPv6 disabled. May break some services."); font.pixelSize: fontCaption; color: errorColor }
                }
            }

            Item { Layout.preferredHeight: 20 }
        }
    }
}
