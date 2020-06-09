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

Rectangle {
	id:root
	CommonTableView
    {
        id:notificationListTable
        anchors.fill: parent     
		
		
        roles:  ["time","title","author","text"]
        titles: ["Time","Title","Author","Text"]
        widths: [150,150,150,width-450]
		
		model:walletModel.notificationRecords_Proxy
		
		itemDelegate: textDelegate
		
		Component {
            id:textDelegate

            Text{
                text: styleData.value
                color: styleData.selected? "#F0F0F0" : "#FFFFFF"
                elide:styleData.elideMode
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
                font.letterSpacing: 0.5
                font.weight: Font.Light
            }
        }

		onDoubleClicked:
		{
			detail_dialog.detail_dialog_str = walletModel.GetNotificationRecord(currentRow)
            detail_dialog.show()
		}

		CommonDialog
        {
            id:detail_dialog
            title: qsTr("Notification details")
            cancel_btn_text: qsTr("Cancel")
            content_text: "sssssss"
            width:600
            height: 400
            modality: Qt.WindowModal
            confrim_btn_visible: false
            property string detail_dialog_str

            Item{
                parent:detail_dialog.background_rec
                anchors.fill: parent
                anchors.topMargin: 25

                CommonTextArea
                {
                    id:detailArea
                    font.weight: Font.Light
                    font.pixelSize:13
                    anchors.fill: parent
                    anchors.margins: 10
                    anchors.bottomMargin: 50
                    textFormat: Qt.RichText
                    readOnly: true
                    wrapMode: TextEdit.Wrap
                    text:detail_dialog.detail_dialog_str
                }
            }
        }
    }
}