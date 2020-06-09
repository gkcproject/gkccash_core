import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.10
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtGraphicalEffects 1.0

import "../app_items"
import "../base_items"

Window{
    id:root

    width:500
    height:210

    flags: Qt.FramelessWindowHint
//    modality: Qt.WindowModal

    property string title
    property string content_text

    property string dialogType:"question"
    property alias confirm_btn_text: confrim_btn.text
    property alias cancel_btn_text: cancel_btn.text
    property alias background_rec: background
    property bool confrim_btn_visible: true
    property bool close_btnRect_visible: true
    property bool confrim_btn_enabled: true
    property bool cancel_btn_enabled: true
    property bool cancel_btn_visible: true
    property bool dim_back: false

    color: "white"
    signal closed()
    signal confirmed()
    signal canceled()


    Shortcut
    {

    }

    Connections
    {

    }


    Component.onCompleted:
    {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2-100
    }


    onVisibleChanged:{
//        if(visible){
//            x = (Screen.width - width) / 2
//            y = (Screen.height - height) / 2-100
//        }

        if(visible)
        {
            if(dim_back){
                //root_window_dim_switch = true
            }

            close_btnRect.color = Qt.rgba(0,0,0,0)
            openWindow.start()
            openWindow2.start()


        }
    }

    Rectangle{
        id:background
        anchors.fill: parent
        radius:3
        opacity: 1
        color: "#FFFFFF"
        scale:0.8

        MouseArea{
            anchors.fill: parent
            property var clickPos

            onPressed: {
                clickPos = { x: mouse.x, y: mouse.y }

            }
            onPositionChanged: {

                root.x = mousePosition.cursorPos().x - clickPos.x - background.x
                root.y = mousePosition.cursorPos().y - clickPos.y - background.y
            }

        }


        Behavior on scale {
            NumberAnimation {
                duration: 100
                easing.type: Easing.OutCubic
            }
        }


        Label
        {
            id:header_label
            anchors.top:parent.top
            height:30
            width:parent.width
            background: Rectangle
            {
                radius:3
                color: "#F0F0F0"
            }
            font.weight: Font.Medium
            font.pixelSize: 13
            leftPadding: 16
            font.letterSpacing: 0.355
            verticalAlignment: Label.AlignVCenter
            text:title
            color:"#333333"

            Rectangle
            {
                id: close_btnRect
                color: Qt.rgba(0,0,0,0)
                width:22
                height:22
                radius: 11
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                visible: close_btnRect_visible

                PropertyAnimation {id: animateColor_in; target: close_btnRect; properties: "color"; to: "#EE637F"; duration: 200}
                PropertyAnimation {id: animateColor_out; target: close_btnRect; properties: "color"; to: Qt.rgba(0,0,0,0); duration: 200}

                Image{
                    id:close_btn
                    source: "../../images/icons/dialog_close.png"
                    width:12
                    height:12
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled:true

                    onClicked: {
                        close_dialog()
                    }

                    onEntered: {
                        animateColor_in.start()
                    }

                    onExited: {
                        animateColor_out.start()
                    }
                }
            }

        }


        Label
        {
            anchors.top:header_label.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: confrim_btn.top
            anchors.margins: 15
            font.weight: Font.Normal
            font.pixelSize: 13
            font.letterSpacing: 0.355
            wrapMode: Label.WordWrap
            text:content_text
            color:"#333333"
            textFormat: Text.RichText

        }

        CommonButton
        {
            id:confrim_btn
            color: enabled?"#EE637F":"#88EE637F"
            anchors.right: cancel_btn.visible?cancel_btn.left:parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: cancel_btn.verticalCenter
            text:"Confirm"
            visible: confrim_btn_visible
            enabled: confrim_btn_enabled

            onClicked: {
                confirmed()
                //close_dialog()
            }
        }




        CommonButton
        {
            id:cancel_btn
            color: "#469AAC"
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            text:qsTr("Cancel")
            visible: cancel_btn_visible
            enabled: cancel_btn_enabled


            onClicked: {
                canceled()
                close_dialog()
            }
        }


    }


    PropertyAnimation {id: openWindow; target: root; properties: "opacity"; to: 1; duration: 200}
    NumberAnimation {id: openWindow2; target: background; properties: "scale"; to: 1; duration: 200}

    PropertyAnimation {id: closeWindow; target: root; properties: "opacity"; to: 0; duration: 220}
    NumberAnimation {id: closeWindow2; target: background; properties: "scale"; to: 0.8; duration: 220}




    onClosing:
    {
        close_dialog()
    }

    Timer
    {
        id:close_timer
        triggeredOnStart: false
        interval: 220
        onTriggered: {
            root.close()
        }
    }

    function close_dialog()
    {
        if(!close_timer.running)
        {
            closeWindow.start()
            closeWindow2.start()
            close_timer.start()
            if(dim_back){
                //root_window_dim_switch = false
            }
        }

    }


}

