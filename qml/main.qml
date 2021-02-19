import QtQuick 2.2
import QtQuick.Window 2.1
import QtMultimedia 5.0
import QtQuick.Layouts 1.11
import CamVideoProducer 0.1
import QtQuick.Controls 2.12

ApplicationWindow {
    id: wnd
    visible: true
    width: 640
    height: 480
    property var newObject: ""


    Button {
        id: test
        anchors {
            left: parent.left
            top: parent.top
        }

        text: "Start"
        onClicked: {
            console.log("start rectangle")
            newObject = Qt.createQmlObject('import QtQuick 2.0; Rectangle { color: "red"; anchors.fill: parent; MouseArea {  anchors.fill: parent;  onClicked: { parent.parent.test(); } } }',
                                                 place,
                                                 "dynamicSnippet1");

        }
    }

    Button {
        id: del
        anchors {
            left: test.right
            top: parent.top
        }

        text: "delete"
        onClicked: {
            newObject.destroy();
            newObject = ""
        }
    }

    Item {
        id: place
        width: 320
        height: 240
        anchors {
            top: test.bottom
            left: parent.left
        }


        function test() {
            console.log("test")
            place.visible = false;
            test.visible = false
            del.visible = false
            hiResItem.visible = true;
        }
    }

    Item {
        id: hiResItem
        visible: false
        anchors.fill: parent

        Rectangle {
            id: payload
            anchors.centerIn: parent
            color: "green"
            width: 400
            height: 400

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("payload destroy");
                    payload.destroy();
                    hiResItem.visible = false;
                    place.visible = true
                    test.visible = true
                    del.visible = true
                }
            }
        }
    }

    Button {
        anchors {
            left: del.right
            top: parent.top
        }

        text: "Open"
        onClicked: popup.open()
    }


    Popup {
        id: popup
        x: 100
        y: 100
        width: 200
        height: 300
        modal: true
        focus: true

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        Menu {
            id: contextMenu
            MenuItem {
                text: qsTr("Back")
                onClicked: {

                }

            }

            MenuItem {
                text: qsTr("Copy")
                onClicked: {

                }
            }
        }
    }
}
