import QtQuick 2.0
import QtMultimedia 5.0
import CamVideoProducer 0.1

Item {
    id: square_1
    property string url_1: ""
    property string url_2: ""
    property string url_3: ""
    property string url_4: ""

    anchors.fill: parent
    CamVideoProducer {
        id: videoProducer_1
        url: url_1
    }

    VideoOutput {
        id: output1
        anchors.fill: parent
        source: videoProducer_1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("mouse click")
                square_1.destroy();
            }
        }

        Rectangle {
            anchors.top: parent.top
            anchors.centerIn: parent
            width: 100; height: 100
            color: "green"

            MouseArea {
                anchors.fill: parent
                onClicked: { parent.color = 'red' }
            }
        }
        //onDestroyed: videoProducer_1.destroy();
    }

    //Component.onCompleted: {
    //    console.log("completed " + url_1)
    //}
}
