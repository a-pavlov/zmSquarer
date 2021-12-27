/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.1
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

import "variables/fontawesome.js" as FontAwesome
import "variables/buttons.js" as StyleHelper
import "variables/items.js" as StyleHelperItem
import "variables/colors.js" as ColorsHelper

import ZMClient 0.1

FocusScope {
    onActiveFocusChanged: {
        if (activeFocus)
            mainView.state = "showSetupView"
    }


    ZMClient {
        id: zmc
        url: prefs.url

        onMonitors: {
            tilemodel.setAvailableMons(mons, zmUrl.text)
            btnUrl.text = qsTr("Connect")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.visible = false
            prefs.url = url
            prefs.flush()
        }

        onError: {
            btnUrl.text = qsTr("Connect")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error: %1</font> ").arg(msg)
            zmClientError.visible = true
        }
    }

    property bool fullScreen: btnFullScreen.checked

    Rectangle {
        anchors.fill: parent
        clip: true
        KeyNavigation.down: searchView
        Column {
            spacing: 10
            Item{
                id: itemRoot
                width: parent.width
                height: content.implicitHeight + StyleHelperItem.item_padding
                property var style: StyleHelperItem.parseItemClass("stable")

                Rectangle{
                    anchors.fill: parent
                    anchors.margins: - StyleHelperItem.item_border_width
                    color: itemRoot.style.bg
                    border.width: StyleHelperItem.item_border_width
                    border.color: itemRoot.style.border
                    radius: StyleHelperItem.card_border_radius

                    Text {
                        id: content
                        text: "Start using ZM: " + (ZMClient.supportsSsl ? "SSL supported" : "SSL not found")
                        color: itemRoot.style.text
                        font.pixelSize: StyleHelperItem.item_font_size
                        anchors.fill: parent
                        anchors.margins: StyleHelperItem.item_padding
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Row {
                spacing: 10

                TextField {
                    id: zmUrl
                    focus: true
                    anchors.margins: 10
                    height: btnUrl.height
                    placeholderText: qsTr("http://")
                    KeyNavigation.right: btnUrl
                    KeyNavigation.down: searchView
                    ToolTip.delay: 1000
                    ToolTip.timeout: 5000
                    ToolTip.visible: activeFocus
                    ToolTip.text: qsTr("This tool tip is shown after hovering the button for a second.")
                    text: prefs.url
                }

                ButtonDefault {
                    id: btnUrl
                    property bool checkMode: true
                    enabled: zmUrl.text.length > 0
                    text: qsTr("Connect")
                    KeyNavigation.right: btnStartView
                    KeyNavigation.down: searchView
                    icon: FontAwesome.icons.fa_external_link_square
                    class_name: "positive medium"
                    Keys.onPressed: {
                        if (event.key === Qt.Key_Space) {
                            zmUrl.focus = true
                            text = qsTr("Connecting...")
                            zmc.getMonitors()
                            zmClientError.visible = false
                            zmUrl.enabled = checkMode
                            zmUrlProgress.visible = checkMode
                            btnUrl.enabled = false
                        }
                    }
                }

                ButtonDefault {
                    id: btnStartView
                    checkable: false
                    enabled: false
                    text: qsTr("Start")
                    icon: FontAwesome.icons.fa_play
                    class_name: "balanced medium"
                    KeyNavigation.right: btnFullScreen
                    KeyNavigation.down: searchView
                }

                ButtonDefault {
                    id: btnFullScreen
                    anchors.margins: 10
                    x: 10
                    checkable: true
                    checked: prefs.fullScreen
                    text: qsTr("Screen")
                    class_name: "balanced medium"
                    icon: FontAwesome.icons.fa_expand
                    KeyNavigation.right: btnExit
                    KeyNavigation.down: searchView
                    /*
                    ToolTip.delay: 1000
                    ToolTip.timeout: 5000
                    ToolTip.visible: activeFocus
                    ToolTip.text: qsTr("This tool tip is shown after hovering the button for a second.")
*/
                    onClicked: {
                        //wnd.visibility = checked ? Qt.WindowFullScreen : Qt.WindowMinimized
                        //wnd.flags = checked ? (Qt.FramelessWindowHint | Qt.Window) : Qt.Window
                    }
                }

                ButtonDefault {
                    id: btnExit
                    text: qsTr("Quit")
                    class_name: "assertive medium"
                    icon: FontAwesome.icons.fa_sign_out
                    KeyNavigation.down: searchView

                    onClicked: {
                        prefs.fullScreen = btnFullScreen.checked
                        prefs.flush()
                        wnd.close()
                    }
                }
            }

            ProgressBar {
                id: zmUrlProgress
                visible: false
                width: parent.width
                height: 8
                indeterminate: true

                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: 8
                    color: "#e6e6e6"
                    radius: 3
                }

                palette.dark: ColorsHelper.color.positive
            }

            Label {
                id: zmClientError
                text: qsTr("<font color=\"#FF0000\">Error:</font> ")
                visible: false
            }
        }
    }
}
