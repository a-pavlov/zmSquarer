import QtQuick 2.7
import QtQuick.Controls 2.4
import QtMultimedia 5.0
import CamVideoProducer 0.1

Item {
    id: square_hi_res
    property string url_hi_res: ""

    anchors.fill: parent
    CamVideoProducer {
        id: videoProducer_1
        url: url_hi_res
    }

    VideoOutput {
        id: output1
        anchors.fill: parent
        source: videoProducer_1

        Button {
            id: button
            visible: true

            background: Image {
                mipmap: true
                source: "qrc:/images/delete.png"
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

            width: 16
            height: 16
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.bottom: parent.verticalCenter
            anchors.topMargin: 16
            onClicked: {
                square_hi_res.visible = false;
                square_hi_res.destroy();
                square_hi_res.parent.showOutputs();
            }
        }
    }
}
