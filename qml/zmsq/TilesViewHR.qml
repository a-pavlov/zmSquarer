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
            mainView.state = "showTilesViewsHR"
    }

    Rectangle {
        anchors.fill: parent
        clip: true

        Item {
            id: itemRoot
            width: parent.width
            height: content_txt.implicitHeight + StyleHelperItem.item_padding
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
                    text: "Place hi res cameras to corresponding low res tiles"
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
            y: itemRoot.height
            width: parent.width
            height: parent.height - itemRoot.height
            anchors.leftMargin: 20;
            anchors.rightMargin: 20
            cellWidth: 152; cellHeight: 152
            focus: true
            KeyNavigation.up: searchView
            snapMode: ListView.SnapToItem
            clip: true

            model: tilemodel

            delegate: Item {
                id: container
                width: GridView.view.cellWidth; height: GridView.view.cellHeight


                Rectangle {
                    id: content
                    color: "transparent"
                    antialiasing: true
                    anchors.fill: parent;
                    anchors.margins: 5; radius: 10

                    Rectangle {
                        color: havemon_hr ? ((model.status_hr === "Connected") ? ColorsHelper.color.balanced :  ColorsHelper.color.assertive) : ColorsHelper.color.energized
                        anchors.fill: parent;
                        anchors.margins: 3;
                        radius: 8;
                        antialiasing: true
                    }

                    TextContent {
                        id: low_name
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 10
                        width: parent.width - 20
                        text: FontAwesome.icons.fa_video_camera + " " + model.id + " " +
                              (havemon ? model.name : qsTr("No mon"))
                        wrapMode: Text.WordWrap
                    }

                    Rectangle {
                        id: separator
                        anchors.top: low_name.bottom
                        anchors.left: parent.left
                        anchors.margins: 10
                        width: parent.width - 20
                        height: 2
                        color: "black"
                    }

                    TextContent {
                        id: name
                        anchors.top: separator.bottom
                        anchors.left: parent.left
                        anchors.margins: 10
                        width: parent.width - 20
                        text: FontAwesome.icons.fa_video_camera + " " + model.id_hr + " " +
                              (havemon ? (havemon_hr ? model.name_hr : qsTr("Choose HI res mon")) : qsTr("No mon, can't use this slot"))
                        wrapMode: Text.WordWrap
                    }

                    TextContent {
                        id: res
                        anchors.top: name.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: resolution_hr
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        if (container.GridView.view.currentIndex !== index) {
                            container.GridView.view.currentIndex = index
                            container.forceActiveFocus()
                        } else {
                            if (havemon) {
                                updhr = "next"
                            }
                        }
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
                    if (event.key === Qt.Key_Space) {
                        if (havemon) {
                            updhr = "next"
                            console.log("set hi res camera")
                        }
                    }
                }
            }
        }
    }
}
