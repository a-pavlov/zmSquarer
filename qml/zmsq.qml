import QtQuick 2.12
import QtQuick.Window 2.1
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12

import NetworksModel 0.1
import ZMSearch 0.1
import TileModel 0.1
import ZMSQPreferences 0.1
import SceneBuilder 0.1

import QtMultimedia 5.0
import CamVideoProducer 0.1

import "qrc:/qml/zmsq/variables/fontawesome.js" as FontAwesome

import "zmsq"

ApplicationWindow {
    id: wnd
    width: 800
    height: 640
    visible: true
    property int setupViewHeight: 140
    property int searchViewHeight: 200
    property int tilesViewHeight: 520
    property int tilesViewHRHeight: 520
    property int startingY: 80

    property int base_radius: 4
    property int base_margins: 4

    property var camsView: ""
    property string hiResUrl: ""

    visibility: setupView.fullScreen ? Qt.WindowFullScreen : Qt.WindowMinimized
    flags: setupView.fullScreen ? (Qt.FramelessWindowHint | Qt.Window) : Qt.Window

    FontLoader{ source: "qrc:/qml/fonts/fontawesome-webfont.ttf"}

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

    SceneBuilder {
        id: sceneBuilder
        onSuccess: {
            mainView.state = "showCameras"
            camsView = Qt.createQmlObject(code, wnd, "scene1")
            camsView.focus = true
        }

        onFail: {
            zmClientError.text = qsTr(
                        "<font color=\"#FF0000\">Error: %1</font> ").arg(code)
            zmClientError.visible = true
        }
    }

    onClosing: {
        tilemodel.save()
        prefs.fullScreen = setupView.fullScreen
        prefs.httpsEnabled = searchView.httpsEnabled
        prefs.flush()
    }

    Component.onCompleted: {
        netmon.refresh()
        tilemodel.load()
    }

    function showStartScreen() {
        camsView.destroy()
        tilesView.forceActiveFocus()
    }

    function runHiRes() {
        hiResLoader.active = true
        camsView.visible = false
        hiResLoader.focus = true
        hiResLoader.forceActiveFocus()
    }

    Rectangle {
        id: window
        width: 700
        height: parent.height
        anchors.centerIn: parent
        //color: "#3E606F"

        FocusScope {
            id: mainView

            width: parent.width; height: parent.height
            focus: true

            SetupView {
                id: setupView
                y: startingY
                width: parent.width;
                height: setupViewHeight
                focus: true
                activeFocusOnTab: true
            }

            SearchView {
                id: searchView
                y: startingY + setupViewHeight
                width: parent.width;
                height: searchViewHeight
                activeFocusOnTab: true
            }

            TilesView {
                id: tilesView
                y: startingY + setupViewHeight + searchViewHeight
                width: cellWidth*4 + 100
                height: tilesViewHeight
                activeFocusOnTab: true
            }

            TilesViewHR {
                id: tilesViewHR
                y: startingY + setupViewHeight + searchViewHeight + tilesViewHeight
                width: cellWidth*4 + 100
                height: tilesViewHeight
                activeFocusOnTab: true
            }

            Rectangle {
                id: shade
                anchors.fill: parent
                color: "black"
                opacity: 0
            }

            states:  [
                State {
                    name: "showSetupView"
                    PropertyChanges { target: setupView; y:  startingY }
                    PropertyChanges { target: searchView; y: startingY + setupViewHeight }
                    PropertyChanges { target: tilesView; y: startingY + setupViewHeight + searchViewHeight }
                    PropertyChanges { target: tilesViewHR; y: startingY + setupViewHeight + searchViewHeight + tilesViewHeight }
                },

                State {
                    name: "showSearchView"
                    PropertyChanges { target: setupView; y:  startingY - setupViewHeight }
                    PropertyChanges { target: searchView; y: startingY }
                    PropertyChanges { target: tilesView; y: startingY + searchViewHeight }
                    PropertyChanges { target: tilesViewHR; y: startingY + searchViewHeight + tilesViewHeight }
                },

                State {
                    name: "showTilesView"
                    PropertyChanges { target: setupView; y:  -setupViewHeight - searchViewHeight - 100 }
                    PropertyChanges { target: searchView; y: -searchViewHeight - 100 }
                    PropertyChanges { target: tilesView; y: startingY }
                    PropertyChanges { target: tilesViewHR; y: startingY + tilesViewHeight }
                },

                State {
                    name: "showTilesViewsHR"
                    PropertyChanges { target: setupView; y: -setupViewHeight - searchViewHeight - tilesViewHeight }
                    PropertyChanges { target: searchView; y: -searchViewHeight - tilesViewHeight }
                    PropertyChanges { target: tilesView; y: -tilesViewHeight }
                    PropertyChanges { target: tilesViewHR; y: startingY }
                },

                State {
                    name: "showCameras"
                    PropertyChanges { target: setupView; y: -setupViewHeight - searchViewHeight - tilesViewHeight - tilesViewHRHeight }
                    PropertyChanges { target: searchView; y: -searchViewHeight - tilesViewHeight - tilesViewHRHeight }
                    PropertyChanges { target: tilesView; y: -tilesViewHeight - tilesViewHRHeight }
                    PropertyChanges { target: tilesViewHR; y: -tilesViewHRHeight }
                }
            ]

            transitions: Transition {
                NumberAnimation { properties: "y"; duration: 600; easing.type: Easing.OutQuint }
            }
        }

        transitions: Transition {
            NumberAnimation { properties: "x,opacity"; duration: 600; easing.type: Easing.OutQuint }
        }
    }

    // HI resolution view
    Loader {
        id: hiResLoader
        active: false
        anchors.fill: parent
        sourceComponent: hiResView
        focus: true
    }

    FocusScope {
        id: hiResScope
        anchors.fill: parent

        Component {
            id: hiResView
            Rectangle {
                id: hiResRect

                CamVideoProducer {
                    id: videoProducer_1
                    url: hiResUrl
                }

                VideoOutput {
                    id: output1
                    anchors.fill: parent
                    source: videoProducer_1

                    ButtonDefault {
                        id: backBtn
                        focus: true
                        x: output1.contentRect.x + output1.contentRect.width - width - base_margins*2
                        y: output1.contentRect.y + height + base_margins

                        //anchors {
                        //    margins: base_margins*2
                        //    right: parent.right
                        //    top: parent.top
                        //}

                        class_name: "outline small"
                        icon: FontAwesome.icons.fa_arrow_left

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
                    }
                }
            }
        }
    }
}
