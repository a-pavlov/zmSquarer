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
import "variables/fontawesome.js" as FontAwesome
import "variables/colors.js" as ColorsHelper
import "variables/items.js" as StyleHelperItem

FocusScope {
    property alias interactive: gridView.interactive
    property alias cellWidth: gridView.cellWidth

    onActiveFocusChanged: {
        if (activeFocus)
            mainView.state = "showTilesView"
    }

    Rectangle {
        anchors.fill: parent
        clip: true
        /*gradient: Gradient {
            GradientStop { position: 0.0; color: "#193441" }
            GradientStop { position: 1.0; color: Qt.darker("#193441") }
        }*/

        Item{
            id: itemRoot
            width: parent.width
            height: content_txt.implicitHeight + StyleHelperItem.item_padding * 2
            property var style: StyleHelperItem.parseItemClass("stable")

            Rectangle{
                anchors.fill: parent
                anchors.margins: - StyleHelperItem.item_border_width
                color: itemRoot.style.bg
                border.width: StyleHelperItem.item_border_width
                border.color: itemRoot.style.border
                radius: StyleHelperItem.card_border_radius

                Text {
                    id: content_txt
                    text: "TEST message"
                    color: itemRoot.style.text
                    font.pixelSize: StyleHelperItem.item_font_size
                    anchors.fill: parent
                    anchors.margins: StyleHelperItem.item_padding
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                }
            }
        }

        GridView {
            id: gridView
            y: 60
            width: parent.width
            height: parent.height - 40
            anchors.leftMargin: 20;
            anchors.rightMargin: 20
            cellWidth: 152; cellHeight: 152
            focus: true
            model: ListModel {
                id: cm
                ListElement {
                    cam_name: "Camera 1 House"
                    active: true
                    resolution: "1920x1080"
                    cam: true
                }

                ListElement {
                    cam_name: "Camera 2 House"
                    active: true
                    resolution: "1920x1080"
                    cam: true
                }

                ListElement {
                    cam_name: "Not assigned"
                    active: false
                    resolution: "N/a"
                    cam: false
                }

                ListElement {
                    cam_name: "Not assigned"
                    active: false
                    resolution: "N/a"
                    cam: false
                }

                ListElement {
                    cam_name: "Not assigned"
                    active: false
                    resolution: "N/a"
                    cam: false
                }

                ListElement {
                    cam_name: "Back yard camera"
                    active: false
                    resolution: "800x600"
                    cam: true
                }

                ListElement {
                    cam_name: "Not assigned"
                    active: false
                    resolution: "N/a"
                    cam: false
                }

                ListElement {
                    cam_name: "Not assigned"
                    active: false
                    resolution: "N/a"
                    cam: false
                }

                ListElement {
                    cam_name: "Not assigned"
                    active: false
                    resolution: "N/a"
                    cam: false
                }

                ListElement {
                    cam_name: "Super puper camera view"
                    active: true
                    resolution: "320x240"
                    cam: true
                }

                ListElement {
                    cam_name: "Super puper camera view 2"
                    active: false
                    resolution: "1024x768"
                    cam: true
                }

                ListElement {
                    cam_name: "Super puper camera view 3"
                    active: true
                    resolution: "1024x768"
                    cam: true
                }
            }

            KeyNavigation.up: searchView            

            delegate: Item {
                id: container
                width: GridView.view.cellWidth; height: GridView.view.cellHeight

                Rectangle {
                    id: content
                    color: "transparent"
                    antialiasing: true
                    anchors.fill: parent; anchors.margins: 20; radius: 10

                    Rectangle {
                        color: cam ? (active ? ColorsHelper.color.balanced :  ColorsHelper.color.assertive) : ColorsHelper.color.energized
                        anchors.fill: parent;
                        anchors.margins: 3;
                        radius: 8;
                        antialiasing: true
                    }

                    TextContent {
                        id: name
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 10
                        width: parent.width - 20
                        text: FontAwesome.icons.fa_video_camera + "\n" +  cam_name
                        wrapMode: Text.WordWrap
                    }

                    TextContent {
                        anchors.top: name.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        id: xx
                        text: resolution
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        container.GridView.view.currentIndex = index
                        container.forceActiveFocus()
                    }
                }

                states: State {
                    name: "active"; when: container.activeFocus
                    PropertyChanges { target: content; color: "black"; scale: 1.1 }
                }

                transitions: Transition {
                    NumberAnimation { properties: "scale"; duration: 100 }
                }

                Keys.onPressed: {
                    console.log("key pressed")
                }
            }
        }
    }
}