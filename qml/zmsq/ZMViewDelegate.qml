import QtQuick 2.1

import "variables/fontawesome.js" as FontAwesome
import "variables/colors.js" as ColorsHelper

Item {
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
        text: (model.check ? FontAwesome.icons.fa_check : FontAwesome.icons.fa_times) + qsTr("%1\nver: %2 api: %3 ").arg(model.ip).arg(model.version).arg(model.apiversion)
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
            model.upd = "xx";
            if (model.check) {
                prefs.url = model.ip
            }
        }
    }

    Keys.onEnterPressed: {
        model.upd = "xx";
        if (model.check) {
            prefs.url = model.ip
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
