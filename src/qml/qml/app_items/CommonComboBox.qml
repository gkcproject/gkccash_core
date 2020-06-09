import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

CommonButton
{
    id:control
    color: "#3B4674"
    textColor:"#FFFFFF"
    border.color: "#D5D9E8"
    border.width: 1
    width: 75
    height: 32
    radius: 3
    label.anchors.horizontalCenter: undefined
    label.font.weight: Font.Medium
    label.leftPadding: 12
    textSize:13
    letterSpacing:0.355
    property alias model: listview.model
    property alias listview: listview

    signal currentIndexChanged(int index)
    signal currentStringChanged(string index)
    property int currentIndex_: -1

    Component.onCompleted:
    {
        if(listview.count>0)
            text = listview.contentItem.children[0].item_label.text
    }
	
    onClicked:
    {
        if(listview.count>0)
            popuper.open()
    }


    Image
    {
        id:drop_img
        source: "qrc:/images/icons/drop_gray.png"
        fillMode: Image.PreserveAspectFit
        anchors.right:parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        width:10
        height:5

    }

    Popup {
        id:popuper
        y: control.height-2
        x:0
        width: control.width
        padding: 0
        contentItem: ListView {
            id:listview
            clip: true
            width:control.width
            implicitHeight: contentHeight>200?200:contentHeight
            implicitWidth: control.width
            boundsBehavior:Flickable.StopAtBounds

            //model: control.popup.visible ? control.model : null
            currentIndex: 0

            delegate:ItemDelegate {
                width: control.width
                height:22
                property alias item_label: item_label
                Label
                {
                    id:item_label
                    anchors.fill: parent
                    text: modelData
                    font.weight: Font.Medium
                    font.pixelSize:11
                    font.letterSpacing: 0.355
                    verticalAlignment: Label.AlignVCenter
                    horizontalAlignment: Label.AlignLeft
                    leftPadding: 10

                }

                background: Rectangle
                {
                    color:listview.currentIndex == index?"#8B469AAC":(hovered?"#D3D3D3":"#FFFFFF")
                }


                MouseArea{
                    anchors.fill:parent
                    hoverEnabled: true
                    onClicked: {
                        control.text = item_label.text
                        listview.currentIndex = index
                        control.currentIndexChanged(index)
                        control.currentStringChanged(control.text)
                        popuper.close()
                    }



                }

            }


            ScrollBar.vertical: MyScrollBar {
                id: scrollBar
                width:6
                height:30

                anchors.right: listview.right
                handle.implicitHeight: 100

                policy:ScrollBar.AsNeeded
                visible: listview.count * 22 > listview.height
            }
        }
    }


}
