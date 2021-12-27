import QtQuick 2.12
import QtQuick.Window 2.1
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12

import NetworksModel 0.1
import ZMSearch 0.1
import TileModel 0.1
import ZMSQPreferences 0.1
import SceneBuilder 0.1

import "zmsq"

Window {
    id: wnd
    width: 800
    height: 640
    visible: true
    property int setupViewHeight: 140
    property int searchViewHeight: 200
    property int tilesViewHeight: 520
    property int tilesViewHRHeight: 520
    property int startingY: 80

    visibility: setupView.fullScreen ? Qt.WindowFullScreen : Qt.WindowMinimized
    flags: setupView.fullScreen ? (Qt.FramelessWindowHint | Qt.Window) : Qt.Window

    Component.onCompleted: {
        console.log("wnd completed full screen: " + setupView.fullScreen)
        netmon.refresh()
        console.log("netmon refreshed")
    }

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
}
