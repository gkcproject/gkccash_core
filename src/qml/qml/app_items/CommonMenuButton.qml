import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

CommonButton
{
    id:control
    color: opened?"#0E3549":(walletModel.isTestNet()?"#202020":"#212b50")
    textColor:"#FFFFFF"
    width: 78
    height: 30
    radius: 3
    label.font.weight: Font.Normal
    textSize:16
    letterSpacing:0.2
    property alias model: listview.model
    property int contentWidth:100
    property bool opened: false

    signal menu_triggered(int index)

    onClicked:
    {
        opened = true
        popuper.open()
    }

    function close_popup()
    {
        opened = false
        popuper.close()
    }


    Popup {
        id:popuper
        y: control.height-2
        x: 0
        padding: 0
        contentItem: ListView {
            id:listview
            clip: true
            width:control.width
            implicitHeight: contentHeight
            implicitWidth: control.contentWidth
            boundsBehavior:Flickable.StopAtBounds


            //model: control.popup.visible ? control.model : null
            currentIndex: 0

            delegate:MenuItem {
                width: control.contentWidth
                height:26
                Label
                {
                    id:item_label
                    anchors.fill: parent
                    text: modelData
                    font.weight: Font.Medium
                    font.pixelSize:12
                    font.letterSpacing: 0.355
                    verticalAlignment: Label.AlignVCenter
                    horizontalAlignment: Label.AlignLeft
                    leftPadding: 10

                }

                background: Rectangle
                {
                    color:hovered?"#D3D3D3":"#FFFFFF"
                }


                MouseArea{
                    anchors.fill:parent
                    hoverEnabled: true
                    onClicked: {
                        menu_triggered(index)
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
                visible: listview.count * 26 > listview.height
            }
        }

        onClosed:
        {
            control.opened = false
        }
    }


}
