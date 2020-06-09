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

Controls_1_4.Tab {
	Rectangle {
		id:root
		anchors.fill: parent
        color: "#212b50"
		
		property bool tabVisible: true
		property bool detailsVisible: false
		property bool sendVisible: false
		property int page: 0
		property int count: 20
		property int type: 0
		
		function visible_All_Clear()
        {
        	tabVisible = false
        	detailsVisible = false
        	sendVisible = false
        }

		function init()
		{
			visible_All_Clear()
			tabVisible = true
			type = 0
			page = 0
			walletModel.UpdateNotificationRecordList(type,0,20)
		}


		function showsendpage()
		{
			visible_All_Clear()
			sendVisible = true
		}

		
		Controls_1_4.TabView {
			id:notification_tab
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.right:parent.right
            anchors.bottom:parent.bottom
            anchors.topMargin: 33
            anchors.leftMargin: 27
            anchors.rightMargin:27
            anchors.bottomMargin:80
            visible:tabVisible

			currentIndex: 0
			onCurrentIndexChanged:
			{
				page = 0
				walletModel.UpdateNotificationRecordList(currentIndex,0,20)
			}
			style:Controls_1_4_style.TabViewStyle {
                frameOverlap: 1
                tab: Rectangle{
                    id:tab_rec
                    color: styleData.index > 2?"#F0F0F0":"#212b50"
                    implicitWidth: 100
                    implicitHeight: 50
                    border.width: 0
                    border.color: "#D5D9E8"

                    Text{
                        id: text
                        anchors.horizontalCenter: tab_rec.horizontalCenter
                        anchors.verticalCenter: tab_rec.verticalCenter
                        anchors.verticalCenterOffset: 5
                        text: styleData.title
                        color: styleData.selected ?"#FDA205":"#6B78A8"
                        font.pixelSize: 15
                        font.weight:Font.Normal
                    }

                    Rectangle {
                        color: "#212b50"
                        height: 2
                        width: 65
                        anchors.horizontalCenter: tab_rec.horizontalCenter
                        anchors.bottom: tab_rec.bottom
                        anchors.bottomMargin: 0
                        radius:1
                        visible: styleData.selected
                    }
                }
            }

            
			Controls_1_4.Tab {
				title:"Information"
				NotificationList{
					id:informationPage
                    anchors.fill: parent
				}
			}
			Controls_1_4.Tab {
				title:"Notice"				
				NotificationList{
					id:noticePage
                    anchors.fill: parent
				}
			}
		}

		Image {
			id:send_icon
			anchors.top: parent.top
	        anchors.right: parent.right
	        anchors.topMargin: 40
	        anchors.rightMargin: 107
			width:40
			height:40
			source:"../../images/icons/sendAdve.png"
			MouseArea{
	            anchors.fill: parent
	            onClicked: {
	            	showsendpage()
	            }
            }

            visible:tabVisible
    	}

		Label
        {
            id:send_label
            text:"Send adve"
	        color: "#FDA205"
            anchors.top: send_icon.top
            anchors.topMargin: 9
            anchors.left: send_icon.right
            anchors.leftMargin: 16
            
            visible:tabVisible
		}
		
		CommonButton
        {
            id:next_btn
            color: "#5567A1"
            anchors.top: notification_tab.bottom
            anchors.right: notification_tab.right
            anchors.topMargin: 13
            width: 70
            height: 30
            radius: 3
            text:qsTr("Next")
            textSize:14
            letterSpacing:0
            visible:tabVisible
            onClicked:
            {
				type = notification_tab.currentIndex 
				if(walletModel.HaveNotificationNextPage(type,page*20,20))
				{
					page = page + 1
					walletModel.UpdateNotificationRecordList(type,page*20,20)
				}
            }
		}

		CommonButton
        {
            id:prev_btn
            color: "#5567A1"
            anchors.top: next_btn.top
            anchors.right: next_btn.left
            anchors.rightMargin: 13
            width: 70
            height: 30
            radius: 3
            text:qsTr("Prev")
            textSize:14
            letterSpacing:0
            visible:tabVisible
            onClicked:
            {
				type = notification_tab.currentIndex 
				if(page>0)
				{
					page = page - 1
				}
				walletModel.UpdateNotificationRecordList(type,page*20,20)
            }
		}

		NotificationDetailsItem {
			id:notification_details

            visible:detailsVisible
		}

		NotificationSendItem {
			id:notification_send

            visible:sendVisible
		}
	}
}
