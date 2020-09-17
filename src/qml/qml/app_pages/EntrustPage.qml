import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtGraphicalEffects 1.0

import "../app_items"

Controls_1_4.Tab {
	

    Rectangle {
		id:root
		property string entrust_label_str: qsTr("My Entrust:  0.00000000  ")
		property string profit_label_str: qsTr("TotalDividend:  0.00000000  ")
		property string nodeID_label_str: qsTr("Node ID: ------  ")
    	property string commissioned_label_str: qsTr("Commissioned: ------  ")
		property string surplus_label_str: qsTr("Availabel Balance: ------  ")
    	property bool mainVisible: true
    	property bool entrustRecordVisible: false
    	property bool redeemRecordVisible: false
    	property bool entrustVisible: false
		property alias bonusBtn: seeBonus_btn
	property alias nodeListTable:nodeListTable
	property alias entrustRecordTable:entrustRecordTable
	property alias redeemRecordTable:redeemRecordTable
    
		
        property string currentNodeID: qsTr("")
		property string amountEntrustNum: qsTr("")
		property int indexEntrustNum:0
        property string redeemTXID: qsTr("")
        property string entrustSuccess_dialog_str: qsTr("")
	property alias entrustSuccess_dialog:entrustSuccess_dialog
	property alias redeemSuccess_dialog:redeemSuccess_dialog
	property alias error_dialog:error_dialog
              
        anchors.fill:parent
		
        radius: 0
        color: "#212b50"

        Component.onCompleted:
        {
            walletModel.entrustChanged.connect(setEntrusts)
        }
        

        function setEntrusts(balance,
                            Entrusts,
                            TotalDividend)
        {
            entrust_label_str = qsTr("My Entrust: ") + walletModel.formatAmount(Entrusts) + unitName
            profit_label_str = qsTr("TotalDividend: ") + walletModel.formatAmount(TotalDividend) + unitName
            commissioned_label_str = qsTr("Commissioned: ") + walletModel.formatAmount(Entrusts) + unitName
            surplus_label_str = qsTr("Availabel Balance: ") + walletModel.formatAmount(balance) + unitName
        }
        
        function visible_All_Clear()
        {
            mainVisible = false
            entrustRecordVisible = false
            redeemRecordVisible = false
            entrustVisible = false
        }
        
        function showEntrust()
        {
            visible_All_Clear()
            entrustVisible = true
        }
        
        function showRedeemRecord()
        {
            visible_All_Clear()
            redeemRecordVisible = true
        }
        
        function init()
        {
            visible_All_Clear()
			walletModel.InitEntrust()
            mainVisible = true
        }
        
        function showEntrustRecord()
        {
            visible_All_Clear()
            entrustRecordVisible = true
        }
		
		CommonSelectableLabel {
            id:allEntrust_label
            font.weight: Font.Normal
            font.pixelSize:20
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 61
            anchors.leftMargin: 27
            text:entrust_label_str
            visible:mainVisible
        }
		
		CommonSelectableLabel {
            id:profit_label
            font.weight: Font.Normal
            font.pixelSize:20
            anchors.top:allEntrust_label.bottom
            anchors.left: allEntrust_label.left
            anchors.topMargin: 33
            text:profit_label_str
            visible:mainVisible
        }
		
		CommonButton
        {
            id:entrustRecord_btn
            color: "#5567A1"
            anchors.top: profit_label.bottom
            anchors.left: profit_label.left
            anchors.topMargin: 54
            width: 130
            height: 44
            radius: 3
            text:qsTr("Entrust Record")
            textSize:16
            letterSpacing:0
            visible:mainVisible
            
            onClicked:
            {
                showEntrustRecord()
            }
		}
		
		CommonButton
        {
            id:redeemRecord_btn
            color: "#5567A1"
            anchors.top: entrustRecord_btn.top
            anchors.left: entrustRecord_btn.right
            anchors.leftMargin: 13
            width: 130
            height: 44
            radius: 3
            text:qsTr("Redeem Record")
            textSize:16
            letterSpacing:0
            visible:mainVisible
            
            onClicked:
            {
                showRedeemRecord()
            }
		}
		
		CommonButton
        {
            id:seeBonus_btn
            color: "#5567A1"
            anchors.top: redeemRecord_btn.top
            anchors.left: redeemRecord_btn.right
            anchors.leftMargin: 13
            width: 130
            height: 44
            radius: 3
            text:qsTr("See Bonus")
            textSize:16
            letterSpacing:0
            visible:mainVisible
            
			onClicked:
			{
				gotoTxpageReward()
			}
		}
        
        Label
        {
            id:nodeList_label
            font.weight: Font.Normal
            font.pixelSize:17
            anchors.left: entrustRecord_btn.left
            anchors.top: entrustRecord_btn.bottom
            anchors.topMargin: 53
            color: "#FFFFFF"
            text:qsTr("Node List")			
            visible:mainVisible
        }
		
        CommonTableView
        {
            id:nodeListTable
            anchors.top:nodeList_label.bottom
            anchors.left: nodeList_label.left
            anchors.topMargin: 31
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom:parent.bottom
            anchors.bottomMargin: 40
            visible:mainVisible
            
			
			
            roles:  ["nodeid","nodename","usernum","entrust","totaldealdivide","totaldividend","operation"]
            titles: ["NodeID","NodeName","UserNum","Entrusts(GKC)","TotalDealDivide(GKC)","TotalDividend(GKC)","Operation"]
            widths: [520,180,100,180,180,180,120]
			
			model:walletModel.nodeList_Proxy
			
			itemDelegate: textDelegate
			
			Component.onCompleted:
			{
				getColumn(6).delegate = operationDelegate
			}
			
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
			
			Component {
				id: operationDelegate
				Item{
					CommonButton{
						id:entrustbtn_Delegate
						anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
						color: "#FDA205"
						width:56
						height:28
            			radius: 3
            			text:qsTr("Entrust")
            			textSize:12
						onClicked:
						{							
                            currentNodeID = walletModel.getNodeID(styleData.row)
                            nodeID_label_str = qsTr("Node ID: ") + currentNodeID
                            showEntrust()
						}
					}
				}
			}
        }

        Label {
            id:entrustRecord_label
            font.weight: Font.Normal
            font.pixelSize:20
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 57
            anchors.leftMargin: 27
            color: "#FFFFFF"
            text:qsTr("Entrust Record")
            visible:entrustRecordVisible
        }

        CommonDialog
        {
            id:detail_dialog
            title: qsTr("Transaction details")
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
                    id:detailAreaEntruct
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
        
        CommonTableView
        {
            id:entrustRecordTable
            anchors.top:entrustRecord_label.bottom
            anchors.left: entrustRecord_label.left
            anchors.topMargin: 42
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom:parent.bottom
            anchors.bottomMargin: 201
            visible:entrustRecordVisible
            
            roles:  ["agent","agentname","amount","txid","operation"]
            titles: ["NodeID","NodeName","Entrusts(GKC)","TXID","Operation"]
            widths: [520,200,200,400,200]
			
			model:walletModel.entrustRecords_Proxy
			
			itemDelegate: textDelegate1
			
			Component.onCompleted:
			{
				getColumn(4).delegate = operationDelegate1
			}

			onDoubleClicked:
            {
                detail_dialog.detail_dialog_str = walletModel.GetEntructDescription(currentRow)
                detail_dialog.show()
            }

            
			
			Component {
                id:textDelegate1

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
			
			Component {
				id: operationDelegate1
				Item{
					CommonButton{
						id:entrustbtn_Delegate
						//anchors.left:parent.left
						anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
						color: "#FDA205"
						width:56
						height:28
            			radius: 3
            			text:qsTr("Redeem")
            			textSize:12
						onClicked:
						{
							if(requestUnlock())	return
							redeem_dialog.redeem_dialog_str = walletModel.GetRedeemDescription(styleData.row)
                            redeemTXID = walletModel.GetRedeemTXID(styleData.row)
                            redeem_dialog.show()
						}
					}
				}
			}
        }
        
        Label {
            id:redeemRecord_label
            font.weight: Font.Normal
            font.pixelSize:20
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 57
            anchors.leftMargin: 27
            color: "#FFFFFF"
            text:qsTr("Redeem Record")
            visible:redeemRecordVisible
        }
        
        CommonTableView
        {
            id:redeemRecordTable
            anchors.top:redeemRecord_label.bottom
            anchors.left: redeemRecord_label.left
            anchors.topMargin: 42
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom:parent.bottom
            anchors.bottomMargin: 201
            visible:redeemRecordVisible
			
			roles:  ["agent","amount","txid","vout"]
            titles: ["NodeID","Entrusts(GKC)","TXID","VoutIndex"]
            widths: [520,200,400,200]
			
			model:walletModel.redeemRecords_Proxy
			
			itemDelegate: textDelegate2

			onDoubleClicked:
            {
                detail_dialog.detail_dialog_str = walletModel.GetDepriveDescription(currentRow)
                detail_dialog.show()
            }
			
			Component {
                id:textDelegate2

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
        }
        
        Label {
            id:entrust_label
            font.weight: Font.Normal
            font.pixelSize:20
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 57
            anchors.leftMargin: 27
            text:qsTr("Entrust")			
            color: "#FFFFFF"
            visible:entrustVisible
        }
        
        CommonSelectableLabel {
            id:nodeID_label
            font.weight: Font.Normal
            font.pixelSize:17
            anchors.top:entrust_label.bottom
            anchors.left: entrust_label.left
            anchors.topMargin: 98
            text:nodeID_label_str
            visible:entrustVisible
        }
        
        CommonSelectableLabel {
            id:commissioned_label
            font.weight: Font.Normal
            font.pixelSize:18
            anchors.top:nodeID_label.bottom
            anchors.left: nodeID_label.left
            anchors.topMargin: 37
            text:commissioned_label_str
            visible:entrustVisible
        }
        
        CommonSelectableLabel {
            id:surplus_label
            font.weight: Font.Normal
            font.pixelSize:18
            anchors.top:commissioned_label.bottom
            anchors.left: commissioned_label.left
            anchors.topMargin: 38
            text:surplus_label_str
            visible:entrustVisible
        }
        
        Label {
            id:entrustNum_label
            font.weight: Font.Medium
            font.pixelSize:17
            anchors.top:surplus_label.bottom
            anchors.left: surplus_label.left
            anchors.topMargin: 84
            color: "#FFFFFF"
            text:qsTr("Entrust Number")
            visible:entrustVisible
        }


        AmountField
        {
            id:amountEntrsut
            width: 842
            height: 48
            anchors.top:surplus_label.bottom
            anchors.topMargin: 69
            //anchors.verticalCenter: total_label.verticalCenter
            anchors.left: entrustNum_label.right
            anchors.leftMargin: 33
            visible:entrustVisible
        }
        
        CommonButton{
			id:entrust_btn
            anchors.top:amountEntrsut.bottom
            anchors.topMargin: 86
			anchors.right:amountEntrsut.right
			color: "#FDA205"
			width:122
            height:44
   			radius: 3
   			text:qsTr("SURE")
   			textSize:16
            visible:entrustVisible
			onClicked:
			{
				if(requestUnlock())	return
				amountEntrustNum = amountEntrsut.valueAmount
				indexEntrustNum = amountEntrsut.coinTypeBtn.index
				entrust_dialog.entrust_dialog_str = walletModel.GetEntrustSure(currentNodeID,amountEntrustNum,amountEntrsut.coinTypeBtn.uint)
                entrust_dialog.show()
			}
		}
        
        CommonDialog
        {
            id:redeem_dialog
            title: qsTr("Sure Redeem")
            confirm_btn_text: qsTr("Sure Redeem")
            width:900
            height: 410
            modality: Qt.WindowModal
            close_btnRect_visible: false
            property string redeem_dialog_str: qsTr("")

            Item{
                parent:redeem_dialog.background_rec
                anchors.fill: parent
                anchors.topMargin: 25
                CommonTextArea
                {
                    id:detailAreaRedeem
                    font.weight: Font.Light
                    font.pixelSize:16
                    anchors.fill: parent
                    anchors.margins: 10
                    anchors.bottomMargin: 50
                    textFormat: Qt.RichText
                    readOnly: true
                    wrapMode: TextEdit.Wrap
                    text:redeem_dialog.redeem_dialog_str
                }
            }
            
            onConfirmed:
            {
                redeem_dialog.close()
                if(walletModel.RedeemOperation(redeemTXID))
                {
                    redeemSuccess_dialog.show()
                }
                else
                {
					//root_window.warningDialog.title = qsTr("Redeem failure")
                	//root_window.warningDialog.content_text = walletModel.GetMessage()
                	//root_window.warningDialog.dim_back = false
                	//root_window.warningDialog.show()
					
                    //error_dialog.error_dialog_title = "Sure Redeem"
                    //error_dialog.error_dialog_str = "Redeem failure"
                    //error_dialog.error_dialog_msg = walletModel.GetMessage()
                    //error_dialog.show()
                }
            }
	}
        
        CommonDialog
        {
            id:redeemSuccess_dialog
            title: qsTr("Sure Redeem")
            width:500
            height: 220
            modality: Qt.WindowModal
            cancel_btn_visible: false
            confrim_btn_visible: false

            Item{
                parent:redeemSuccess_dialog.background_rec
                anchors.fill: parent
                anchors.topMargin: 25
                Image {
                    id:redeemSuccess_image
                    anchors.left: parent.left
                    anchors.leftMargin:20
                    anchors.top: parent.top
                    anchors.topMargin:48
                    fillMode: Image.PreserveAspectFit
                    height:32
                    cache : true
                    asynchronous: true
                    source: "qrc:/images/icons/chengong.png"
                }
                Label{
                    font.weight: Font.Medium
                    font.pixelSize:17
                    //anchors.top:redeemSuccess_image.top
			anchors.verticalCenter: redeemSuccess_image.verticalCenter
                    anchors.left: redeemSuccess_image.right
                    anchors.leftMargin: 21
                    color: "#5D77AB"
                    text:qsTr("Redeem Success")
                }                
            }
        }

        CommonDialog
        {
            id:entrust_dialog
            title: qsTr("Sure Entrust")
            confirm_btn_text: qsTr("Sure Entrust")
            width:900
            height: 410
            modality: Qt.WindowModal
            cancel_btn_visible: false
            property string entrust_dialog_str

            Item{
                parent:entrust_dialog.background_rec
                anchors.fill: parent
                anchors.topMargin: 25
                CommonTextArea
                {
                    id:detailArea1entrustSuccess_dialog_str
                    font.weight: Font.Light
                    font.pixelSize:16
                    anchors.fill: parent
                    anchors.margins: 10
                    anchors.bottomMargin: 50
                    textFormat: Qt.RichText
                    readOnly: true
                    wrapMode: TextEdit.Wrap
                    text:entrust_dialog.entrust_dialog_str
                }
            }
            
            onConfirmed:
            {
				entrust_dialog.close()
                if(walletModel.EntrustOperation(currentNodeID,amountEntrustNum,indexEntrustNum))
                {	
                    //entrustSuccess_dialog_str = qsTr("TXID: ")+walletModel.GetMessage()
                    entrustSuccess_dialog.show()
                }
                else
                {
					//root_window.warningDialog.title = qsTr("Entrust failure")
                	//root_window.warningDialog.content_text = walletModel.GetMessage()
                	//root_window.warningDialog.dim_back = false
                	//root_window.warningDialog.show()
                    //error_dialog.error_dialog_title = qsTr("Sure Entrust")
                    //error_dialog.error_dialog_str = qsTr("Entrust failure")
                    //error_dialog.error_dialog_msg = walletModel.GetMessage()
                    //error_dialog.show()
                }
            }
	
        }
        
        CommonDialog
        {
            id:entrustSuccess_dialog
            title: qsTr("Sure Entrust")
            width:800
            height: 300
            modality: Qt.WindowModal
            cancel_btn_visible: false
            confrim_btn_visible: false
        	

            Item{
                parent:entrustSuccess_dialog.background_rec
                anchors.fill: parent
                anchors.topMargin: 25
                Image {
                    id:entrustSuccess_image
                    anchors.left: parent.left
                    anchors.leftMargin:20
                    anchors.top: parent.top
                    anchors.topMargin:48
                    fillMode: Image.PreserveAspectFit
                    height:32
                    cache : true
                    asynchronous: true
                    source: "qrc:/images/icons/chengong.png"
                }
                Label{
                    font.weight: Font.Medium
                    font.pixelSize:17
                    //anchors.top:entrustSuccess_image.top
					anchors.verticalCenter: entrustSuccess_image.verticalCenter
                    anchors.left: entrustSuccess_image.right
                    anchors.leftMargin: 21
                    color: "#5D77AB"
                    text:qsTr("Entrust Success")
                }
                Label{
					id:entrustSuccess_nodeid
					anchors.left:entrustSuccess_image.left
					anchors.top:entrustSuccess_image.bottom
					anchors.topMargin:44
					font.pixelSize:16
					color:"#333333"
					text:qsTr("NodeID:")+currentNodeID
				}
				Label{
					id:entrustSuccess_number
					anchors.left:entrustSuccess_nodeid.left
					anchors.top:entrustSuccess_nodeid.bottom
					anchors.topMargin:39
					font.pixelSize:16
					color:"#333333"
					text:qsTr("EntrustNumber:")
				}
				Label{
					anchors.left:entrustSuccess_number.right
					anchors.top:entrustSuccess_nodeid.bottom
					anchors.leftMargin:14
					anchors.topMargin:37
					font.pixelSize:19
					color:"#5D77AB"
					text:amountEntrustNum + amountEntrsut.coinTypeBtn.uint
				}
            }
        }
        
        CommonDialog
        {
            id:error_dialog
            
            property string error_dialog_title
            title: error_dialog.error_dialog_title
            width:500
            height: 220
            modality: Qt.WindowModal
            cancel_btn_visible: false
            confrim_btn_visible: false
            property string error_dialog_str
            property string error_dialog_msg

            Item{
                parent:error_dialog.background_rec
                anchors.fill: parent
                anchors.topMargin: 25
                Image {
                    id:error_image
                    anchors.left: parent.left
                    anchors.leftMargin:20
                    anchors.top: parent.top
                    anchors.topMargin:48
                    fillMode: Image.PreserveAspectFit
                    height:32
                    cache : true
                    asynchronous: true
                    source: "qrc:/images/icons/shibai.png"
                }
                Label{
                    font.weight: Font.Medium
                    font.pixelSize:17
                    //anchors.top:error_image.top
					anchors.verticalCenter: error_image.verticalCenter
                    anchors.left: error_image.right
                    anchors.leftMargin: 21
                    color: "#5D77AB"
                    text:error_dialog.error_dialog_str
                }
            }
        }
	}
}
