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

    property string title
    property string msgTxt
    property bool dialogType:true
    property string msgType
    property alias background_rec: background
   
    property bool close_btnRect_visible: true
    
    property bool dim_back: false

    color: "white"
 

    Component.onCompleted:
    {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2-100
    }


    onVisibleChanged:{
        if(visible)
        {
            if(dim_back){
                root_window_dim_switch = true
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

	Image {
             id:entrustSuccess_image
             anchors.left: parent.left
             anchors.leftMargin:20
             anchors.top: parent.top
             anchors.topMargin:48
             fillMode: Image.PreserveAspectFit
             height:32
             cache : true
             asynchronous: true
             source: dialogType?"qrc:/images/icons/chengong.png":"qrc:/images/icons/shibai.png"
        }
        Label{
            font.weight: Font.Medium
            font.pixelSize:17
            anchors.verticalCenter: entrustSuccess_image.verticalCenter
            anchors.left: entrustSuccess_image.right
            anchors.leftMargin: 21
            color: "#5D77AB"
            text:msgType
        }

	 CommonTextArea
                {
                    id:detailArea
                    font.weight: Font.Light
                    font.pixelSize:16
		    anchors.left: parent.left
                    anchors.leftMargin:20
             	    anchors.top: entrustSuccess_image.bottom
                    anchors.topMargin:20                    
                    textFormat: Qt.RichText
                    readOnly: true
                    wrapMode: TextEdit.Wrap
                    text:msgTxt
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
                root_window_dim_switch = false
            }
        }
    }
}

