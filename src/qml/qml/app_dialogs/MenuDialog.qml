import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0

import "../app_items"


Window {
    id:root
    height: listView.contentHeight>120?120:listView.contentHeight
    width: listView.contentWidth

    flags: Qt.Popup | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint


    property string text
    property alias model:listView.model

    onActiveChanged:
    {
        console.log("active:"+active)
    }


    ListView {
        id:listView
        anchors.fill: parent
        clip: true
        width:contentWidth

        boundsBehavior:Flickable.StopAtBounds

        Component.onCompleted:
            console.log("listView.width:"+listView.contentWidth)

        delegate:MenuItem {
            width: item_label.width
            height:22
            property variant myData: model
            Label
            {
                id:item_label
                anchors.fill: parent
                text: modelData
                font.weight: Font.Medium
                width: contentWidth+20

                font.pixelSize:11
                font.letterSpacing: 0.355
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignLeft
                leftPadding: 10




            }

            background: Rectangle
            {
                color:listView.currentIndex == index?"#8B469AAC":(hovered?"#D3D3D3":"#FFFFFF")
            }


            MouseArea{
                anchors.fill:parent
                hoverEnabled: true
                onClicked: {
                    root.text = item_label.text
                    listView.currentIndex = index
                    root.close()
                }


            }

        }


        ScrollBar.vertical: MyScrollBar {
            id: scrollBar
            width:6
            height:30

            anchors.right: listView.right
            handle.implicitHeight: 80

            policy:ScrollBar.AsNeeded
            visible: listView.count * 22 > listView.height
        }




    }

}
