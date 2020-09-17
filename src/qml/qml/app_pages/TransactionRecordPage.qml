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
        id: recBck
        anchors.fill:parent

        property alias historyTable: history_table
		property alias typeCombtn: type_label

        radius: 0
        color: "#212b50"

		function gotoEntrustReward()
        {			
			type_label.currentIndex_ = 19
			type_label.currentIndexChanged(19)
			type_label.text = qsTr("Bonus Reward")
        }

        CommonComboBox {
            id:header_label
            width: 120
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 20
            anchors.leftMargin: 30
            //color: "#333333"
            //text:"Use this from to request payments ALL fields are optional"
            onCurrentIndexChanged:
            {
                currentIndex_ = index
                if(index == 6)
                {
                    fromDate.visible = true
					fromDate.dateValue = new Date().toLocaleDateString(Qt.locale(),"yyyy-MM-dd")
                    toDate.visible = true
					toDate.dateValue = new Date().toLocaleDateString(Qt.locale(),"yyyy-MM-dd")
					walletModel.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
                }
                else
                {
                    fromDate.visible = false
                    toDate.visible = false
                }
                walletModel.chooseDate(index)
            }



            model: ListModel
            {
                ListElement { modelData: qsTr("ALL"); }
                ListElement { modelData: qsTr("Today"); }
                ListElement { modelData: qsTr("This week"); }
                ListElement { modelData: qsTr("This month"); }
                ListElement { modelData: qsTr("Last month"); }
                ListElement { modelData: qsTr("This year"); }
                ListElement { modelData: qsTr("Range..."); }
            }
        }

        CommonComboBox {
            id:type_label
            width: 280
            anchors.verticalCenter: header_label.verticalCenter
            anchors.left: header_label.right
            anchors.leftMargin: 3
            //color: "#333333"
            //text:"Use this from to request payments ALL fields are optional"
            onCurrentIndexChanged:
            {
                walletModel.chooseType(index)
            }


            model: ListModel
            {
                ListElement { modelData: qsTr("ALL"); }
                ListElement { modelData: qsTr("Most Common"); }
                ListElement { modelData: qsTr("Received with"); }
                ListElement { modelData: qsTr("Sent to"); }
                ListElement { modelData: qsTr("Obfuscated"); }
                ListElement { modelData: qsTr("Obfuscation Make Collateral Inputs"); }
                ListElement { modelData: qsTr("Obfuscation Create Denominations"); }
                ListElement { modelData: qsTr("Obfuscation Denominate"); }
                ListElement { modelData: qsTr("Obfuscation Collateral Payment"); }
                ListElement { modelData: qsTr("To yourself"); }
                //ListElement { modelData: qsTr("Contract receive"); }
                ListElement { modelData: qsTr("Contract send"); }
                ListElement { modelData: qsTr("Mined"); }
                ListElement { modelData: qsTr("Minted"); }
                ListElement { modelData: qsTr("Masternode Reward"); }
                ListElement { modelData: qsTr("Received PIV from zPIV"); }
                ListElement { modelData: qsTr("Zerocoin Mint"); }
                ListElement { modelData: qsTr("Zerocoin Spend"); }
                ListElement { modelData: qsTr("Zerocoin Spend, Change in zPIV"); }
                ListElement { modelData: qsTr("Zerocoin Spend to  Self"); }				
                ListElement { modelData: qsTr("Bonus Reward"); }
				ListElement { modelData: qsTr("Entrust"); }
				ListElement { modelData: qsTr("Deprive"); }
				ListElement { modelData: qsTr("GasRefund"); }
				ListElement { modelData: qsTr("FundReward"); }
                ListElement { modelData: qsTr("Other"); }


            }
        }


        CommonTextField
        {
            id:query_textField
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: type_label.right
            anchors.leftMargin: 3
            anchors.right: min_amount_textField.left
            anchors.rightMargin: 3
            anchors.verticalCenter: header_label.verticalCenter
            placeholderText: qsTr("Enter address or label to search")

            onTextChanged:walletModel.changedPrefix(text)

        }

        CommonTextField
        {
            id:min_amount_textField
            width:160
            font.weight: Font.Light
            font.pixelSize:13
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: header_label.verticalCenter
            placeholderText: qsTr("Min amount")
            validator: DoubleValidator
            {
                bottom: 0
                top:9999999999.99
                decimals: 12
                notation: DoubleValidator.StandardNotation
            }


            onTextChanged:walletModel.changedAmount(text)

        }

        DateEdit
        {
            id:fromDate
            anchors.top:header_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 10
            property var locale_: Qt.locale()
            dateValue:new Date().toLocaleDateString(locale_,"yyyy-MM-dd")
            visible: false
            z:history_table.z+1

            onDateChanged:
            {
                if(header_label.currentIndex_ == 6)
                walletModel.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
            }

            onCalendarVisibleChanged:
            {
                if(visible)
                {
                    toDate.close_calender()
                }

            }

        }

        DateEdit
        {
            id:toDate
            anchors.top:header_label.bottom
            anchors.left: fromDate.right
            anchors.leftMargin: 30
            anchors.topMargin: 10
            property var locale_: Qt.locale()
            dateValue:new Date().toLocaleDateString(locale_,"yyyy-MM-dd")
            visible: false
            z:history_table.z+1

            onDateChanged:
            {
                if(header_label.currentIndex_ == 6)
                walletModel.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
            }

            onCalendarVisibleChanged:
            {
                if(visible)
                {
                    fromDate.close_calender()
                }

            }
        }



        CommonTableView
        {
            id:history_table
            anchors.top:fromDate.visible?fromDate.bottom:header_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
			property string amountTitle: root_window.unitName
            roles:  ["status","date","type","address","amount"]
            titles: ["Status","Date","Type","Address","Amount(" + amountTitle.trim() +")"]
            widths: [50,120,180,300,160,width-830]

            model: walletModel.transaction_ProxyModel

            itemDelegate: textDelegate

            Component.onCompleted:
            {
                getColumn(0).delegate = imageDelegate
                getColumn(3).delegate = addressDelegate
            }

            onDoubleClicked:
            {
                detail_dialog.detail_dialog_str = walletModel.getTxDescription(currentRow)
                detail_dialog.show()
            }
			
			onAmountTitleChanged:
            {
                for(var i =0;i<history_table.columnCount;i++)
                {
                    if(history_table.getColumn(i).role === "amount")
                    {
                        history_table.getColumn(i).title = "Amount(" + amountTitle.trim() +")"
                    }
                }
            }

            Connections {
                target: history_table.selection
                onSelectionChanged:
                {
                    var rows_arry = new Array
                    history_table.selection.forEach( function(rowIndex) {
                         rows_arry.push(rowIndex);
                    } )

                    if(rows_arry.length>0)
                        selected_amount_label.text = walletModel.caculateSum(rows_arry)
                }

            }

            Component {
                id:textDelegate

                Text{
                    text: styleData.value.split("|")[0]
                    color: styleData.value.split("|")[1] === "6"?"#BBFFFFFF": (styleData.selected? "#F0F0F0" : "#FFFFFF")
                    elide:styleData.elideMode
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                    font.letterSpacing: 0.5
                    font.weight: Font.Light
                }
            }


            Component {
                id: imageDelegate
                Item {
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        fillMode: Image.PreserveAspectFit
                        height:16
                        cache : true;
                        asynchronous: true;
                        source: styleData === undefined?"":styleData.value
                    }
                }
            }

            Component {
                id: addressDelegate
                Item {
                    Image {
                        id:address_icon
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        fillMode: Image.PreserveAspectFit
                        height:16
                        cache : true;
                        asynchronous: true;
                        source: styleData.value.split("|")[1] === undefined  ? "" : styleData.value.split("|")[1]
                    }

                    Text{
                        id:address_item
                        anchors.left:address_icon.right
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: styleData.value.split("|")[0]

                        color: styleData.value.split("|")[2] === "6"?"#BBFFFFFF": (styleData.selected? "#F0F0F0" : "#FFFFFF")
                        elide:styleData.elideMode
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                        font.letterSpacing: 0.5
                        font.weight: Font.Light


                    }

                }
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

        Label {
            id:selected_amount_label
            //font.weight: Font.Medium
            font.pixelSize:13
            font.letterSpacing: 0.355
            anchors.bottom:parent.bottom
            textFormat: Qt.RichText
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#FFFFFF"
        }

        CommonButton
        {
            id:export_btn
            color: walletModel.isTestNet()?"#202020":"#5567A1"
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            width: 83
            height: 32
            radius: 3
            text:qsTr("Export")
            textSize:12
            letterSpacing:0

            onClicked: walletModel.exportClicked()
        }

        CommonDialog
        {
            id:export_dialog
            cancel_btn_text: qsTr("Confirm")
            width:300
            height: 300
            modality: Qt.WindowModal
            confrim_btn_visible: false


        }

        Connections {
            target: walletModel
            onMessage:
            {
                export_dialog.title = title
                export_dialog.content_text = message
                export_dialog.show()
            }

        }


    }

}
