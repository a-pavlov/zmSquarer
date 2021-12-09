import QtQuick 2.3
import QtQuick.Window 2.1
import QtMultimedia 5.0
import QtQuick.Layouts 1.11
import CamVideoProducer 0.1
import QtQuick.Controls 2.12
import NetworksModel 0.1
import ZMSearch 0.1
import TileModel 0.1
import ZMClient 0.1
import ZMSQPreferences 0.1

ApplicationWindow {
    id: wnd
    visible: true
    width: 1024
    height: 768
    visibility: cbFullScreen.checked ? Qt.WindowFullScreen : Qt.WindowMinimized
    flags: cbFullScreen.checked ? (Qt.FramelessWindowHint | Qt.Window) : Qt.Window
    property var comp: ""
    property int cell_width: 160
    property int cell_height: 160
    property int delegate_width: 152
    property int delegate_heigth: 152
    property int base_radius: 4
    property int base_margins: 4
    property var camsView: ""
    property string hiResUrl: ""

    property var newObject: ""

    NetworksModel {
        id: netmon
        onSelectedChanged: {
           console.log("selected " + count)
        }
    }

    ZMSQPreferences {
        id: prefs
    }

    TileModel {
        id: tilemodel
    }

    ZMSearch {
        id: zmsearch
    }

    ZMClient {
        id: zmc
        url: prefs.url
        onMonitors: {
            /*
            monmod.clear()
            monmod.addAll(mons)
            monmod.addStopper()
            btnUrl.text = qsTr("Connect")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.visible = false
            // flush settings in case of successfull connection
            prefs.url = url
            prefs.flush()*/
        }

        onError: {
            /*
            monmod.clean()
            btnUrl.text = qsTr("Connect")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error: %1</font> ").arg(msg)
            zmClientError.visible = true
            */
        }
    }

    GroupBox {
        id: setup
        focus: true
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        title: qsTr("Setup connection")

        ColumnLayout {
            Label {
                id: lb
                text: qsTr("Enter ZM url with scheme. ") + (zmc.supportsSsl?qsTr("<font color=\"green\">SSL supported</font>"):qsTr("<font color=\"yellow\">SSL not supported</font>"))
            }

            RowLayout {
                id: r

                TextField {
                    id: zmUrl
                    placeholderText: qsTr("http://")
                    text: prefs.url
                    onTextChanged: {
                        zmc.url = text
                        console.log("text changed " + text)
                    }
                }

                Button {
                    id: btnUrl
                    property bool checkMode: true
                    enabled: zmUrl.text.length > 0
                    anchors.margins: base_margins
                    text: qsTr("Connect")
                    onClicked: {
                        //if (checkMode) {
                        text = qsTr("Connecting...")
                        zmc.getMonitors()

                        //} else {
                        //    text = qsTr("Check")
                        //}
                        console.log("check " + checkMode)
                        zmClientError.visible = false
                        zmUrl.enabled = checkMode
                        zmUrlProgress.visible = checkMode
                        //checkMode = !checkMode
                        btnUrl.enabled = false
                    }
                }

                Button {
                    id: btnStartView
                    anchors.margins: base_margins
                    enabled: monmod.monitorsCount > 0
                    text: qsTr("Start")
                    onClicked: {
                        zmClientError.visible = false
                        setVisualIndexes()
                        sceneBuilder.buildScene(zmc, monmod)
                    }
                }

                Button {
                    id: btnExit
                    anchors.margins: base_margins
                    enabled: monmod.monitorsCount > 0
                    text: qsTr("Quit")
                    onClicked: {
                        prefs.fullScreen = cbFullScreen.checked
                        prefs.flush()
                        Qt.quit()
                    }
                }

                Button {
                    id: refreshNetworks
                    text: "Search"
                    onClicked: {
                        netmon.refresh()
                        //netScanLoader.active = true
                        selNet.visible = true
                    }
                }
            }

            CheckBox {
                id: cbFullScreen
                text: qsTr("Full screen mode")
                checked: prefs.fullScreen
                onCheckedChanged: {

                }
            }

            ProgressBar {
                id: zmUrlProgress
                visible: false
                width: parent.width
                indeterminate: true
            }

            Label {
                id: zmClientError
                text: qsTr("<font color=\"#FF0000\">Error:</font> ")
                visible: false
            }

            /*Loader {
                id: netScanLoader
                // Explicitly set the size of the
                // Loader to the parent item's size
                anchors.fill: parent
                sourceComponent: rect
                active: false
                focus: true
            }

            Component {
              id: rect
              Rectangle {
                width: 50
                height: 50
                color: "red"
                }
            }*/
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_Escape || event.key === Qt.Key_Back) {
                event.accepted = true
                Qt.quit()
            }
        }
    }

    Rectangle {
        id: slots
        anchors {
            top: setup.bottom
            left: setup.left
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
                    Text { text: select; anchors.horizontalCenter: parent.horizontalCenter; }
                    Text { text: GridView.isCurrentItem ? "curr" : "nc"; anchors.horizontalCenter: parent.horizontalCenter}
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (grid.currentIndex == pos) {
                            upd = "test"
                        } else {
                            console.log("clicked " + model.pos + " current item " + grid.currentIndex)
                            grid.currentIndex = model.pos;
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
            model: tilemodel

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

    Dialog {
        id: selNet
        visible: false
        anchors.centerIn: parent
        title: qsTr("Search ZM dialog")

        onAccepted: {
            console.log("Accept ")
        }

        onRejected: {
            console.log("Rejected")
        }

        standardButtons: StandardButton.Ok

        contentItem: Rectangle {
            ColumnLayout {
                Text {
                    id: name
                    text: qsTr("Networks:")
                }

                ListView {
                    id: networks
                    implicitHeight: 100

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

                RowLayout {
                    Button {
                        id: startScanning
                        text: "Scan..."
                        enabled: netmon.selectedCount > 0
                        onClicked: {
                            console.log("start scanning")
                            zmsearch.search(netmon.getSelected())

                        }
                    }

                    Button {
                        id: cancelNetworkScan
                        enabled: zmsearch.inProgress

                        text: "Cancel scan"
                        onClicked: {
                            console.log("cancel scan")
                            zmsearch.cancel()
                        }
                    }
                }

                Text {
                    id: name2
                    text: qsTr("ZM hosts")
                }

                ListView {
                    id: knownHosts
                    implicitHeight: 100

                    Component {
                        id: khComp
                        CheckBox {
                            id: ip_address
                            checked: check
                            text: ip
                            onClicked: {
                                upd = "xx"
                                if (checked) {
                                    zmUrl.text = ip
                                } else {
                                    zmUrl.text = ""
                                }
                            }
                        }
                    }

                    delegate: khComp
                    model: zmsearch
                }
            }

            color: "white"
            implicitWidth: 400
            implicitHeight: 300
        }
    }
}
