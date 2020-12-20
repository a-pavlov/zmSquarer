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
                    enabled: zmUrl.text.length > 0
                    text: qsTr("Check")
                    onClicked: {
                        text = qsTr("Cancel")
                        zmUrl.enabled = false
                        zmUrlProgress.visible = true
                    }
                }

                Button {
                    id: btnTest
                    enabled: true
                    text: qsTr("Test")
                    onClicked: {
                        //text = qsTr("Cancel")
                        //zmUrl.enabled = false
                        //zmUrlProgress.visible = true
                        console.log("start test")
                        zmc.getMonitors("http://192.168.100.12/zm/api/monitors.json")
                    }
                }
            }

            ProgressBar {
                id: zmUrlProgress
                visible: false
                width: parent.width
                indeterminate: true
            }
        }
    }
}
