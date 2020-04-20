import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3


Popup {
    id:root
    implicitWidth: 50
    padding: 0
    property alias model: listview.model
    signal menuPicked(int index)
    property int itemHeight: 22


    contentItem: ListView {
        id:listview
        clip: true
        width:root.width
        implicitHeight: contentHeight>200?200:contentHeight
        implicitWidth: root.width
        boundsBehavior:Flickable.StopAtBounds

        currentIndex: -1

        delegate:MenuItem {
            width: root.width
            height:root.itemHeight
            Label
            {
                id:item_label
                anchors.fill: parent
                text: itemData
                font.weight: Font.Medium
                font.pixelSize:11
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
                    root.menuPicked(index)
                    root.close()
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
