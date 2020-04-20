import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

CommonButton
{
    id:coin_type_btn
    color: "#469AAC"

    radius: 3
    label.anchors.horizontalCenter: undefined
    label.anchors.left:coin_type_btn.left
    label.width: 50
    text:"GKC"
    textSize:11
    letterSpacing:0
    property bool spacer_visibility: false
    property bool opened: popuper.opened
    property alias index: listview.currentIndex
	property string uint:qsTr("GKC")
    property bool showUp: false
    onClicked:
    {
        popuper.open()
    }
    Rectangle
    {
        color: "#E6E6E6"
        width: 1
        height:8
        anchors.verticalCenter: parent.verticalCenter
        anchors.right:drop_img.left
        anchors.rightMargin: 10
        visible:spacer_visibility
    }

    Image
    {
        id:drop_img
        source: "qrc:/images/icons/drop.png"
        fillMode: Image.PreserveAspectFit
        anchors.right:parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        width:10
        height:5

    }

    Popup {
        id:popuper
        y: showUp?(0-listview.contentHeight):(coin_type_btn.height-2)
        x:0
        width: coin_type_btn.width
        padding: 0
        contentItem: ListView {
            id:listview
            clip: true
            width:coin_type_btn.width
            implicitHeight: contentHeight
            implicitWidth: coin_type_btn.width
            boundsBehavior:Flickable.StopAtBounds

            //model: coin_type_btn.popup.visible ? coin_type_btn.model : null
            currentIndex: 0


            model: ListModel {

                ListElement
                {
                    modelData: "GKC";
                }
                ListElement
                {
                    modelData: "mGKC";
                }
                ListElement
                {
                    modelData: "μGKC";
                }
            }

            delegate:MenuItem {
                width: coin_type_btn.width
                height:22
                Label
                {
                    id:item_label
                    anchors.fill: parent
                    text: modelData
                    font.weight: Font.Light
                    font.pixelSize:11
                    color: "#333333"
                    verticalAlignment: Label.AlignVCenter
                    horizontalAlignment: Label.AlignHCenter

                }

                background: Rectangle
                {
                    color:listview.currentIndex == index?"#8B469AAC":(hovered?"#D3D3D3":"#FFFFFF")
                }


                MouseArea{
                    anchors.fill:parent
                    hoverEnabled: true
                    onClicked: {
						uint = item_label.text
                        coin_type_btn.text = item_label.text
                        listview.currentIndex = index
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
