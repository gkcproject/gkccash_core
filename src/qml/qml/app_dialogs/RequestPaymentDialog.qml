import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0

import "../app_pages"
import "../app_items"
import "../base_items"

CommonDialog
{
    id:root
    modality: Qt.ApplicationModal
    width:500
    height: 700

    cancel_btn_text: qsTr("Cancel")
    confrim_btn_visible: false
    property int currentRow: -1

    property string tag
    property string address
    property alias info:messageArea.text
    property alias qRImg:qRImg

    title: (tag === undefined || tag === "")?("Request payment to " + address):("Request payment to "+tag)

    Item{
        parent:root.background_rec
        anchors.fill: parent

        Image{
            id:qRImg
            cache:false
            fillMode: Image.PreserveAspectFit
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 30
            width: 300
            height:300
            property bool pressed
            property bool down
            property bool hovered
            Ripple {
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                width: 22; height: 22

                z: -1
                anchor: qRImg
                pressed: qRImg.pressed
                active: qRImg.down || qRImg.visualFocus || qRImg.hovered
                color: qRImg.Material.rippleColor
            }

            MouseArea
            {
                id:mouseArea
                anchors.fill: parent
                hoverEnabled: true

                acceptedButtons: Qt.RightButton
                onClicked:
                {
                    if(!popuper.opened)
                        popuper.open()
                    else
                        popuper.close()
                }


            }

            CommonPopup
            {
                id:popuper

                width: 100
                itemHeight: 30

                onAboutToShow:
                {
                    x = mouseArea.mouseX
                    y = mouseArea.mouseY
                }

                model: ListModel
                {
                    ListElement {
                        itemData: qsTr("Save Image...")
                    }
                    ListElement {
                        itemData: qsTr("Copy Image")
                    }
                }

                onMenuPicked:
                {
                    if(index == 0)
                        walletModel.recentRequestsFilterProxy.saveImage()
                    else
                        walletModel.recentRequestsFilterProxy.copyImage()
                }

            }

        }



        CommonTextArea
        {
            id:messageArea
            font.weight: Font.Light
            font.pixelSize:13
            anchors.leftMargin: 10
            anchors.left: parent.left
            anchors.rightMargin:10
            anchors.right: parent.right
            anchors.top: qRImg.bottom
            anchors.topMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            textFormat: Qt.RichText
            readOnly: true

            wrapMode: TextEdit.Wrap

        }


        CommonButton
        {
            id:copyURIBtn
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            anchors.left: messageArea.left
            text: qsTr("Copy URI")
            color: "#469AAC"
			
			ToolTip
            {
                id:tooltip_URI_paste
                text:qsTr("URI Pasted")
            }
            onClicked:
			{
				walletModel.recentRequestsFilterProxy.copyURI()
				tooltip_URI_paste.visible = true
			}
        }

        CommonButton
        {
            id:copyAddressBtn
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            anchors.left: copyURIBtn.right
            anchors.leftMargin: 10
            text: qsTr("Copy Address")
            color: "#469AAC"
			
			ToolTip
            {
                id:tooltip_Address_paste
                text:qsTr("Address Pasted")
            }
			
            onClicked:
			{
				walletModel.recentRequestsFilterProxy.copyAddress()
				tooltip_Address_paste.visible = true
			}
        }

        CommonButton
        {
            id:saveImgBtn
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            anchors.left: copyAddressBtn.right
            anchors.leftMargin: 10
            text: qsTr("Save Image...")
            color: "#469AAC"

            onClicked:walletModel.recentRequestsFilterProxy.saveImage()

        }
    }

    onConfirmed:
    {
        root.close()
    }

    onCanceled:
    {
        root.close()
    }

}
