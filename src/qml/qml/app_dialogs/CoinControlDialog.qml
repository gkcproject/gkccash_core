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
    width:900
    height: 600

    confirm_btn_text: qsTr("Ok")
    cancel_btn_visible:false

    title: qsTr("Coin Selection")

    property int treeMode:0
    property int tableMode:1
    property int viewMode:tableMode

    function updateDialogViewOnce()
    {
        var msg = walletModel.coinControlProxy.updateView(getPaymentList())

        if(msg.length > 0)
        {
            quantityContent.text = msg[0];
            amountContent.text = msg[1];
            feeContent.text = msg[2];
            afterFeeContent.text = msg[3];
            byteContent.text = msg[4];
            priorityContent.text = msg[5];
            dustContent.text = msg[6];
            changeContent.text = msg[7];

        }

    }

    onVisibleChanged:
    {
        if(visible)
        {
            updateInnerViewOnce()
        }

    }

    onConfirmed:
    {
        root.close()
    }

    onClosing:
    {
        walletModel.coinControlProxy.updateSendingPage()
    }



    Item{

        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 30


        Item{
            id:statusTable
            height:byteLabel.y+30
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.right:parent.right
            anchors.rightMargin:15
            anchors.top:parent.top


            Label {
                id:quantityLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: parent.left
                anchors.top: parent.top
                color: "#333333"
                text:qsTr("Quantity:")
            }


            Label {
                id:quantityContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: quantityLabel.left
                anchors.leftMargin: 70
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:""
            }


            Label {
                id:byteLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: quantityLabel.left
                anchors.top: quantityLabel.bottom
                anchors.topMargin: 20
                color: "#333333"
                text:qsTr("Bytes:")
            }


            Label {
                id:byteContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: byteLabel.left
                anchors.leftMargin: 70
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:""
            }

            Label {
                id:amountLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: quantityLabel.left
                anchors.leftMargin:180
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:qsTr("Amount:")
            }


            Label {
                id:amountContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: amountLabel.left
                anchors.leftMargin: 60
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:""
            }

            Label {
                id:priorityLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: amountLabel.left
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:qsTr("Priority:")
            }


            Label {
                id:priorityContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: priorityLabel.left
                anchors.leftMargin: 60
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:""
            }


            Label {
                id:feeLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: quantityLabel.left
                anchors.leftMargin:380
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:qsTr("Fee:")
            }


            Label {
                id:feeContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: feeLabel.left
                anchors.leftMargin: 60
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:""
            }

            Label {
                id:dustLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: feeLabel.left
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:qsTr("Dust:")
            }


            Label {
                id:dustContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: dustLabel.left
                anchors.leftMargin: 60
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:""
            }


            Label {
                id:afterFeeLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: quantityLabel.left
                anchors.leftMargin:580
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:qsTr("After Fee:")
            }


            Label {
                id:afterFeeContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: afterFeeLabel.left
                anchors.leftMargin: 70
                anchors.verticalCenter: quantityLabel.verticalCenter
                color: "#333333"
                text:""
            }


            Label {
                id:changeLabel
                font.weight: Font.Medium
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: afterFeeLabel.left
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:qsTr("Change:")
            }


            Label {
                id:changeContent
                font.weight: Font.Normal
                font.pixelSize:14
                font.letterSpacing: 0.355
                anchors.left: changeLabel.left
                anchors.leftMargin: 70
                anchors.verticalCenter: byteLabel.verticalCenter
                color: "#333333"
                text:""
            }

        }



        CommonButton
        {
            id:selectBtn
            color: "#469AAC"
            anchors.top: statusTable.bottom
            anchors.topMargin: 20
            anchors.left: statusTable.left
            text:qsTr("(Un)select all")
            width:100

            onClicked:
            {
                coinControlTable.model.selectAll()
            }

        }

        CommonButton
        {
            id:toggleBtn
            color: "#469AAC"
            anchors.top: statusTable.bottom
            anchors.topMargin: 20
            anchors.left: selectBtn.right
            anchors.leftMargin:15
            text:qsTr("toggle lock state")
            width:100

            onClicked:
            {
//                if(!root.viewMode !==  root.tableMode)
//                {
//                    root_window.warningDialog.title = qsTr("Tips")
//                    root_window.warningDialog.content_text = qsTr("Please switch to \"List Mode\" to use this function.")
//                    root_window.warningDialog.dim_back = false
//                    root_window.warningDialog.show()
//                    return
//                }

                coinControlTable.model.toggle()

            }

        }


        ButtonGroup
        {
            id: radioGroup
        }


        CommonRadioButton
        {
            id:treeBtn
            anchors.verticalCenter: toggleBtn.verticalCenter
            anchors.left: toggleBtn.right
            anchors.leftMargin: 20
            text: qsTr("Tree mode")
            ButtonGroup.group: radioGroup
            font.weight: Font.Medium
            font.pixelSize: 12

            onCheckedChanged:
            {
                if(checked)
                    root.viewMode = root.treeMode
                else
                    root.viewMode = root.tableMode
            }

        }

        CommonRadioButton
        {
            id:listBtn
            anchors.verticalCenter: toggleBtn.verticalCenter
            anchors.left: treeBtn.right
            anchors.leftMargin: 20
            text: qsTr("List mode")
            ButtonGroup.group: radioGroup
            font.weight: Font.Medium
            font.pixelSize: 12
        }


        CommonTableView
        {
            id:coinControlTable
            anchors.top:listBtn.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60
            roles:  ["status","amount","label","address","type","date","confirmations","priority"]
            titles: ["Status","Amount","Label","Address","Type","Date","Confirmations","Priority"]
            widths: [100,80,80,200,80,120,70,100,width-850]

            model: walletModel.coinControlProxy
            selectionMode:Controls_1_4.SelectionMode.SingleSelection

            //itemDelegate: textDelegate


            menuModel:menuListModel

            ListModel
            {
                id:menuListModel
                ListElement {
                    itemData: qsTr("Copy Address")
                }
                ListElement {
                    itemData: qsTr("Copy Label")
                }
                ListElement {
                    itemData: qsTr("Copy Amount")
                }
                ListElement {
                    itemData: qsTr("Copy TxID")
                }
                ListElement {
                    itemData: qsTr("Lock UTXO")
                }
                ListElement {
                    itemData: qsTr("Unlock UTXO")
                }
            }

            onMenuPicked:
            {
                if(menuIndex == 0)
                {
                    walletModel.setClipBoard(coinControlTable.model.getData("address",coinControlTable.currentRow))
                }
                else if(menuIndex == 1)
                {
                    walletModel.setClipBoard(coinControlTable.model.getData("label",coinControlTable.currentRow))
                }
                else if(menuIndex == 2)
                {
                    walletModel.setClipBoard(coinControlTable.model.getData("amount",coinControlTable.currentRow))
                }
                else if(menuIndex == 3)
                {
                    walletModel.setClipBoard(coinControlTable.model.getData("txid",coinControlTable.currentRow))

                }
                else if(menuIndex == 4)
                {
                    coinControlTable.model.setData_(coinControlTable.currentRow,"1",0)
                }
                else if(menuIndex == 5)
                {
                    coinControlTable.model.setData_(coinControlTable.currentRow,"0",0)
                }


            }


            Component.onCompleted:
            {
                getColumn(0).delegate = statusDelegate
                // getColumn(3).delegate = addressDelegate
            }

            onDoubleClicked:
            {
                // detail_dialog.detail_dialog_str = walletModel.getTxDescription(currentRow)
                // detail_dialog.show()
            }


            Component {
                id:statusDelegate
                Item {
                    CommonCheckBox
                    {
                        id:selectionCheckBox
                        font.weight: Font.Light
                        font.pixelSize: 11
                        font.letterSpacing: 0.5
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset:-20
                        checked: styleData.value.split("|")[0] === "1"
                        enabled: styleData.value.split("|")[1] === "0"


                        onClicked:
                        {
                            coinControlTable.model.setData_(coinControlTable.currentRow,checked?"1":"0",1)
                        }
                    }


                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset:20
                        fillMode: Image.PreserveAspectFit
                        height:16
                        visible:styleData.value.split("|")[1] === "1"
                        cache : true;
                        asynchronous: true;
                        source: "qrc:/images/icons/locked.png"
                    }
                }
            }


            //        Component {
            //            id: imageDelegate
            //            Item {
            //                Image {
            //                    anchors.verticalCenter: parent.verticalCenter
            //                    anchors.horizontalCenter: parent.horizontalCenter
            //                    fillMode: Image.PreserveAspectFit
            //                    height:16
            //                    cache : true;
            //                    asynchronous: true;
            //                    source: styleData === undefined?"":styleData.value
            //                }
            //            }
            //        }

            //        Component {
            //            id: addressDelegate
            //            Item {
            //                Image {
            //                    id:address_icon
            //                    anchors.left: parent.left
            //                    anchors.verticalCenter: parent.verticalCenter
            //                    fillMode: Image.PreserveAspectFit
            //                    height:16
            //                    cache : true;
            //                    asynchronous: true;
            //                    source: styleData.value.split("|")[1] === undefined  ? "" : styleData.value.split("|")[1]
            //                }

            //                Text{
            //                    id:address_item
            //                    anchors.left:address_icon.right
            //                    anchors.leftMargin: 15
            //                    anchors.verticalCenter: parent.verticalCenter
            //                    anchors.right: parent.right
            //                    text: styleData.value.split("|")[0]

            //                    color: styleData.value.split("|")[2] === "6"?"#88333333": (styleData.selected? "#333333" : styleData.textColor)
            //                    elide:styleData.elideMode
            //                    horizontalAlignment: Text.AlignLeft
            //                    verticalAlignment: Text.AlignVCenter
            //                    font.pixelSize: 12
            //                    font.letterSpacing: 0.5
            //                    font.weight: Font.Light


            //                }

            //            }
            //        }


        }

    }
}
