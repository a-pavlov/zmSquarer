import QtQuick 2.0


Component {
    ListModel {
        id: hosts
        ListElement {
            network: "192.168.100.1"
            selected: false
        }

        ListElement {
            network: "10.10.1.1"
            selected: true
        }

        ListElement {
            network: "192.168.1.1"
            selected: false
        }
    }

    ListModel {
        id: zmhosts
        ListElement {
            network: "192.168.100.1"
            version: "1.22"
            apiVersion: "2.33"
            selected: false
        }

        ListElement {
            network: "10.10.1.1"
            version: "1.22"
            apiVersion: "2.33"
            selected: false
        }
    }

    ListModel {
        id: cm
        ListElement {
            cam_id: "1"
            cam_name: "Camera 1 House"
            active: true
            resolution: "1920x1080"
            cam: true
        }

        ListElement {
            cam_id: "2"
            cam_name: "Camera 2 House"
            active: true
            resolution: "1920x1080"
            cam: true
        }

        ListElement {
            cam_id: "3"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "4"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "5"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "6"
            cam_name: "Back yard camera"
            active: false
            resolution: "800x600"
            cam: true
        }

        ListElement {
            cam_id: "7"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "18"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "9"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "14"
            cam_name: "Super puper camera view"
            active: true
            resolution: "320x240"
            cam: true
        }

        ListElement {
            cam_id: "21"
            cam_name: "Super puper camera view 2"
            active: false
            resolution: "1024x768"
            cam: true
        }

        ListElement {
            cam_id: "11"
            cam_name: "Super puper camera view 3"
            active: true
            resolution: "1024x768"
            cam: true
        }

        ListElement {
            cam_id: "10"
            cam_name: "Not assigned"
            active: false
            resolution: "N/a"
            cam: false
        }

        ListElement {
            cam_id: "31"
            cam_name: "Super puper camera view"
            active: true
            resolution: "320x240"
            cam: true
        }

        ListElement {
            cam_id: "14"
            cam_name: "Super puper camera view 2"
            active: false
            resolution: "1024x768"
            cam: true
        }

        ListElement {
            cam_id: "81"
            cam_name: "Super puper camera view 3"
            active: true
            resolution: "1024x768"
            cam: true
        }
    }
}
