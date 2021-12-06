import QtQuick 2.2
import QtQuick.Window 2.1
import QtMultimedia 5.0
import QtQuick.Layouts 1.11
import CamVideoProducer 0.1
import QtQuick.Controls 2.12
import NetworksModel 0.1
import ZMSearch 0.1

ApplicationWindow {
    id: wnd
    visible: true
    width: 640
    height: 480
    property var newObject: ""
    property var base_margins: 4


    NetworksModel {
        id: netmon

        onSelectedChanged: {
           console.log("selected " + count)
        }
    }

    ZMSearch {
        id: zmsearch
    }

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

    Button {
        id: refreshNetworks
        anchors {
            left: load.right
            top: parent.top
            margins: base_margins
        }

        text: "refresh networks"
        onClicked: {
            netmon.refresh()
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


    Rectangle {
        id: slots
        anchors {
            top: load.bottom
            left: load.left
        }

        width: 320; height: 320

        Component {
            id: contactDelegate
            Item {
                width: grid.cellWidth;
                height: grid.cellHeight

                Column {
                    //anchors.fill: parent
                    Text { text: name; anchors.horizontalCenter: parent.horizontalCenter }
                    Text { text: num; anchors.horizontalCenter: parent.horizontalCenter; }
                    Text { text: GridView.isCurrentItem ? "curr" : "nc"; anchors.horizontalCenter: parent.horizontalCenter}
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (grid.currentIndex == (num - 1)) {
                            console.log("start cam")
                        } else {
                            console.log("clicked " + model.num + " current item " + grid.currentIndex)
                            grid.currentIndex = model.num - 1;
                        }
                    }

                    onDoubleClicked: {
                        console.log("dbl clk")
                    }
                }
            }

        }

        GridView {
            id: grid
            anchors.fill: parent
            cellWidth: 80; cellHeight: 80

            model: ListModel {
                ListElement {
                    name: "Jim Williams"
                    num: 1
                }

                ListElement {
                    name: "John Brown"
                    num: 2
                }

                ListElement {
                    name: "Bill Smyth"
                    num: 3
                }

                ListElement {
                    name: "Sam Wise"
                    num: 4
                }

                ListElement {
                    name: "Jim Williams"
                    num: 5
                }

                ListElement {
                    name: "John Brown"
                    num: 6
                }

                ListElement {
                    name: "Bill Smyth"
                    num: 7
                }

                ListElement {
                    name: "Sam Wise"
                    num: 8
                }

                ListElement {
                    name: "Jim Williams"
                    num: 9
                }

                ListElement {
                    name: "John Brown"
                    num: 10
                }

                ListElement {
                    name: "Bill Smyth"
                    num: 11
                }

                ListElement {
                    name: "Sam Wise"
                    num: 12
                }

                ListElement {
                    name: "Jim Williams"
                    num: 13
                }

                ListElement {
                    name: "John Brown"
                    num: 14
                }

                ListElement {
                    name: "Bill Smyth"
                    num: 15
                }

                ListElement {
                    name: "Sam Wise"
                    num: 16
                }
            }

            delegate: contactDelegate
            highlight: Rectangle {
                width: grid.cellWidth; height: grid.cellHeight
                        color: "lightsteelblue"; radius: 5
                        x: grid.currentItem.x
                        y: grid.currentItem.y
                        Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
                        Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }

                //color: "lightsteelblue";
                //radius: 5
                //Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
                //Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
            }

            highlightFollowsCurrentItem: true
            //keyNavigationEnabled : true
            focus: true
            //onCurrentItemChanged: console.log(model.get(list.currentIndex).name + ' selected')

            Component.onCompleted: {
                                      console.log("started")
                                  }
        }
    }


    ListView {
        id: networks
        anchors.left: slots.right
        anchors.top: slots.top
        width: 120;
        height: 80

        Component {
            id: comp
            Row {
                CheckBox {
                    id: nm
                    checked: selected
                    text: address
                    onCheckedChanged: {
                        console.log("check " + checked)
                        selected = checked
                    }
                }
            }
        }

        delegate: comp
        model: netmon
    }

    ListView {
        id: knownHosts
        anchors.left: slots.left
        anchors.top: slots.bottom
        width: 100;
        height: 80

        Component {
            id: khComp
            Row {
                Text {
                    id: ip_address
                    text: ip
                }

                Text {
                    id: ver
                    text: version
                }

                Text {
                    id: api
                    text: apiversion
                }
            }
        }

        delegate: khComp
        model: zmsearch
    }


    Button {
        id: startScanning
        text: "Scan..."
        anchors.top: networks.bottom
        anchors.left: networks.left
        enabled: netmon.selectedCount > 0
        onClicked: {
            console.log("start scanning")
            zmsearch.search(netmon.getSelected())
        }
    }

    Button {
        id: cancelNetworkScan

        anchors {
            left: startScanning.right
            top: startScanning.top
            margins: base_margins
        }

        enabled: zmsearch.inProgress

        text: "cancel scan"
        onClicked: {
            console.log("cancel scan")
            zmsearch.cancel()
        }
    }
}
