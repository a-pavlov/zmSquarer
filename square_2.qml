import QtQuick 2.0
import QtMultimedia 5.0
import CamVideoProducer 0.1

Item {
    property string url_1: ""
    property string url_2: ""
    property string url_3: ""
    property string url_4: ""

    anchors.fill: parent

    CamVideoProducer {
        id: videoProducer_1
        url: url_1
    }

    CamVideoProducer {
        id: videoProducer_2
        url: url_2
    }

    VideoOutput {
        id: output1
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width/2
        height: parent.height
        source: videoProducer_1
    }

    VideoOutput {
        id: output2
        anchors.left: output1.right
        anchors.top: parent.top
        width: parent.width/2
        height: parent.height
        source: videoProducer_2
    }
}
