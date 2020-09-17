import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtGraphicalEffects 1.0

import "../app_pages"

Rectangle {
	id:root
	anchors.fill: parent
    color: "#212b50"

    CommonTextField
    {
        id:title_textField
        font.weight: Font.Light
        font.pixelSize:18        
        anchors.top:root.top
        anchors.topMargin:33
        anchors.right: root.right
        anchors.rightMargin: 28
        anchors.left: root.left
        anchors.leftMargin: 62
        height:70		
        
        placeholderText: qsTr("Enter Title")

    }

    CommonTextField
    {
        id:link_textField
        font.weight: Font.Light
        font.pixelSize:18
        anchors.top:title_textField.bottom
        anchors.topMargin:24
        anchors.right: title_textField.right
        anchors.left: title_textField.left
        height:70

        placeholderText: qsTr("Enter Link (Optional)")

    }

    Label
    {
		id:text_l
		anchors.top: link_textField.bottom
        anchors.left: link_textField.left
        anchors.topMargin: 24        
        font.pixelSize:16
        color: "#FFFFFF"
        text:"Enter content:"
    }

    Rectangle{
        id:edit_rec
        anchors.top: text_l.bottom
        anchors.left: text_l.left
        anchors.right: link_textField.right
        anchors.topMargin: 24
        height: 240
        color: "#2E3962"
        focus: true

		Flickable {
	        id: flick
	        focus: false
	        anchors {
	            fill: parent
	        }
	        contentWidth: parent.width
	        contentHeight: text_textField.paintedHeight + 5
	        clip: true
	        boundsBehavior:Flickable.StopAtBounds
	        function ensureVisible(r) {
	                   if (contentX >= r.x)
	                       contentX = r.x;
	                   else if (contentX+width <= r.x+r.width)
	                       contentX = r.x+r.width-width;
	                   if (contentY >= r.y)
	                       contentY = r.y;
	                   else if (contentY+height <= r.y+r.height)
	                       contentY = r.y+r.height-height + 10;
	                }
	        TextEdit {
	            property bool ctrlPressed: false
	            anchors.fill:parent
	            id: text_textField
	            //readOnly: root.readOnly
            	selectByKeyboard: true
	            selectByMouse: true
	            //tabStopDistance: 20
            	color: "#FFFFFF" 
	            activeFocusOnPress: true
	            focus: true
	            clip: true
	            width: 640
	            selectionColor: Material.accent
	            wrapMode: TextEdit.WordWrap
	            topPadding: 0.5
	            font.pointSize: 14
	            height: text_textField.contentHeight > flick.height ?
	                        text_textField.contentHeight : flick.height
	            onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
	        }
	    }
    }

    CommonButton
    {
        id:sending_btn
        color: "#FDA205"
        anchors.right: edit_rec.right
        anchors.top: edit_rec.bottom
        anchors.topMargin:66
        width: 144
        height: 44
        radius: 3
        text:qsTr("Send")
        textSize:18
        letterSpacing:0
        onClicked:
		{
			if(walletModel.SendAdvertiseOperation(title_textField.text,link_textField.text,text_textField.text))
			{				
                title_textField.text = ""
                link_textField.text = ""
                text_textField.text = ""
				//sendDialog.title = "Send succes"
                //sendDialog.content_text = "Send Advertise successfully!"
                //sendDialog.dim_back = false
                //sendDialog.show()
			}
			else
			{
				//root_window.warningDialog.title = "Error send"
                //root_window.warningDialog.content_text = walletModel.GetMessage()
                //root_window.warningDialog.dim_back = false
                //root_window.warningDialog.show()
			}
		}
    }

    CommonDialog
    {
        id:sendDialog
        title: qsTr("Attention")
        confirm_btn_text: qsTr("Ok")
        cancel_btn_visible: false
        modality: Qt.ApplicationModal
        width:500
        height: 180
        dim_back:true
        onConfirmed:
        {
            sendDialog.close()
        }

    }
}