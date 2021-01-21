import QtQuick 2.0
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
    }

    Button {
        id: back
        text: qsTr("Back")
        onClicked: {
            console.log("hi res component initiated shutdown")
            square_hi_res.visible = false;
            square_hi_res.destroy();
            square_hi_res.parent.showOutputs();
        }
    }
}
