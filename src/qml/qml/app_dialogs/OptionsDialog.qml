import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import TrafficGraphWidget 1.0
import "../app_pages"
import "../app_items"
import "../base_items"


CommonDialog
{
    id:root
    title: qsTr("Options")
    confrim_btn_visible:false
    cancel_btn_visible:false
    property alias current_index:tabview.currentIndex
    width:600
    height:500




    Item{
        id:rootItem
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 25
        anchors.bottomMargin: 15


        CommonTabView
        {
            id:tabview
            tab_width:60
            currentIndex: 0
            anchors.fill: parent
            Controls_1_4.Tab {
				id:tab1
                title: qsTr("Wallet")
				
                Rectangle{
                    id:tab2_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:expertTitle
                        anchors.top:parent.top
                        anchors.left: parent.left
                        text:qsTr("Expert")
                        font.weight: Font.Medium
                        font.pixelSize:14
                        font.letterSpacing:0.3
                    }

                    CommonCheckBox {
                        id:coincontrolEnableCheckBox
                        font.weight: Font.Medium
                        font.pixelSize: 12
                        font.letterSpacing: 0.5
                        height:20
                        checked: false
                        text: qsTr("Coin Control")
						textColor:"#000000"
                        anchors.top:expertTitle.bottom
                        anchors.topMargin:20
                        anchors.left: parent.left
                        onCheckStateChanged:
                        {
                            //if(checked)
                            //    root_window.changeCoincontrol(true)
                            //else
                            //    root_window.changeCoincontrol(false)
                        }
                    }
					
					Label
			        {
			            id:avtiveCommandsTitle
						anchors.bottom: avtiveCommands.top
			            anchors.bottomMargin:5
			            anchors.left:parent.left
			            anchors.leftMargin:15
			            text:qsTr("Active command-line options that override above options:")
			            font.weight: Font.Medium
			            font.pixelSize:13
			            font.letterSpacing:0.3
			        }
			
			        Label
			        {
			            id:avtiveCommands
						anchors.bottom: restoreBtn.top
			            anchors.bottomMargin:5
			            anchors.left:parent.left
			            anchors.leftMargin:15
			            text:qsTr("None:")
			            font.weight: Font.Medium
			            font.pixelSize:13
			            font.letterSpacing:0.3
			        }			
			
			        CommonButton
			        {
			            id:restoreBtn
			            color: "#469AAC"
			            anchors.left:parent.left
			            anchors.leftMargin:15
			            anchors.bottom: parent.bottom
			            text:qsTr("Reset Options")
			            width: 100
			            height: 28
			            onClicked: {
							coincontrolEnableCheckBox.checked = false
			            }
			        }			
			
			        CommonButton
			        {
			            id:cancleBtn
			            color: "#EE637F"
			            anchors.right:parent.right
			            anchors.bottom: parent.bottom
			            anchors.rightMargin:15
			            text:qsTr("Cancel")
			            width: 70
			            height: 28
			            onClicked: {
			                root.close()
			            }
			        }			
			
			        CommonButton
			        {
			            id:confirmBtn
			            color: "#469AAC"
			            anchors.right:cancleBtn.left
			            anchors.rightMargin:10
			            anchors.bottom: parent.bottom

			            text:qsTr("Ok")
			            width: 70
			            height: 28
			            onClicked: {
							root_window.changeCoincontrol(coincontrolEnableCheckBox.checkState)
			                root.close()
			            }
			        }
                }
            }
        }        
    }
}
