import QtQuick 2.12
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
import SceneBuilder 0.1

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
            tilemodel.setAvailableMons(mons, zmUrl.text)
            btnUrl.text = qsTr("Connect")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.visible = false
            prefs.url = url
            prefs.flush()
        }

        onError: {
            monmod.clean()
            btnUrl.text = qsTr("Connect")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error: %1</font> ").arg(msg)
            zmClientError.visible = true
        }
    }

    SceneBuilder {
        id: sceneBuilder
        onSuccess: {
            camsView = Qt.createQmlObject(code, wnd, "scene1")
            camsView.focus = true
            setup.visible = false
            slots.visible = false
        }

        onFail: {
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error: %1</font> ").arg(code)
            zmClientError.visible = true
        }
    }

    function runHiRes() {
        hiResLoader.active = true
        camsView.visible = false
        hiResLoader.focus = true
        hiResLoader.forceActiveFocus()
    }

    function showStartScreen() {
        camsView.destroy()
        setup.visible = true
        slots.visible = true
        btnStartView.forceActiveFocus()
    }

    function saveParameters() {
        tilemodel.save()
        prefs.fullScreen = cbFullScreen.checked
        prefs.url = zmUrl.text
        prefs.flush()
    }

    onClosing: {
        saveParameters()
    }

    Component.onCompleted: {
        tilemodel.load()
    }


    GroupBox {
        id: setup
        anchors.margins: base_margins
        visible: true
        enabled: true
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
                    }

                    Keys.onPressed: {
                        if (event.key == Qt.Key_Down) {
                            console.log("to grid")
                            grid.focus = true
                        }
                    }
                }

                Button {
                    id: btnUrl
                    property bool checkMode: true
                    enabled: zmUrl.text.length > 0
                    anchors.margins: base_margins
                    text: qsTr("Connect")
                    onClicked: {
                        text = qsTr("Connecting...")
                        zmc.getMonitors()
                        zmClientError.visible = false
                        zmUrl.enabled = checkMode
                        zmUrlProgress.visible = checkMode
                        btnUrl.enabled = false
                    }

                    Keys.onPressed: {
                        if (event.key == Qt.Key_Right) {
                            console.log("focus to start")
                            btnStartView.focus = true
                        }
                    }
                }

                Button {
                    id: btnStartView
                    anchors.margins: base_margins
                    enabled: tilemodel.numeratedTilesCount > 0
                    text: qsTr("Start")
                    onClicked: {
                        zmClientError.visible = false
                        sceneBuilder.buildScene(tilemodel)
                    }
                }

                Button {
                    id: btnExit
                    anchors.margins: base_margins
                    text: qsTr("Quit")
                    onClicked: {
                        prefs.fullScreen = cbFullScreen.checked
                        prefs.flush()
                        wnd.close()
                        //Qt.quit()
                    }
                }

                Button {
                    id: refreshNetworks
                    text: "Search"
                    onClicked: {
                        netmon.refresh()
                        netScan.active = true
                        netScan.visible = true
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
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_Escape || event.key === Qt.Key_Back) {
                event.accepted = true
                Qt.quit()
            }

            console.log("key pressed")
        }
    }

    Rectangle {
        id: slots
        anchors {
            top: setup.bottom
            left: setup.left
        }

        anchors.margins: base_margins

        width: 500
        height: 500

        Component {
            id: contactDelegate            

            Item {
                width: grid.cellWidth;
                height: grid.cellHeight
                property bool isCurrent: grid.currentIndex === index
                anchors.margins: base_margins
                //Rectangle {
                //    anchors.fill: parent
                //    border.color: "black"
                //Rectangle {
                //    width: grid.cellWidth;
                //    height: grid.cellHeight
                //
                //    color: isCurrent?'yellow':'green'

                //    Keys.onPressed: {
                //        console.log("key item pressed")
                //    }


                    Rectangle {
                        id: mainView
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.margins: base_margins
                        radius: base_radius

                        color: haveMon?"lightgreen":"lightgray"
                        Text {
                            text: qsTr("Low: %1").arg(name)
                            anchors.centerIn: parent
                            wrapMode: Text.Wrap
                        }

                        width: parent.width - base_margins*2
                        height: parent.height/2 - base_margins*2

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (grid.currentIndex == index) {
                                    upd = "test"
                                } else {
                                    grid.currentIndex = index; //model.pos;
                                }
                            }
                        }
                    }

                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: mainView.bottom
                        radius: base_radius
                        anchors.margins: base_margins

                        color: havehr?"lightgreen":"lightgray"
                        Text {
                            text: qsTr("Hi: %1").arg(hrname)
                            anchors.centerIn: parent
                            wrapMode: Text.Wrap
                        }

                        width: parent.width - base_margins*2
                        height: parent.height/2 - base_margins*2

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (grid.currentIndex == index) {
                                    updhr = "test"
                                } else {
                                    grid.currentIndex = index; //model.pos;
                                }
                            }
                        }
                    }
                //}

               // }
            }
        }


        GridView {
            id: grid
            anchors.fill: parent
            cellWidth: 120; cellHeight: 120
            model: tilemodel
            keyNavigationEnabled: true
            focus: true
            delegate: contactDelegate
            //KeyNavigation.up: setup

            highlight: Rectangle {
                width: grid.cellWidth
                height: grid.cellHeight
                color: "lightsteelblue"
                radius: 5
                x: grid.currentItem.x
                y: grid.currentItem.y
                Behavior on x { SpringAnimation { spring: 3; damping: 0.2 } }
                Behavior on y { SpringAnimation { spring: 3; damping: 0.2 } }
            }

            highlightFollowsCurrentItem: true

            Component.onCompleted: {
                console.log("started")
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Escape) {
                    console.log("back from grid")
                    //grid.focus = false
                    zmUrl.focus = true
                    //btnUrl.focus = true
                    zmUrl.forceActiveFocus()
                }
            }
        }        
    }

    Loader {
        id: netScan
        active: false
        anchors {
            centerIn: parent
            fill: parent
        }

        sourceComponent: ns
        focus: true
    }

    Component {
        id: ns
        Rectangle {
            GroupBox {
                id: selNet
                visible: true
                anchors.centerIn: parent
                title: qsTr("Search ZM")
                width: 250
                height: 450

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
                                        selected = checked
                                    }
                                }
                            }
                        }

                        delegate: comp
                        model: netmon
                    }

                    CheckBox {
                        id: httpsEnabled
                        text: qsTr("Search for HTTPS")
                        checked: false
                    }

                    RowLayout {
                        Button {
                            id: startScanning
                            text: qsTr("Start search")
                            enabled: netmon.selectedCount > 0
                            onClicked: {
                                zmsearch.search(netmon.getSelected(), httpsEnabled.checked)
                            }
                        }

                        Button {
                            id: cancelNetworkScan
                            enabled: zmsearch.inProgress

                            text: qsTr("Stop search")
                            onClicked: {
                                zmsearch.cancel()
                            }
                        }
                    }

                    ProgressBar {
                        id: netScanProgress
                        visible: true
                        width: parent.width
                        indeterminate: false
                        value: zmsearch.progress
                    }

                    Text {
                        id: name2
                        text: qsTr("ZM hosts")
                    }

                    ListView {
                        id: knownHosts
                        height: 100

                        Component {
                            id: khComp
                            Item {
                                height: 40
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
                        }

                        delegate: khComp
                        model: zmsearch
                    }
                }

                Button {
                    id: closeNetScan
                    text: qsTr("Ok")
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    onClicked: {
                        netScan.active = false
                    }
                }
            }
        }
    }

    Loader {
        id: hiResLoader
        active: false
        anchors.fill: parent
        sourceComponent: hiResView
        focus: true
    }

    Component {
        id: hiResView
        Rectangle {
            id: hiResRect
            focus: true

            CamVideoProducer {
                id: videoProducer_1
                url: hiResUrl
            }

            VideoOutput {
                id: output1
                anchors.fill: parent
                source: videoProducer_1

                ImageButton {
                    id: backBtn
                    width: 32
                    height: 32
                    anchors {
                        margins: base_margins*2
                        right: parent.right
                        top: parent.top
                    }

                    image:  "qrc:/images/Goback.png"

                    onClicked: {
                        hiResLoader.active = false
                        camsView.visible = true
                        camsView.focus = true
                    }
                }
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Escape || event.key === Qt.Key_Back) {
                    event.accepted = true
                    hiResLoader.active = false
                    camsView.visible = true
                    camsView.focus = true
                    console.log("key pressed handled")
                }

                console.log("key pressed next")
            }
        }
    }
}
