import QtQuick 2.12
import QtQuick.Window 2.1
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12

Window {
    height: 300
    width: 300
    color: "white"

    FontLoader{ source: "/home/inkpot/dev/zmSquarer/qml/zmsq/fontawesome-webfont.ttf"}

    ButtonDefault {
        id: test
        y: 20
        x: 20
        
        
        text: "back"        
        icon: FontAwesome.icons.fa_search
        class_name: "outline small"
    }

    Component.onCompleted: {
        console.log("w " + test.width);
    }

}