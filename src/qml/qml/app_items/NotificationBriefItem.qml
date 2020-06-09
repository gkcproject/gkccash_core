import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtGraphicalEffects 1.0


Rectangle {
	id:root
	height:146
	color: "#252F54"

	property int index:0

	property string title_str: qsTr("")
	property string time_str: qsTr(" ")
	property string text_str: qsTr(" ")
	property string block_str: qsTr(" ")
	property string link_str: qsTr(" ")
	property string author_str: qsTr("")

	property string txhash:qsTr("")

	function init()
	{
		walletModel.GetNotificationRecord(index)
	}

	Component.onCompleted:
    {
        walletModel.notificationChanged.connect(setNotification)
    }

    function setNotification(indexss,title,time,text,block,link,author,hash)
    {
    	if(index==indexss)
    	{
			title_str = title
			time_str = time
			text_str = text
			block_str = block
			link_str = link
			author_str = author
			txhash = hash
		}
    }
    
	MouseArea {
        anchors.fill: parent;
        onDoubleClicked: {
        	console.log("click index ",index)
            showDetails(title_str,time_str,text_str,block_str,link_str,author_str,txhash)
        }
    }

	CommonSelectableLabel {
        id:title_label
        font.weight: Font.Normal
        font.pixelSize:16
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 24
        anchors.leftMargin: 16
        text:title_str
    }

    TextEdit {
        id:text_text
        font.weight: Font.Light
        font.pixelSize:15
        anchors.left: title_label.left
        anchors.top: title_label.bottom
        anchors.right: parent.right
        anchors.rightMargin: 86
        anchors.topMargin: 8
        height:49

        //textColor:"#8F9AC5"
        textFormat: Qt.RichText
        readOnly: true
        text:text_str

        wrapMode: TextEdit.Wrap
    }

    Label{
        id:time_label
        font.weight: Font.Light
        font.pixelSize:14
        anchors.top:text_text.bottom
        anchors.left: text_text.left
        anchors.topMargin: 13
        color: "#8F9AC5"
        text:time_str
    }

    Label{
    	id:block_label
        text:block_str
        color: "#FDA205"
        anchors.left:time_label.right
        anchors.leftMargin: 56
        anchors.top:text_text.bottom
        anchors.topMargin: 13
        font.pixelSize: 14        
        font.weight: Font.Normal
    }

    Image {
		id:link_icon
		anchors.top:text_text.bottom
        anchors.topMargin: 12
        anchors.left:block_label.right
        anchors.leftMargin: 50
		width:22
		height:22
		source:"../../images/icons/link.png"
    }

    Label{
    	id:link_label
        text:link_str
        color: "#FDA205"
        anchors.left:link_icon.right
        anchors.leftMargin: 6
        anchors.top:text_text.bottom
        anchors.topMargin: 13
        font.pixelSize: 14        
        font.weight: Font.Normal
    }

    Image {
		id:author_icon
		anchors.top:text_text.bottom
        anchors.topMargin: 12
        anchors.left:link_label.right
        anchors.leftMargin: 50
		width:22
		height:22
		source:"../../images/icons/author.png"
    }

    Label{
    	id:author_label
        text:author_str
        color: "#FDA205"
        anchors.left:author_icon.right
        anchors.leftMargin: 6
        anchors.top:text_text.bottom
        anchors.topMargin: 13
        font.pixelSize: 14        
        font.weight: Font.Normal
    }
}