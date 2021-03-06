import QtQuick 2.12
import QtQml.Models 2.1
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import Qt.labs.qmlmodels 1.0
import QtMultimedia 5.0
import ZMClient 0.1
import MonitorModel 0.1
import ZMSQPreferences 0.1
import SceneBuilder 0.1
import CamVideoProducer 0.1

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

    function runHiRes() {
        hiResLoader.active = true
        camsView.visible = false
        hiResLoader.focus = true
        hiResLoader.forceActiveFocus()
    }

    function showStartScreen() {
        camsView.destroy()
        setup.visible = true
        itm.visible = true
        btnStartView.forceActiveFocus()
    }

    ZMSQPreferences {
        id: prefs
    }

    MonitorModel {
        id: monmod

        onMonitorsCountChanged: {
            fc.focus = true
        }
    }

    function saveParameters() {
        setVisualIndexes()
        monmod.save()
        prefs.fullScreen = cbFullScreen.checked
        prefs.url = zmUrl.text
        prefs.flush()
    }

    onClosing: {
        saveParameters()
    }

    ZMClient {
        id: zmc
        url: prefs.url
        onMonitors: {
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
            itm.visible = false
        }

        onFail: {
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error: %1</font> ").arg(code)
            zmClientError.visible = true
        }
    }

    Component.onCompleted: {
        monmod.load()
    }

    function setVisualIndexes() {
        for (var i = 0; i < visualModel.count; ++i) {
            visualModel.model.setVisualIndex(visualModel.items.get(
                                                 i).model.index, i)
        }
    }

    Loader {
        id: hiResLoader
        active: false
        anchors.fill: parent
        sourceComponent: hiResView
        focus: true
    }

    FocusScope {
        id: fc
        focus: true
        anchors.fill: parent

        Keys.onPressed: {
            if (event.key === Qt.Key_Escape || event.key === Qt.Key_Back) {
                event.accepted = true
                Qt.quit()
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
                        id: btnNewLine
                        anchors.margins: base_margins
                        enabled: (monmod.monitorsCount > 0)
                        text: qsTr("Add line sep")
                        onClicked: {
                            monmod.addNewLine()
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
            }
        }

        Rectangle {
            id: itm
            width: parent.width

            //height: parent.height - setup.height - 30
            anchors {
                top: setup.bottom
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                margins: base_margins
            }

            GridView {
                id: root
                anchors.top: parent.top
                anchors.left: parent.left

                width: parent.width
                height: parent.height
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

                model: DelegateModel {
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
                                    visualModel.items.move(
                                                drag.source.visualIndex,
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
                                        id: groupId
                                        anchors {
                                            left: parent.left
                                            top: name.bottom
                                            margins: base_margins
                                        }

                                        text: qsTr("Group: %1").arg(
                                                  model.colorIndex)
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
                                        source: model.monStatus === "Connected" ? "qrc:/images/Goodmark.png" : "qrc:/images/Badmark.png"
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
                                        onDoubleClicked: {
                                            model.color = "test"
                                        }

                                        onClicked: {
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

/*
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onDoubleClicked: {
                            contextMenu_hiRes.popup()
                        }

                        onClicked: {
                            // do not set focus now to video output1
                            //output1.focus=true
                            if (mouse.button === Qt.RightButton)
                                contextMenu_hiRes.popup()
                        }

                        onPressAndHold: {
                            if (mouse.source === Qt.MouseEventNotSynthesized)
                                contextMenu_hiRes.popup()
                        }

                        Menu {
                            id: contextMenu_hiRes
                            MenuItem {
                                text: qsTr("Close")
                                onClicked: {
                                    hiResLoader.active = false
                                    camsView.visible = true
                                    camsView.focus = true
                                }
                            }

                            MenuItem {
                                text: qsTr("Quit application")
                                onClicked: {
                                    saveParameters()
                                    Qt.quit()
                                }
                            }
                        }
                    }*/
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Escape || event.key === Qt.Key_Back) {
                        event.accepted = true
                        hiResLoader.active = false
                        camsView.visible = true
                        camsView.focus = true
                    }
                }
            }
        }
    }
}
