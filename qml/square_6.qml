import QtQuick 2.0
import QtMultimedia 5.0
import CamVideoProducer 0.1

Item {
    id: main
    anchors.fill: parent
    property string url_1: ""
    property string url_2: ""
    property string url_3: ""
    property string url_4: ""
    property string url_5: ""
    property string url_6: ""

    property string hires_url_1: ""
    property string hires_url_2: ""
    property string hires_url_3: ""
    property string hires_url_4: ""
    property string hires_url_5: ""
    property string hires_url_6: ""
    focus: true

    function showOutputs() {
        output1.visible = true
        output2.visible = true
        output3.visible = true
        output4.visible = true
        output5.visible = true
        output6.visible = true
    }

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

    VideoOutput {
        id: output1
        anchors.left: parent.left
        anchors.top: parent.top
        width: parent.width/3
        height: parent.height/2
        source: videoProducer_1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("producer 1 clicked, start " + hires_url_1)
                var component = Qt.createComponent("square_hi_res.qml");
                if (component.status === Component.Ready) {
                    var item = component.createObject(main);
                    item.url_hi_res = hires_url_1;

                    output1.visible = false
                    output2.visible = false
                    output3.visible = false
                    output4.visible = false
                    output5.visible = false
                    output6.visible = false
                }
            }
        }

        onVisibleChanged: {
            if (visible) {
                console.log("visible")
            } else {
                console.log("invisible")
            }

            videoProducer_1.updateTimer(visible)
        }
    }

    VideoOutput {
        id: output2
        anchors.left: output1.right
        anchors.top: parent.top
        width: parent.width/3
        height: parent.height/2
        source: videoProducer_2
    }

    VideoOutput {
        id: output3
        anchors.left: output2.right
        anchors.top: parent.top
        width: parent.width/3
        height: parent.height/2
        source: videoProducer_3
    }

    VideoOutput {
        id: output4
        anchors.left: parent.left
        anchors.top: output1.bottom
        width: parent.width/3
        height: parent.height/2
        source: videoProducer_4
    }

    VideoOutput {
        id: output5
        anchors.left: output4.right
        anchors.top: output2.bottom
        width: parent.width/3
        height: parent.height/2
        source: videoProducer_5
    }

    VideoOutput {
        id: output6
        anchors.left: output5.right
        anchors.top: output3.bottom
        width: parent.width/3
        height: parent.height/2
        source: videoProducer_6
    }
}
