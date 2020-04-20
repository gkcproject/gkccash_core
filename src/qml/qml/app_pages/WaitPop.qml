import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.10

Window{
    id:root
    flags: Qt.FramelessWindowHint
    modality: Qt.WindowModal
    x: (Screen.width - width) / 2
    y: (Screen.height - height) / 2
    width:180
    height:180
    color:"transparent"


    onVisibleChanged: {
        if(visible){
            x = (Screen.width - width) / 2
            y = (Screen.height - height) / 2
        }

        if(visible){
            openWindow.start()
            dot_timer.start()
        }
    }



    Rectangle{
        id:background
        anchors.fill: parent
        anchors.leftMargin:20
        anchors.rightMargin: 20
        anchors.topMargin: 20
        anchors.bottomMargin:20
        radius:10
        opacity: 0
        color:"#202020"

        MouseArea{
            anchors.fill: parent
            onClicked: {closeWindow.start(); close_timer.start();dot_timer.stop()}
        }
    }


    Label{
        anchors.centerIn: background
        anchors.verticalCenterOffset: -10
        wrapMode: Label.WordWrap
        font.letterSpacing: 2
        text:qsTr("Loading")
        font.weight: Font.Normal
        font.pixelSize: 18
        color: "#FFFFFFFF"
    }

    Rectangle{
        id:left_dot
        anchors.centerIn: background
        anchors.verticalCenterOffset: 30
        anchors.horizontalCenterOffset: -20
        color: "#FFFFFFFF"
        radius: 4
        width:8
        height:8
        property int seq: 0
    }
    Rectangle{
        id:middle_dot
        anchors.centerIn: background
        anchors.verticalCenterOffset: 30
        color: "#9BFFFFFF"
        radius: 4
        width:8
        height:8
    }
    Rectangle{
        id:right_dot
        anchors.centerIn: background
        anchors.verticalCenterOffset: 30
        anchors.horizontalCenterOffset: 20
        color: "#5BFFFFFF"
        radius: 4
        width:8
        height:8
    }


    PropertyAnimation {id: openWindow; target: background; properties: "opacity"; to: 0.8; duration: 200}

    PropertyAnimation {id: closeWindow; target: background; properties: "opacity"; to: 0; duration: 300}

    Timer{
        id:close_timer
        triggeredOnStart: false
        interval: 300
        onTriggered: root.close()
    }

    Timer{
        id:dot_timer
        triggeredOnStart: false
        repeat: true
        interval: 150
        onTriggered: {


            if(left_dot.seq==0){
                left_dot.seq=1
                console.log("dsfdsf")
                left_dot.color="#5BFFFFFF"
                middle_dot.color="#FFFFFFFF"
                right_dot.color="#9BFFFFFF"
            }
            else if(left_dot.seq==1){
                left_dot.seq=2
                left_dot.color="#9BFFFFFF"
                middle_dot.color="#5BFFFFFF"
                right_dot.color="#FFFFFFFF"
            }
            else if(left_dot.seq==2){
                left_dot.seq=0
                left_dot.color="#FFFFFFFF"
                middle_dot.color="#9BFFFFFF"
                right_dot.color="#5BFFFFFF"

            }

        }
    }


}

