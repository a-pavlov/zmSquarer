import QtQuick 2.2
import QtQuick.Window 2.1
import QtMultimedia 5.0
import QtQuick.Layouts 1.11
import DesktopVideoProducer 0.1

Window {
    visible: true
    //width: 1920
    //height: 1080
    width: 480
    height: 320


    //DesktopVideoProducer {
    //    id: videoProducer
    //    url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    //}
/*
    DesktopVideoProducer {
        id: videoProducer2
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=2&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }

    DesktopVideoProducer {
        id: videoProducer3
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=3&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }

    DesktopVideoProducer {
        id: videoProducer7
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=7&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }

    DesktopVideoProducer {
        id: videoProducer8
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=8&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }

    DesktopVideoProducer {
        id: videoProducer9
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=9&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }


    DesktopVideoProducer {
        id: videoProducer22
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=22&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }

    DesktopVideoProducer {
        id: videoProducer23
        url: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=23&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
    }
*/

    /*MediaPlayer {
        id: mp        
        //source: "/home/inkpot/Downloads/BigBuckBunny_512kb.mp4"
        source: "http://192.168.100.12/zm/cgi-bin/nph-zms?mode=jpeg&monitor=1&scale=100&maxfps=30&buffer=1000&user=admin&pass=root"
        autoPlay: true
        autoLoad: true
    }*/

    //GridLayout {
    //    id: grid
    //    columns: 3

        /*VideoOutput {
            id: output1
            //anchors.fill: parent
            anchors.left: parent
            width: parent.width/3
            height:parent.height/3
            source: videoProducer
        }*/
/*
        VideoOutput {
            id: output2
            anchors.top: output1.top
            anchors.left: output1.right
            width: parent.width/3
            height:parent.height/3
            source: videoProducer2
        }

        VideoOutput {
            id: output3
            anchors.top: output2.top
            anchors.left: output2.right
            width: parent.width/3
            height:parent.height/3
            source: videoProducer3
        }

        VideoOutput {
            id: output7
            anchors.top: output1.bottom
            anchors.left: output1.left
            width: parent.width/3
            height:parent.height/3
            source: videoProducer7
        }

        VideoOutput {
            id: output8
            anchors.top: output2.bottom
            anchors.left: output2.left
            width: parent.width/3
            height:parent.height/3
            source: videoProducer8
        }

        VideoOutput {
            id: output9
            anchors.top: output3.bottom
            anchors.left: output3.left
            width: parent.width/3
            height:parent.height/3
            source: videoProducer9
        }

        VideoOutput {
            id: output22
            anchors.top: output7.bottom
            anchors.left: output7.left
            width: parent.width/3
            height:parent.height/3
            source: videoProducer22
        }

        VideoOutput {
            id: output23
            anchors.top: output8.bottom
            anchors.left: output8.left
            width: parent.width/3
            height:parent.height/3
            source: videoProducer23
        }*/
    //}
}
