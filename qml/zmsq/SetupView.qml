import QtQuick 2.1
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1

import "qrc:/qml/zmsq/variables/fontawesome.js" as FontAwesome
import "qrc:/qml/zmsq/variables/buttons.js" as StyleHelper
import "qrc:/qml/zmsq/variables/items.js" as StyleHelperItem
import "qrc:/qml/zmsq/variables/colors.js" as ColorsHelper

import ZMClient 0.1
import Platform 0.1

FocusScope {
    onActiveFocusChanged: {
        if (activeFocus)
            mainView.state = "showSetupView"
    }

    property bool doConnect: true
    property bool wantMonitors: false
    property bool wantStart: false

    ZMClient {
        id: zmc
        url: prefs.url

        onMonitors: {
            tilemodel.setAvailableMons(mons, zmUrl.text)
            doConnect = true
            zmUrlProgress.visible = false
            zmClientError.visible = false
            zmClientError.color = ColorsHelper.color.balanced
            zmClientError.text = qsTr("Monitors found: %1").arg(mcount)
            zmClientError.visible = true
        }

        onError: {
            doConnect = true
            zmUrlProgress.visible = false
            zmClientError.color = ColorsHelper.color.assertive
            zmClientError.text = qsTr("Error: %1").arg(msg)
            zmClientError.visible = true
        }

        onAuthentificationRequired: {
            doConnect = true
            zmUrlProgress.visible = false
            zmClientError.color = ColorsHelper.color.assertive
            zmClientError.text = qsTr("Authentication required")
            zmClientError.visible = true
            loginDialog.open()
        }

        onInvalidCredentials: {
            doConnect = true
            zmUrlProgress.visible = false
            zmClientError.color = ColorsHelper.color.assertive
            zmClientError.text = qsTr("Invalid credentials provided")
            zmClientError.visible = true
            loginDialog.open()
        }

        onLogged: {
            zmClientError.color = ColorsHelper.color.balanced
            zmClientError.text = qsTr("Login successfull")
            zmClientError.visible = true
        }

        onVersion: {
            sceneBuilder.buildScene(tilemodel, zmc.getMonitorUrl())
            zmClientError.visible = false
        }
    }

    Platform {
        id: platform
    }

    property bool fullScreen: btnFullScreen.checked

    Rectangle {
        anchors.fill: parent
        clip: true
        KeyNavigation.down: searchView
        Column {
            spacing: 10

            Item{
                id: itemRoot
                width: parent.width
                height: content.implicitHeight + StyleHelperItem.item_padding
                property var style: StyleHelperItem.parseItemClass("stable")

                Rectangle{
                    anchors.fill: parent
                    anchors.margins: - StyleHelperItem.item_border_width
                    color: itemRoot.style.bg
                    border.width: StyleHelperItem.item_border_width
                    border.color: itemRoot.style.border
                    radius: StyleHelperItem.card_border_radius

                    Text {
                        id: content
                        text: qsTr("Setup squarer for ZM: ") + (zmc.supportsSsl?qsTr("<font color=\"green\">SSL supported</font>"):qsTr("<font color=\"yellow\">SSL not supported</font>"))
                        color: itemRoot.style.text
                        font.pixelSize: StyleHelperItem.item_font_size
                        anchors.fill: parent
                        anchors.margins: StyleHelperItem.item_padding
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Row {
                spacing: 10

                TextField {
                    id: zmUrl
                    focus: true
                    anchors.margins: 10
                    height: btnConnectZM.height
                    placeholderText: qsTr("http://")
                    KeyNavigation.right: btnConnectZM
                    KeyNavigation.down: searchView
                    font.pixelSize: StyleHelperItem.item_font_size
                    ToolTip.delay: StyleHelperItem.item_tooltip_delay
                    ToolTip.timeout: StyleHelperItem.item_tooltip_timeout
                    ToolTip.visible: activeFocus
                    ToolTip.text: qsTr("Enter ZM host url here with http or https and click connect")
                    text: prefs.url
                }

                ButtonDefault {
                    id: btnConnectZM
                    enabled: zmUrl.text.length > 0
                    text: doConnect ? qsTr("Connect") : qsTr("Cancel");
                    KeyNavigation.right: btnStartView
                    KeyNavigation.down: searchView
                    icon: FontAwesome.icons.fa_external_link_square
                    class_name: "positive medium"

                    onClicked: {
                        if (doConnect) {
                            prefs.url = zmUrl.text
                            zmc.getMonitors()
                            zmClientError.visible = false
                            doConnect = false
                            wantMonitors = true
                        } else {
                            zmc.cancel()
                        }
                    }

                    Keys.onEnterPressed: {
                        btnConnectZM.clicked()
                    }
                }

                ButtonDefault {
                    id: btnStartView
                    checkable: false
                    enabled: tilemodel.numeratedTilesCount > 0
                    text: qsTr("Start")
                    icon: FontAwesome.icons.fa_play
                    class_name: "balanced medium"
                    KeyNavigation.right: btnFullScreen
                    KeyNavigation.down: searchView

                    onClicked: {
                        // ping ZM to verify we have an access
                        wantStart = true
                        zmc.getVersion()
                    }

                    Keys.onEnterPressed: {
                        btnStartView.clicked()
                    }
                }

                ButtonDefault {
                    id: btnFullScreen
                    anchors.margins: 10
                    x: 10
                    //visible: platform.name !== "android"
                    enabled: platform.name !== "android"
                    checkable: true
                    checked: prefs.fullScreen
                    text: qsTr("Screen")
                    class_name: "balanced medium"
                    icon: FontAwesome.icons.fa_expand
                    KeyNavigation.right: btnExit
                    KeyNavigation.down: searchView
                }

                ButtonDefault {
                    id: btnExit
                    text: qsTr("Quit")
                    class_name: "assertive medium"
                    icon: FontAwesome.icons.fa_sign_out
                    KeyNavigation.down: searchView

                    onClicked: {
                        wnd.close()
                    }

                    Keys.onEnterPressed: {
                        btnExit.clicked()
                    }
                }
            }

            ProgressBar {
                id: zmUrlProgress
                visible: !doConnect
                width: parent.width
                height: 8
                indeterminate: true

                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: 8
                    color: "#e6e6e6"
                    radius: 3
                }

                palette.dark: ColorsHelper.color.positive
            }

            Label {
                id: zmClientError
                font.pixelSize: StyleHelperItem.item_font_size
                text: ""
                visible: false
            }
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            wnd.close()
        }
    }

    Dialog {
        id: loginDialog        
        anchors.centerIn: parent
        modal: true
        //standardButtons: Dialog.Ok | Dialog.Cancel

        Column {
            spacing: 10

            Text {
                id: dlgTitle
                text: qsTr("ZM requires authentication")
                color: itemRoot.style.text
                font.pixelSize: StyleHelperItem.item_font_size
                wrapMode: Text.WordWrap
            }

            TextField {
                id: zmLogin
                focus: true
                anchors.margins: 10
                height: btnConnectZM.height
                placeholderText: qsTr("login")                
                KeyNavigation.down: zmPassword
                font.pixelSize: StyleHelperItem.item_font_size
                ToolTip.delay: StyleHelperItem.item_tooltip_delay
                ToolTip.timeout: StyleHelperItem.item_tooltip_timeout
                ToolTip.visible: activeFocus
                ToolTip.text: qsTr("Enter ZM host url here with http or https and click connect")
                //text: zmc.username
            }

            TextField {
                id: zmPassword
                focus: false
                anchors.margins: 10
                height: btnConnectZM.height
                placeholderText: qsTr("password")
                KeyNavigation.up: zmLogin
                KeyNavigation.down: btnLoginOk
                font.pixelSize: StyleHelperItem.item_font_size
                ToolTip.delay: StyleHelperItem.item_tooltip_delay
                ToolTip.timeout: StyleHelperItem.item_tooltip_timeout
                ToolTip.visible: activeFocus
                ToolTip.text: qsTr("Enter ZM host url here with http or https and click connect")
                passwordCharacter: "*"
                //text: zmc.password
                echoMode: TextInput.Password
            }

            Row {
                spacing: 10

                ButtonDefault {
                    id: btnLoginOk
                    enabled: zmLogin.text.length > 0 && zmPassword.text.length > 0
                    text: qsTr("Ok")
                    KeyNavigation.right: btnLoginCancel
                    KeyNavigation.up: zmPassword
                    icon: FontAwesome.icons.fa_external_link_square
                    class_name: "positive medium"

                    onClicked: {
                        zmc.setCredentials(zmLogin.text, zmPassword.text)
                        zmClientError.color = ColorsHelper.color.balanced
                        zmClientError.text = qsTr("Please repeat")
                        zmClientError.visible = true
                        loginDialog.close()
                        if (wantMonitors) {
                            btnConnectZM.clicked()
                        }

                        if (wantStart) {
                            btnStartView.clicked()
                        }

                        wantMonitors = false
                        wantStart = false
                    }

                    Keys.onEnterPressed: {
                        loginDialog.close()
                        wantMonitors = false
                        wantStart = false
                    }
                }

                ButtonDefault {
                    id: btnLoginCancel
                    checkable: false
                    enabled: true
                    text: qsTr("Cancel")
                    icon: FontAwesome.icons.fa_play
                    class_name: "balanced medium"
                    KeyNavigation.left: btnLoginOk
                    KeyNavigation.up: zmPassword

                    onClicked: {
                        loginDialog.close()
                        wantMonitors = false
                        wantStart = false
                    }

                    Keys.onEnterPressed: {
                        loginDialog.close()
                        wantMonitors = false
                        wantStart = false
                    }
                }
            }
        }
    }
}
