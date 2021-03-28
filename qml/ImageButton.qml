import QtQuick 2.7
import QtQuick.Controls 2.3

Button {
    id: button
    property string image: ""

    background: Image { 
        mipmap: true
        source: image
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
}