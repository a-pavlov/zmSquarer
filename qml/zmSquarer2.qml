import QtQuick 2.12
import QtQml.Models 2.1
import QtQuick.Controls 2.12
import Qt.labs.qmlmodels 1.0
import ZMClient 0.1
import MonitorModel 0.1
import ZMSQPreferences 0.1

ApplicationWindow {
    id: wnd
    visible: true
    width: 640
    height: 480
    property var comp: ""
    property int cell_width: 100
    property int cell_height: 100
    property int delegate_width: 92
    property int delegate_heigth: 92
    property int base_radius: 4
    property int base_margins: 4

    ZMSQPreferences {
        id: prefs
    }

    MonitorModel {
        id: monmod
        onDataIncoming: {
            //monsList.visible = mc>0
            //monsTitle.visible = mc>0
            //monsStart.visible = mc>0
        }
    }

    Component.onCompleted: {
        console.log("component is ready");
        monmod.testAdd()
        monmod.testAdd()
        monmod.testAdd()
        monmod.testAdd()
        monmod.testAdd()
        monmod.testAdd()
        monmod.addNewLine()
        monmod.addStopper()
    }

    Rectangle {
        id: itm
        width: 320
        height: 400
        color: "white"
        anchors.centerIn: parent

        ListModel {
            id: colorModel
            ListElement {
                mid: "1"
                name: "Name1: camera 1"
                host: "192.168.100.11"
                size: "1920x1080"
                color: "red"
                type: "cam"
                vi: ""
                monStatus: "on"
            }

            ListElement {
                mid: "2"
                name: "Name2: camera 2 test line"
                host: "192.168.100.11"
                size: "1920x1080"
                color: "green"
                type: "cam"
                vi: ""
                monStatus: "on"
            }

            ListElement {
                mid: "3"
                name: "Name3: xxx aaa"
                host: "192.168.100.11"
                size: "1920x1080"
                color: "blue"
                type: "cam"
                vi: ""
                monStatus: "off"
            }

            ListElement {
                mid: "4"
                name: "Name4: mmm"
                host: "192.168.100.11"
                size: "1920x1080"
                color: "goldenrod"
                type: "cam"
                vi: ""
                monStatus: "on"
            }

            ListElement {
                mid: "1"
                name: "Name1"
                host: "192.168.100.11"
                size: "1920x1080"
                color: "pink"
                type: "cam"
                vi: ""
                monStatus: "on"
            }

            ListElement {
                mid: "1"
                name: "Name1"
                host: "192.168.100.11"
                size: "1920x1080"
                color: "violet"
                type: "cam"
                vi: ""
                monStatus: "off"
            }

            ListElement {
                mid: "1"
                name: qsTr("End marker")
                host: "192.168.100.11"
                size: "1920x1080"
                color: "lightgray"
                type: "stopper"
                vi: ""
                monStatus: "active"
            }
        }

        //! [1]
        Button {
            id: btnNL
            text: qsTr("Add \\n")
            anchors {
                top: parent.top
                left: parent.left
                margins: base_margins
            }

            onClicked: {
                colorModel.append({
                                      "color": "lightgray",
                                      "name": qsTr("New line marker"),
                                      "mid": "-1",
                                      "type": "newline",
                                      "monStatus": "xxx"
                                  })
                monmod.testAdd()
                monmod.testAdd()
                monmod.addNewLine()
            }
        }

        Button {
            id: btnState
            text: qsTr("State")
            anchors {
                top: parent.top
                left: btnNL.right
                margins: base_margins
            }

            onClicked: {
                for (var i = 0; i < visualModel.count; ++i) {
                    console.log("visual item " + i + " name: " + visualModel.items.get(
                                    i).model.name + " base index: " + visualModel.items.get(
                                    i).model.index)
                }
            }
        }

        GridView {
            id: root
            anchors.top: btnNL.bottom
            anchors.left: parent.left

            width: 300
            height: 300
            cellWidth: cell_width
            cellHeight: cell_height
            clip: true
            ScrollBar.vertical: ScrollBar {
                visible: true // hides scrollbar
            }

            displaced: Transition {
                NumberAnimation {
                    properties: "x,y"
                    easing.type: Easing.OutQuad
                }
            }

            //! [0]
            model: DelegateModel {
                //! [0]
                id: visualModel
                model: monmod

                delegate: chooser
                DelegateChooser {
                    id: chooser
                    role: "type"

                    DelegateChoice {
                        roleValue: "cam"
                        DropArea {
                            id: delegateRoot
                            width: cell_width
                            height: cell_height

                            onEntered: {
                                visualModel.items.move(drag.source.visualIndex,
                                                       cam.visualIndex)
                            }

                            property int visualIndex: DelegateModel.itemsIndex
                            Binding {
                                target: cam
                                property: "visualIndex"
                                value: visualIndex
                            }

                            Rectangle {
                                id: cam
                                property int visualIndex: 0
                                property int rootIndex: 0
                                width: delegate_width
                                height: delegate_heigth
                                radius: base_radius
                                color: model.color

                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                    verticalCenter: parent.verticalCenter
                                }

                                Label {
                                    id: name
                                    anchors {
                                        left: parent.left
                                        top: parent.top
                                        margins: base_margins
                                    }

                                    width: parent.width
                                    text: model.name
                                    wrapMode: Label.WordWrap
                                }

                                Label {
                                    id: camSize
                                    anchors {
                                        left: parent.left
                                        bottom: parent.bottom
                                        margins: base_margins
                                    }

                                    text: model.size
                                }

                                Image {
                                    id: camMark
                                    source: model.monStatus == "on" ? "qrc:/images/Goodmark.png" : "qrc:/images/Badmark.png"
                                    width: 12
                                    height: 12
                                    anchors {
                                        bottom: parent.bottom
                                        right: parent.right
                                        margins: base_margins
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    //propagateComposedEvents: true

                                    onDoubleClicked: {
                                        console.log("double click on visual index "
                                                    + parent.visualIndex + " indx " + index)
                                        //monmod.remove(index)
                                        model.color = "test"
                                    }

                                    onClicked: {
                                        console.log("click on cam")
                                        model.color = "test"
                                    }
                                }

                                DragHandler {
                                    id: dragHandler
                                }

                                Drag.active: dragHandler.active
                                Drag.source: cam
                                Drag.hotSpot.x: 36
                                Drag.hotSpot.y: 36

                                states: [
                                    State {
                                        when: cam.Drag.active
                                        ParentChange {
                                            target: cam
                                            parent: root
                                        }

                                        AnchorChanges {
                                            target: cam
                                            anchors.horizontalCenter: undefined
                                            anchors.verticalCenter: undefined
                                        }
                                    }
                                ]
                            }
                        }
                    }

                    DelegateChoice {
                        roleValue: "newline"
                        DropArea {
                            width: cell_width
                            height: cell_height

                            onEntered: visualModel.items.move(
                                           drag.source.visualIndex,
                                           newLine.visualIndex)

                            property int visualIndex: DelegateModel.itemsIndex

                            Binding {
                                target: newLine
                                property: "visualIndex"
                                value: visualIndex
                            }

                            Rectangle {
                                id: newLine
                                property int visualIndex: 0
                                property int rootIndex: 0
                                width: delegate_width
                                height: delegate_heigth

                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                    verticalCenter: parent.verticalCenter
                                }

                                Label {
                                    id: name2
                                    anchors.centerIn: parent
                                    text: model.name
                                    wrapMode: Label.WordWrap
                                }

                                radius: base_radius
                                color: model.color

                                Image {
                                    id: pictDel
                                    source: "qrc:/images/delete.png"
                                    width: 12
                                    height: 12
                                    anchors {
                                        top: parent.top
                                        right: parent.right
                                        margins: base_margins
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            monmod.remove(index)
                                        }
                                    }
                                }

                                DragHandler {
                                    id: dragHandler2
                                }

                                Drag.active: dragHandler2.active
                                Drag.source: newLine
                                Drag.hotSpot.x: 36
                                Drag.hotSpot.y: 36

                                states: [
                                    State {
                                        when: newLine.Drag.active
                                        ParentChange {
                                            target: newLine
                                            parent: root
                                        }

                                        AnchorChanges {
                                            target: newLine
                                            anchors.horizontalCenter: undefined
                                            anchors.verticalCenter: undefined
                                        }
                                    }
                                ]
                            }
                        }
                    }

                    DelegateChoice {
                        roleValue: "stopper"
                        DropArea {
                            id: stopperDrop
                            width: cell_width
                            height: cell_height

                            onEntered: visualModel.items.move(
                                           drag.source.visualIndex,
                                           stopper.visualIndex)
                            property int visualIndex: DelegateModel.itemsIndex
                            Binding {
                                target: stopper
                                property: "visualIndex"
                                value: visualIndex
                            }

                            Rectangle {
                                id: stopper
                                property int visualIndex: 0
                                width: delegate_width
                                height: delegate_heigth

                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                    verticalCenter: parent.verticalCenter
                                }

                                radius: base_radius
                                color: model.color

                                Label {
                                    anchors.centerIn: parent
                                    text: model.name
                                    wrapMode: Label.WordWrap
                                }

                                DragHandler {
                                    id: dragHandler3
                                }

                                Drag.active: dragHandler3.active
                                Drag.source: stopper
                                Drag.hotSpot.x: 36
                                Drag.hotSpot.y: 36

                                states: [
                                    State {
                                        when: stopper.Drag.active
                                        ParentChange {
                                            target: stopper
                                            parent: root
                                        }

                                        AnchorChanges {
                                            target: stopper
                                            anchors.horizontalCenter: undefined
                                            anchors.verticalCenter: undefined
                                        }
                                    }
                                ]
                            }
                        }
                    }
                }
            }
        }
    }
}
