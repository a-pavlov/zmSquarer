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
    property var base_margins: 4

    Button {
        id: test
        anchors {
            left: parent.left
            top: parent.top
            margins: base_margins
        }

        text: "Start"
        //enabled: false
        onClicked: {
            console.log("start rectangle")
            newObject = Qt.createQmlObject(
                        'import QtQuick 2.0; Rectangle { color: "red"; anchors.fill: parent; MouseArea {  anchors.fill: parent;  onClicked: { console.log(\"xxx\"); } } Keys.onPressed: {console.log(\"Dync key pressed\");} }',
                        wnd, "dynamicSnippet1")
            newObject.focus=true
        }
    }

    Button {
        id: load
        anchors {
            left: test.right
            top: parent.top
            margins: base_margins
        }

        text: "load"
        onClicked: {
            loader.sourceComponent = c1
            loader.focus=true
        }
    }

    ImageButton {
        id: xx
        anchors.left: load.right
        anchors.top: load.top
        image:  "qrc:/images/Back.png"
        onClicked: {
            console.log("clicked")
        }

    }

    Loader {
        id: loader
        active: true
        anchors.fill: parent
        focus: true

        Keys.onPressed: {
            console.log("loader key pressed")
        }

        onLoaded: {
            console.log("component loaded");
        }
    }

    Component {
        id: c1

        /*FocusScope {
            focus: true
            x: rect.x
            y: rect.y
            width: rect.width
            height: rect.height
*/
            Rectangle {
                id: rect
                focus: false
                color: "red"
                anchors {
                    //left: parent.left
                    //top: del.bottom
                    margins: base_margins
                }

                width: 100
                height: 100

                Keys.onPressed: {
                    if (event.key === Qt.Key_Escape) {
                        console.log("Escape pressed")
                    }

                    console.log("key pressed")
                    event.accepted = true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("onclic")
                        focus = true
                    }
                }

                Rectangle {
                    id: out1
                    width: 40
                    height: 40
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins: base_margins
                    focus: true
                    color: "black"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("vout 1click")
                            focus = true
                        }
                    }

                    Keys.onPressed: {
                        console.log("VO 1 key pressed")
                        event.accepted = true
                    }
                }

                Rectangle {
                    id: out2
                    width: 40
                    height: 40
                    anchors.centerIn: parent
                    focus: true
                    color: "green"
                    anchors.margins: base_margins

                    anchors.left: out1.right
                    anchors.top: parent.top

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("vout 2 click")
                            focus = true
                        }
                    }

                    Keys.onPressed: {
                        console.log("VO 2 key pressed")
                        event.accepted = true
                    }
                }
            }
        //}
    }


    /*MouseArea {
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
    }*/
}
