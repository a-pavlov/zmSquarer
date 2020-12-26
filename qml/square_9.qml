import QtQuick 2.0
import QtMultimedia 5.0
import CamVideoProducer 0.1

Item {
    property string url_1: ""
    property string url_2: ""
    property string url_3: ""
    property string url_4: ""
    property string url_5: ""
    property string url_6: ""
    property string url_7: ""
    property string url_8: ""
    property string url_9: ""


    anchors.fill: parent

    CamVideoProducer {
        id: videoProducer_1
        url: url_1
    }

    CamVideoProducer {
        id: videoProducer_2
        url: url_2
    }

    CamVideoProducer {
        id: videoProducer_3
        url: url_3
    }

    CamVideoProducer {
        id: videoProducer_4
        url: url_4
    }

    CamVideoProducer {
        id: videoProducer_5
        url: url_5
    }

    CamVideoProducer {
        id: videoProducer_6
        url: url_6
    }

    CamVideoProducer {
        id: videoProducer_7
        url: url_7
    }

    CamVideoProducer {
        id: videoProducer_8
        url: url_8
    }

    CamVideoProducer {
        id: videoProducer_9
        url: url_9
    }

    VideoOutput {
        id: output1
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_1
    }

    VideoOutput {
        id: output2
        anchors.left: output1.right
        anchors.top: parent.top
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_2
    }

    VideoOutput {
        id: output3
        anchors.left: output2.right
        anchors.top: parent.top
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_3
    }

    VideoOutput {
        id: output4
        anchors.left: output1.left
        anchors.top: output1.bottom
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_4
    }

    VideoOutput {
        id: output5
        anchors.left: output2.left
        anchors.top: output2.bottom
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_5
    }

    VideoOutput {
        id: output6
        anchors.left: output3.left
        anchors.top: output3.bottom
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_6
    }

    VideoOutput {
        id: output7
        anchors.left: output4.left
        anchors.top: output4.bottom
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_7
    }

    VideoOutput {
        id: output8
        anchors.left: output5.left
        anchors.top: output5.bottom
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_8
    }

    VideoOutput {
        id: output9
        anchors.left: output6.left
        anchors.top: output6.bottom
        width: parent.width/3
        height: parent.height/3
        source: videoProducer_9
    }
}
