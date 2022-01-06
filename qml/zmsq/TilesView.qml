import QtQuick 2.1
import "variables/fontawesome.js" as FontAwesome
import "variables/colors.js" as ColorsHelper
import "variables/items.js" as StyleHelperItem

import Platform 0.1

FocusScope {
    property alias interactive: gridView.interactive
    property alias cellWidth: gridView.cellWidth

    Platform {
        id: platform
    }

    onActiveFocusChanged: {
        if (activeFocus)
            mainView.state = "showTilesView"
    }

    Rectangle {
        anchors.fill: parent
        clip: true

        Item {
            id: itemRoot
            width: parent.width
            height: content_txt.implicitHeight + StyleHelperItem.item_padding
            property var style: StyleHelperItem.parseItemClass("stable")

            Rectangle {
                anchors.fill: parent
                anchors.margins: - StyleHelperItem.item_border_width
                color: itemRoot.style.bg
                border.width: StyleHelperItem.item_border_width
                border.color: itemRoot.style.border
                radius: StyleHelperItem.card_border_radius

                Text {
                    id: content_txt
                    text: qsTr("Place low res cameras to squares. %1").arg(platform.name !== "android" ? "Click space to select cam" : "Enter to select cam")
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
            cellWidth: 152; cellHeight: 152
            focus: true
            KeyNavigation.up: searchView
            KeyNavigation.down: tilesViewHR
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
                        color: havemon ? ((model.status === "Connected") ? ColorsHelper.color.balanced :  ColorsHelper.color.assertive) : ColorsHelper.color.energized
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
                        text: FontAwesome.icons.fa_video_camera + " " + model.id + "\n" +
                              (havemon ? model.name : qsTr("place camera here"))
                        wrapMode: Text.WordWrap
                    }

                    TextContent {
                        anchors.top: name.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        id: res
                        text: model.resolution
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
                    if (event.key === Qt.Key_Space) {
                        upd = "test"
                    }
                }

                Keys.onEnterPressed: {
                    upd = "test"
                }
            }
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            wnd.close()
        }
    }
}
