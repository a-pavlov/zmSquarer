import QtQuick 2.1
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

import "variables/fontawesome.js" as FontAwesome

FocusScope {
    onActiveFocusChanged: {
        if (activeFocus)
            mainView.state = "showSearchView"
    }

    ButtonDefault {
        y: 10
        id: httpsEnabled
        checkable: true
        focus: true
        text: qsTr("Https enabled")
        class_name: "balanced medium"
        icon: FontAwesome.icons.fa_check
        KeyNavigation.right: list2
        KeyNavigation.down: btnStartNetworkScan
    }

    ButtonDefault {
        y: parseInt(parent.height / 3)
        id: btnStartNetworkScan
        property bool checkMode: true

        text: "Start search"
        KeyNavigation.right: list2
        KeyNavigation.down: btnStopNetworkScan

        icon: FontAwesome.icons.fa_search
        iconRight: true
        class_name: "positive medium"
    }

    ButtonDefault {
        property bool btnChecked: false
        id: btnStopNetworkScan
        y: parseInt(2 * parent.height / 3)
        checkable: false
        enabled: true
        text: qsTr("Stop scan")
        icon: btnChecked? FontAwesome.icons.fa_check : FontAwesome.icons.fa_times;
        class_name: "balanced medium"
        KeyNavigation.right: list2
        KeyNavigation.down: tilesView
        onClicked: {
            btnChecked = !btnChecked
        }
    }

    ListView {
        id: list2
        y: activeFocus ? 10 : 40;
        x: parseInt(parent.width / 3); width: parent.width / 3; height: parent.height - 20
        KeyNavigation.down: tilesView;
        KeyNavigation.right : list3
        model: ListModel {
            ListElement {
                network: "192.168.100.1"
                selected: false
            }

            ListElement {
                network: "10.10.1.1"
                selected: true
            }

            ListElement {
                network: "192.168.1.1"
                selected: false
            }
        }

        cacheBuffer: 200
        delegate: ListViewDelegate {}

        Behavior on y {
            NumberAnimation { duration: 600; easing.type: Easing.OutQuint }
        }
    }

    ListView {
        id: list3
        y: activeFocus ? 10 : 40; x: parseInt(2 * parent.width / 3); width: parent.width / 3; height: parent.height - 20
        KeyNavigation.down: tilesView;
        KeyNavigation.left: list2
        model: 3; cacheBuffer: 200
        delegate: ListViewDelegate {}

        Behavior on y {
            NumberAnimation { duration: 600; easing.type: Easing.OutQuint }
        }
    }
/*

    Rectangle {
    GroupBox {
        id: selNet
        visible: true
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
    */
}
