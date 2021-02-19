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
    width: 640
    height: 480
    property var comp: ""
    property int cell_width: 100
    property int cell_height: 100
    property int delegate_width: 92
    property int delegate_heigth: 92
    property int base_radius: 4
    property int base_margins: 4
    property var camsView: ""
    property string hiResUrl: ""

    function runHiRes() {
        hiResLoader.active = true
        camsView.visible = false
    }

    function test() {
        console.log("function test")
    }

    function showStartScreen() {
        camsView.destroy();
        setup.visible = true
        itm.visible = true
    }

    ZMSQPreferences {
        id: prefs
    }

    MonitorModel {
        id: monmod
    }

    ZMClient {
        id: zmc
        url: prefs.url
        onMonitors: {
            monmod.clear()
            monmod.addAll(mons)
            monmod.addStopper()
            btnUrl.text = qsTr("Check")
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
            btnUrl.text = qsTr("Check")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error:</font> ") + msg
            zmClientError.visible = true
        }
    }

    SceneBuilder {
        id: sceneBuilder
        onSuccess: {
            camsView = Qt.createQmlObject(code,wnd,"scene1");
            setup.visible = false;
            itm.visible = false;
        }

        onFail: {
            console.log("fail " + code);
        }
    }

    Component.onCompleted: {
        monmod.load()
    }

    function setVisualIndexes() {
        for (var i = 0; i < visualModel.count; ++i) {
            visualModel.model.setVisualIndex(visualModel.items.get(i).model.index, i)
        }
    }

    Loader {
        id: hiResLoader
        active: false
        anchors.fill: parent
        sourceComponent: hiResView
    }

    GroupBox {
        id: setup
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        title: qsTr("Setup connection")

        ColumnLayout {
            Label {
                id: lb
                text: qsTr("Enter ZM url with scheme")
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
                        if (checkMode) {
                            text = qsTr("Cancel")
                            zmc.getMonitors()
                        } else {
                            text = qsTr("Check")
                        }

                        console.log("check " + checkMode)
                        zmUrl.enabled = checkMode
                        zmUrlProgress.visible = checkMode
                        checkMode = !checkMode
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
                    id: startView
                    anchors.margins: base_margins
                    enabled: monmod.monitorsCount > 0
                    text: qsTr("Start")
                    onClicked: {
                        setVisualIndexes();
                        sceneBuilder.buildScene(zmc,monmod)
                    }
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
    }

    Rectangle {
        id: itm
        width: 420
        height: 500

        anchors {
            top: setup.bottom
            horizontalCenter: parent.horizontalCenter
        }

        /*Button {
            id: btnBack
            text: qsTr("<<< Load")
            anchors {
                top: parent.top
                left: parent.left
                margins: base_margins
            }

            onClicked: {
                console.log("load")
                monmod.load()
            }
        }

        Button {
            id: btnNL
            text: qsTr("Add \\n")
            anchors {
                top: parent.top
                left: btnBack.right
                margins: base_margins
            }

            onClicked: {
                monmod.addNewLine()
            }
        }*/

        Button {
            id: btnState
            text: qsTr("Save")
            anchors {
                top: parent.top
                left: parent.left
                margins: base_margins
            }

            onClicked: {
                setVisualIndexes();
                monmod.save()
            }
        }

        Button {
            id: btnTest
            text: qsTr("Test")
            anchors {
                top: parent.top
                left: btnState.right
                margins: base_margins
            }

            onClicked: {
                monmod.testAdd()
                monmod.testAdd()
                monmod.testAdd()
                monmod.testAdd()
                monmod.testAdd()
                monmod.testAdd()
                monmod.addNewLine()
                monmod.addStopper()
            }
        }

        Button {
            id: btnClear
            text: qsTr("Clear")
            anchors {
                top: parent.top
                left: btnTest.right
                margins: base_margins
            }

            onClicked: {
                console.log("clear")
                monmod.clear()
            }
        }

        GridView {
            id: root
            anchors.top: btnClear.bottom
            anchors.left: parent.left

            width: parent.width
            height: 460
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
                                    id: groupId
                                    anchors {
                                        left: parent.left
                                        top: name.bottom
                                        margins: base_margins
                                    }

                                    text: qsTr("Group: %1").arg(model.colorIndex)
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

    Component {
        id: hiResView
        Rectangle {

            CamVideoProducer {
                id: videoProducer_1
                url: hiResUrl
            }

            VideoOutput {
                id: output1
                anchors.fill: parent
                source: videoProducer_1

                Button {
                    id: button
                    visible: true

                    background: Image {
                        mipmap: true
                        source: "qrc:/images/delete.png"
                        sourceSize.width: parent.width
                        sourceSize.height: parent.height
                    }

                    focusPolicy: Qt.NoFocus

                    Component.onCompleted: {
                        if (button.topInset !== undefined)
                        {
                            button.topInset = 0;
                            button.bottomInset = 0;
                        }
                    }

                    width: 16
                    height: 16
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.bottom: parent.verticalCenter
                    anchors.topMargin: 16
                    onClicked: {
                        hiResLoader.active = false
                        camsView.visible = true
                    }
                }
            }
        }
    }
}
