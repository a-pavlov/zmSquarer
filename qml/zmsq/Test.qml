import QtQuick 2.12
import QtQuick.Window 2.1
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12

Window {
    height: 300
    width: 300
    color: "white"

    FontLoader{ source: "/home/inkpot/dev/zmSquarer/qml/zmsq/fontawesome-webfont.ttf"}

    Row {

        ButtonDefault {
            id: test
            text: "test1"
            focus: true
            icon: FontAwesome.icons.fa_search
            class_name: "outline small"
            //KeyNavigation.left: test
            onClicked: {
                console.log("test 1 clicked")
            }
        }

        ButtonDefault {
            id: test2
            text: "test2"
            icon: FontAwesome.icons.fa_search
            class_name: "outline small"
            KeyNavigation.left: test
            onClicked: {
                console.log("test 2 clicked")
            }
        }

        ButtonDefault {
            id: test3
            text: "test3"
            icon: FontAwesome.icons.fa_search
            KeyNavigation.left: test2
            class_name: "outline small"
            onClicked: {
                console.log("test 3 clicked")
            }
        }
    }

    Component.onCompleted: {
        console.log("w " + test.width);
    }

}