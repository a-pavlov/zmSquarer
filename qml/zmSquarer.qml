import QtQuick 2.2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.1
import ZMClient 0.1
import MonitorModel 0.1

ApplicationWindow {
    id: wnd
    visible: true
    width: 640
    height: 480

    ZMClient {
        id: zmc
        onMonitors: {
            monmod.clean()
            monmod.addAll(mons)
            btnUrl.text = qsTr("Check")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.visible = false
        }

        onError: {
            monmod.clean()
            btnUrl.text = qsTr("Check")
            btnUrl.enabled = true
            btnUrl.checkMode = true
            zmUrlProgress.visible = false
            zmClientError.text = qsTr("<font color=\"#FF0000\">Error:</font> ") + msg
            zmClientError.visible = true
        }
    }

    MonitorModel {
        id: monmod
        onDataIncoming: {
            monsList.visible = mc>0
            monsTitle.visible = mc>0
            monsStart.visible = mc>0
        }
    }

    ListModel {
       id: lm
       ListElement {
           name: "Bill Smith"
           mid: "555 3264"
           selected: false
           size: "s11"
       }
       ListElement {
           name: "John Brown"
           mid: "555 8426"
           selected: false
           size: "s11"
       }
       ListElement {
           name: "Sam Wise"
           mid: "555 0473"
           selected: false
           size: "s11"
       }
       ListElement {
           name: "Sam Wise"
           mid: "555 0473"
           selected: true
           size: "s11"
       }
       ListElement {
           name: "Sam Wise"
           mid: "555 0473"
           selected: true
           size: "s11"
       }
       ListElement {
           name: "Sam Wise"
           mid: "555 0473"
           selected: false
           size: "s11"
       }
    }

    GroupBox {
        anchors.centerIn: parent
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
                    text: "http://192.168.100.13"
                    onTextChanged: {
                        zmc.setUrl(text)
                    }
                }

                Button {
                    id: btnUrl
                    property bool checkMode: true
                    enabled: zmUrl.text.length > 0
                    text: qsTr("Check")
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

            Label {
                id: monsTitle
                visible: false
                text: qsTr("Monitors")
            }

            Component {
                id: monDelegate
                Item {
                    Layout.fillWidth: true
                    height: 80
                    Column {
                        spacing: 2
                        Row {
                            spacing: 2
                            Text { text: '<b>Id:</b> '      + mid }
                            Text { text: '<b>Name:</b> '    + name }
                        }
                        CheckBox {
                            text: monStatus==="Connected"?("<font color=\"#00FF00\">" + monStatus + "</font>"):monStatus
                            enabled: monStatus !== ""
                            checked: selected
                            onClicked: {
                                model.selected = checked
                                monSelCount.selectedCount = monSelCount.selectedCount + (checked?1:-1)
                            }
                        }
                        Row {
                            spacing: 2
                            Text { text: '<i>Capture FPS:</i> ' + captureFPS }
                            Text { text: '<i>Size:</i> ' + size }
                        }
                    }
                }
            }

            ListView {
                id: monsList
                Layout.fillWidth: true
                height: 200
                visible: false
                //model: lm
                model: monmod
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                delegate: monDelegate
                clip: true

                highlight: Rectangle { color: "lightsteelblue"; radius: 5 }

                ScrollBar.vertical: ScrollBar {
                    active: true
                }
            }

            Label {
                id: monSelCount
                property var selectedCount: 0
                text: '<b>Selected: </b>' + selectedCount
            }

            Button {
                id: monsStart
                text: qsTr("Start")
                property string componentName: ""
                onClicked: {
                    switch(monmod.getCheckedCount()) {
                    case 1: componentName = "square_1.qml"; break;
                    case 2: componentName = "square_2.qml"; break;
                    case 3: componentName = "square_3.qml"; break;
                    case 4: componentName = "square_4.qml"; break;
                    case 5: componentName = "square_5.qml"; break;
                    case 9: componentName = "square_9.qml"; break;
                    default:
                        console.log("Does not supported yet");
                        break;
                    }

                    console.log("component name is " + componentName)
                    if (componentName !== "") {
                        var component = Qt.createComponent(componentName);
                        if (component.status === Component.Ready) {
                            var item = component.createObject(wnd);
                            var counter = 0;
                            for(var i = 0; i < monmod.rowCount(); ++i) {
                                var monId = monmod.getCheckedMonId(i);
                                if (monId !== "") {
                                    console.log("order " + counter + " mon " + monId)
                                    switch(counter++) {
                                        case 0: item.url_1 = zmc.getMonitorUrl(monId); break;
                                        case 1: item.url_2 = zmc.getMonitorUrl(monId); break;
                                        case 2: item.url_3 = zmc.getMonitorUrl(monId); break;
                                        case 3: item.url_4 = zmc.getMonitorUrl(monId); break;
                                        case 4: item.url_5 = zmc.getMonitorUrl(monId); break;
                                        case 5: item.url_6 = zmc.getMonitorUrl(monId); break;
                                        case 6: item.url_7 = zmc.getMonitorUrl(monId); break;
                                        case 7: item.url_8 = zmc.getMonitorUrl(monId); break;
                                        case 8: item.url_9 = zmc.getMonitorUrl(monId); break;
                                        default: break;
                                    }
                                }
                            }
                        }

                    }
                }

                visible: false
            }
        }
     }
}
