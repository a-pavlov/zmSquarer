import QtQuick 2.2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.1
import ZMClient 0.1

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
        }
    }

    GroupBox {
        anchors.centerIn: parent
        title: qsTr("Setup connection")

        ColumnLayout {
            anchors.fill: parent

            Label {
                id: lb
                text: qsTr("Enter ZM url with scheme")
            }

            RowLayout {
                id: r
                TextField {
                    id: zmUrl
                    placeholderText: qsTr("http://")
                }

                Button {
                    id: btnUrl
                    property bool checkMode: true
                    enabled: zmUrl.text.length > 0
                    text: qsTr("Check")
                    onClicked: {
                        if (checkMode) {
                            text = qsTr("Cancel")
                            zmc.getMonitors(zmUrl.text + "/zm/api/monitors.json")
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
                id: title
                text: qsTr("Monitors")
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: monmod

                delegate: Text {
                    id: monid
                    text: id + ": " + name
                }
            }
        }
    }
}
