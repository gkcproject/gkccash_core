import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtGraphicalEffects 1.0

Window{
	width:500
    height:210

    id:root


	property string title_str: qsTr("title")
	property string time_str: qsTr("time")
	property string text_str: qsTr("text")
	property string block_str: qsTr("block")
	property string link_str: qsTr("link")
	property string author_str: qsTr("author")
	property string hash_str: qsTr("hash")


	function init(currentRow)
	{
		console.log("init!!!!!",currentRow)
		walletModel.GetNotificationRecord(currentRow)
		
	}
	
	Rectangle {
		anchors.fill: parent
		width:parent.width
	    color: "#212B50"


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
				hash_str = hash
			}
	    }

		
		CommonSelectableLabel {
	        id:title_label
	        font.weight: Font.Normal
	        font.pixelSize:20
	        anchors.top:parent.top
	        anchors.left: parent.left
	        anchors.topMargin: 33
	        anchors.leftMargin: 62
	        text:title_str
	    }

	    Label{
	        id:time_label
	        font.weight: Font.Light
	        font.pixelSize:14
	        anchors.top:title_label.bottom
	        anchors.left: title_label.left
	        anchors.topMargin: 8
	        color: "#8F9AC5"
	        text:time_str
	    }

	    TextEdit {
	        id:text_text
	        font.weight: Font.Light
	        font.pixelSize:12
	        anchors.left: time_label.left
	        anchors.top: time_label.bottom
	        anchors.right: parent.right
	        anchors.bottom: parent.bottom
	        anchors.rightMargin: 86
	        anchors.topMargin: 42
	        anchors.bottomMargin: 120

	        //textColor:"#FFFFFF"
	        textFormat: Qt.RichText
	        readOnly: true
	        text:text_str

	        wrapMode: TextEdit.Wrap
	    }

		Image {
			id:block_icon
			anchors.top:text_text.bottom
	        anchors.topMargin: 15
	        anchors.left:text_text.left
			width:22
			height:22
			source:"../../images/icons/block.png"
	    }

	    Label{
	    	id:block_label
	        text:block_str
	        color: "#FDA205"
	        anchors.left:block_icon.right
	        anchors.leftMargin: 11
	        anchors.top:text_text.bottom
	        anchors.topMargin: 17
	        font.pixelSize: 14        
	        font.weight: Font.Normal
	    }

	    Image {
			id:link_icon
			anchors.top:text_text.bottom
	        anchors.topMargin: 15
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
	        anchors.leftMargin: 11
	        anchors.top:text_text.bottom
	        anchors.topMargin: 17
	        font.pixelSize: 14        
	        font.weight: Font.Normal
	    }

	    Image {
			id:author_icon
			anchors.top:text_text.bottom
	        anchors.topMargin: 15
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
	        anchors.leftMargin: 11
	        anchors.top:text_text.bottom
	        anchors.topMargin: 17
	        font.pixelSize: 14        
	        font.weight: Font.Normal
	    }

	    Image {
			id:hash_icon
			anchors.top:block_icon.bottom
	        anchors.topMargin: 8
	        anchors.left:block_icon.left
			width:22
			height:22
			source:"../../images/icons/hash.png"
	    }

	    Label{
	    	id:hash_label
	        text:hash_str
	        color: "#FDA205"
	        anchors.left:hash_icon.right
	        anchors.leftMargin: 11
	        anchors.top:block_icon.bottom
	        anchors.topMargin: 8
	        font.pixelSize: 14        
	        font.weight: Font.Normal
	    }

	    Image {
			id:copy_icon
			anchors.top:block_icon.bottom
	        anchors.topMargin: 8
	       	anchors.left:author_label.right
	        anchors.leftMargin: 50
			width:22
			height:22
			source:"../../images/icons/copy1.png"
	    }
	}
}
