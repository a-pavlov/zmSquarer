import QtQuick 2.1
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

import "variables/fontawesome.js" as FontAwesome
import "variables/colors.js" as ColorsHelper

FocusScope {
    onActiveFocusChanged: {
        if (activeFocus)
            mainView.state = "showSearchView"
    }

    property bool httpsEnabled: btnHttpsEnabled.checked

    ButtonDefault {
        y: 10
        id: btnHttpsEnabled
        checkable: true
        focus: true
        checked: prefs.httpsEnabled
        text: qsTr("Https enabled")
        class_name: "balanced medium"
        icon: FontAwesome.icons.fa_check
        KeyNavigation.right: list2
        KeyNavigation.down: networkScan
    }

    ButtonDefault {
        id: networkScan
        y: parseInt(parent.height / 3)
        enabled: netmon.selectedCount > 0
        property bool startMode: true
        text: startMode ? qsTr("Start search") : qsTr("Stop search")
        KeyNavigation.right: list2
        KeyNavigation.down: tilesView
        icon: startMode ? FontAwesome.icons.fa_search : FontAwesome.icons.fa_stop
        class_name: "positive medium"

        Keys.onPressed: {
            if (event.key === Qt.Key_Space) {
                if (startMode) {
                    zmsearch.search(netmon.getSelected(), btnHttpsEnabled.checked)
                } else {
                    zmsearch.cancel()
                }
                startMode = !startMode
            }
        }

        onClicked: {
            if (event.key === Qt.Key_Space) {
                if (startMode) {
                    zmsearch.search(netmon.getSelected(), btnHttpsEnabled.checked)
                } else {
                    zmsearch.cancel()
                }
                startMode = !startMode
            }
        }
    }

    ProgressBar {
        id: netScanProgress
        width: parent.width / 3
        y: parseInt(2 * parent.height / 3)
        visible: true
        indeterminate: false
        value: zmsearch.progress
        height: 10
        palette.dark: ColorsHelper.color.positive
    }

    ListView {
        id: list2
        x: parseInt(parent.width / 3); width: parent.width / 3; height: parent.height - 20
        KeyNavigation.down: tilesView;
        KeyNavigation.right : list3
        model: netmon
        cacheBuffer: 200
        header: Item {
            id: header_networks
            width: ListView.view.width; height: 60; anchors.leftMargin: 10; anchors.rightMargin: 10

            Rectangle {
                id: h_content
                anchors.centerIn: parent; width: header_networks.width - 40; height: header_networks.height - 10
                color: "transparent"
                antialiasing: true
                radius: 10
                Rectangle { anchors.fill: parent; anchors.margins: 3; color: ColorsHelper.color.energized; antialiasing: true; radius: 8 }
            }

            Text {
                id: h_label
                anchors.centerIn: h_content
                text: qsTr("Available networks")
                color: "#193441"
                font.pixelSize: 14
                font.bold: true
            }
        }

        delegate: Item {
            id: container
            width: ListView.view.width; height: 60; anchors.leftMargin: 10; anchors.rightMargin: 10

            Rectangle {
                id: content
                anchors.centerIn: parent; width: container.width - 40; height: container.height - 10
                color: "transparent"
                antialiasing: true
                radius: 10

                Rectangle { anchors.fill: parent; anchors.margins: 3; color: ColorsHelper.color.energized; antialiasing: true; radius: 8 }
            }

            Text {
                id: label
                anchors.centerIn: content
                text: (model.selected ? FontAwesome.icons.fa_check : FontAwesome.icons.fa_times) + " " + model.address
                color: "#193441"
                font.pixelSize: 14
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    container.ListView.view.currentIndex = index
                    container.forceActiveFocus()
                }
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Space) {
                    model.selected = !model.selected
                }
            }

            states: State {
                name: "active"; when: container.activeFocus
                PropertyChanges { target: content; color: "black"; scale: 1.1 }
                PropertyChanges { target: label; font.pixelSize: 16 }
            }

            transitions: Transition {
                NumberAnimation { properties: "scale"; duration: 100 }
            }
        }

        Behavior on y {
            NumberAnimation { duration: 600; easing.type: Easing.OutQuint }
        }
    }

    ListView {
        id: list3
        x: parseInt(2 * parent.width / 3); width: parent.width / 3; height: parent.height - 20
        KeyNavigation.down: tilesView;
        KeyNavigation.left: list2
        model: zmsearch
        cacheBuffer: 200

        header: Item {
            id: zm_hosts_header
            width: ListView.view.width; height: 60; anchors.leftMargin: 10; anchors.rightMargin: 10

            Rectangle {
                id: zm_hosts_content
                anchors.centerIn: parent; width: zm_hosts_header.width - 40; height: zm_hosts_header.height - 10
                color: "transparent"
                antialiasing: true
                radius: 10

                Rectangle { anchors.fill: parent; anchors.margins: 3; color: ColorsHelper.color.energized; antialiasing: true; radius: 8 }
            }

            Text {
                id: zm_hosts_label
                anchors.centerIn: zm_hosts_content
                text: qsTr("Found ZM hosts")
                color: "#193441"
                font.pixelSize: 14
                font.bold: true
            }
        }

        delegate: ZMViewDelegate {}

        Behavior on y {
            NumberAnimation { duration: 600; easing.type: Easing.OutQuint }
        }
    }
}
