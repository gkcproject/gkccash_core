import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style

import "../app_items"
import "../app_dialogs"

Controls_1_4.Tab{

    Rectangle
    {
        id:root
        anchors.fill: parent
        color: "#212b50"

        property int pageType:rootTab.currentIndex
        property int send: 0
        property int receive: 1
        property int add: 2


        CommonDialog
        {
            id:warningDialog
            title: qsTr("Attention")
            confirm_btn_text: qsTr("Ok")
            cancel_btn_visible: false
            modality: Qt.ApplicationModal
            width:300
            height: 180
            dim_back:true
            onConfirmed:
            {
                warningDialog.close()
            }

        }

        Rectangle
        {
            id:background
            anchors.fill: parent
            color: "#212b50"

        }





        Rectangle
        {
            id:tabRec
            anchors.top: parent.top
            height:60
            anchors.left: parent.left
            anchors.right: parent.right
            color: "#212b50"

            Controls_1_4.TabView{
                currentIndex: 0
                anchors.fill:parent
                id:rootTab



                style:Controls_1_4_style.TabViewStyle
                {
                    frameOverlap: 1
                    tab: Rectangle
                    {
                        id:tab_rec
                        color: styleData.index > 2?"#F0F0F0":"#212b50"
                        implicitWidth: 100
                        implicitHeight: 50
                        border.width: 0
                        border.color: "#D5D9E8"

                        Text
                        {
                            id: text
                            anchors.horizontalCenter: tab_rec.horizontalCenter
                            anchors.verticalCenter: tab_rec.verticalCenter
                            anchors.verticalCenterOffset: 5
                            text: styleData.title
                            color: styleData.selected ?"#FDA205":"#6B78A8"
                            font.pixelSize: 15
                            font.weight:Font.Normal
                        }

                        Rectangle
                        {
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
                    // frame: Rectangle { color: "transparent" }
                }




                Controls_1_4.Tab
                {
                    title: qsTr("Send")

                    CommonTokenPage
                    {
                        id:sendPage
                        anchors.fill: parent
                        //pageType:send

                    }


                }

                Controls_1_4.Tab
                {
                    title: qsTr("Receive")

                    CommonTokenPage
                    {
                        id:receivePage
                        anchors.fill: parent
                        //pageType:receive

                    }
                }

                Controls_1_4.Tab
                {
                    title: qsTr("Add Token")

                    CommonTokenPage
                    {
                        id:addPage
                        anchors.fill: parent
                        //pageType:add

                    }
                }


            }

        }


        Rectangle
        {
            id:tokenItemsRec
            color: "#3B4674"
            anchors.left: parent.left
            anchors.top: tabRec.bottom
            width:350
            height:350
            anchors.topMargin: 0
            anchors.leftMargin: 10

            ListView {
                id: tokenItemsView
                clip:true
                focus: true
                currentIndex: -1
                boundsBehavior:Flickable.StopAtBounds
                anchors.fill: parent
                model: walletModel.tokenItemModel
                spacing: 2

                Component.onCompleted:
			    {
			        walletModel.addTokenEntryEmit.connect(setCurrentIndex)
			    }

			    function setCurrentIndex()
			    {
					currentIndex = -1
			    }

                onCurrentIndexChanged:
                {
                    var res = walletModel.tokenItemModel.updateReceiveImg(currentIndex)
                    tokenReceiveItem.qRImg.source = "image://ReQuestURI?" + Math.random()
                    tokenReceiveItem.titleLabel.text = res[0] + qsTr(" Address")
                    tokenReceiveItem.addressLabel.text = res[1]
                }

                CommonPopup
                {
                    id:popuper

                    width: 200
                    itemHeight: 32

                   Connections
                   {
                    target: warningDialog

                    onConfirmed:
                    {
                        walletModel.tokenItemModel.removeTokenItem(tokenItemsView.currentIndex)
                        currentIndex = -1
                        warningDialog.close()
                    }

                   }

                    onMenuPicked:
                    {
                        if(index !== 4){
                            walletModel.tokenItemModel.menuPicked(tokenItemsView.currentIndex,index)
                        }
                        else
                        {
                            warningDialog.title = qsTr("Confirm token remove")
                            warningDialog.content_text = qsTr("The selected token will be removed from the list. Are you sure?")
                            warningDialog.dim_back = false
                            warningDialog.show()
                        }

                    }

                    model: ListModel
                    {
                        ListElement {
                            itemData: qsTr("Copy receive address")
                        }
                        ListElement {
                            itemData: qsTr("Copy token balance")
                        }
                        ListElement {
                            itemData: qsTr("Copy token name")
                        }
                        ListElement {
                            itemData: qsTr("Copy contract address")
                        }
                        ListElement {
                            itemData: qsTr("Remove token")
                        }
                    }

                }



                delegate: ItemDelegate {
                    id:thisItem
                    width: tokenItemsView.width
                    height:40
                    hoverEnabled: true

                    Rectangle
                    {
                        anchors.fill: parent
                        visible: index === tokenItemsView.currentIndex
                        color: "#37426E"
                    }

                    Label {
                        id:symbolLabel
                        anchors.top: parent.top
                        anchors.leftMargin: 5
                        anchors.topMargin: 2
                        anchors.left: parent.left
                        width:95
                        font.pixelSize: 14
                        lineHeight: 0.8
                        color:"#FFFFFF"
                        font.weight: Font.Normal
                        font.letterSpacing: 0.5
                        elide: Label.ElideRight
                        text:symbol
                    }

                    Label {
                        id:amountLabel
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        anchors.left:parent.left
                        anchors.leftMargin: 150
                        anchors.right:parent.right
                        anchors.rightMargin: 5
                        elide: Label.ElideRight
                        font.pixelSize: 14
                        lineHeight: 0.8
                        font.weight: Font.Normal
                        font.letterSpacing: 0.5
                        text:balance
                        color:"#FFFFFF"
                    }

                    Label {
                        id:addressLabel
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 2
                        elide: Label.ElideRight
                        anchors.left:parent.left
                        anchors.leftMargin: 5
                        anchors.right:parent.right
                        anchors.rightMargin: 5
                        font.pixelSize: 12
                        lineHeight: 0.8
                        color:"#FFFFFF"
                        font.weight: Font.Light
                        font.letterSpacing: 0.5
                        text:address
                    }


                    MouseArea
                    {
                        id:mouseArea
                        anchors.fill: parent
                        acceptedButtons: Qt.AllButtons


                        onClicked:
                        {

                            if (mouse.button === Qt.RightButton)
                            {
                                popuper.x =      mouseX
                                popuper.y =  thisItem.y - tokenItemsView.contentY + mouseY



                                if(popuper.model === undefined)
                                    return

                                tokenItemsView.currentIndex = index

                                if(!popuper.opened)
                                {
                                    if(popuper.model.count>0)
                                        popuper.open()
                                }

                            }
                            else
                            {
                                tokenItemsView.currentIndex = index
                                walletModel.tokenItemModel.updateReceiveImg(index)
                            }




                        }
                    }


                }

                ScrollBar.vertical: MyScrollBar {
                    id: scrollBar
                    width:6
                    height:30

                    anchors.right: tokenItemsView.right
                    handle.implicitHeight: 100

                    policy:ScrollBar.AsNeeded
                    visible: tokenItemsView.count * 42 > tokenItemsView.height
                }



            }
        }

        TokenContractField
        {
            id:tokenContractField
            visible:root.pageType === root.add
            anchors.top: tokenItemsRec.top
            anchors.left: tokenItemsRec.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.bottom: tokenItemsRec.bottom
        }

        TokenReceiveItem
        {
            id:tokenReceiveItem
            visible:root.pageType === root.receive
            anchors.top: tokenItemsRec.top
            anchors.bottom: tokenItemsRec.bottom
            anchors.left: tokenItemsRec.right
            anchors.leftMargin: 10
            anchors.right: parent.right
        }


        TokenSendField
        {
            id:tokenSendField
            visible:root.pageType === root.send
            anchors.top: tokenItemsRec.top
            anchors.bottom: tokenItemsRec.bottom
            anchors.left: tokenItemsRec.right
            anchors.leftMargin: 10
            anchors.right: parent.right
        }



        Rectangle
        {
            id: tokenTableBck
            anchors.bottom:parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:tokenItemsRec.bottom
            anchors.topMargin: 0

            property alias historyTable: tokenTable

            radius: 0
            color: "#212b50"

            CommonComboBox {
                id:header_label
                width: 120
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.topMargin: 20
                anchors.leftMargin: 10
                //color: "#333333"
                //text:"Use this from to request payments ALL fields are optional"
                onCurrentIndexChanged:
                {
                    currentIndex_ = index
                    if(index == 6)
                    {
                        fromDate.visible = true
                        toDate.visible = true
                    }
                    else
                    {
                        fromDate.visible = false
                        toDate.visible = false
                    }
                   walletModel.tokenfilterproxy.chooseDate(index)
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
                    walletModel.tokenfilterproxy.chooseType(index)
                }


                model: ListModel
                {
                    ListElement { modelData: qsTr("ALL"); }
                    ListElement { modelData: qsTr("Received with"); }
                    ListElement { modelData: qsTr("Sent to"); }
                    ListElement { modelData: qsTr("To yourself"); }

                }
            }



            CommonTextField
            {
                id:query_textField
                font.weight: Font.Light
                font.pixelSize:13
                anchors.left: type_label.right
                anchors.leftMargin: 3
                anchors.right: symbolbox.left
                anchors.rightMargin: 3
                anchors.verticalCenter: header_label.verticalCenter
                placeholderText: qsTr("Enter address or label to search")

                onTextChanged:walletModel.tokenfilterproxy.changedPrefix(text)

            }


            CommonComboBox
            {
                id:symbolbox
                width: 180
                anchors.verticalCenter: header_label.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10
                //color: "#333333"
                //text:"Use this from to request payments ALL fields are optional"
                onCurrentStringChanged:
                {
                    walletModel.tokenfilterproxy.chooseName(index)
                }

                model: walletModel.tokenfilterproxy.tokenNameList
            }

            //CommonTextField
            //{
            //    id:min_amount_textField
            //    width:160
            //    font.weight: Font.Light
            //    font.pixelSize:13
            //    anchors.right: parent.right
            //    anchors.rightMargin: 10
            //    anchors.verticalCenter: header_label.verticalCenter
            //    placeholderText: qsTr("Min amount")
            //    enabled: false
            //    validator: DoubleValidator
            //    {
            //        bottom: 0
            //        decimals: 12
            //        notation: DoubleValidator.StandardNotation
            //    }

            //      onTextChanged:walletModel.tokenfilterproxy.changedAmount(text)

            //}

            DateEdit
            {
                id:fromDate
                anchors.top:header_label.bottom
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.topMargin: 10
                property var locale_: Qt.locale()
                dateValue:new Date().toLocaleDateString(locale_,"yyyy-MM-dd")
                visible: false
                z:tokenTable.z+1

                onDateChanged:
                {
                     if(header_label.currentIndex_ == 6)
                        walletModel.tokenfilterproxy.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
                }

                onBtnDown:
                {
					toDate.close_calender()
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
                z:tokenTable.z+1

                onDateChanged:
                {
                    if(header_label.currentIndex_ == 6)
                        walletModel.tokenfilterproxy.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
                }

                onBtnDown:
                {
					fromDate.close_calender()
                }
            }



            CommonTableView
            {
                id:tokenTable
                anchors.top:fromDate.visible?from100Date.bottom:header_label.bottom
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.topMargin: 10
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                popuperWidth:200
                roles:  ["date","type","address","name","amount"]
                titles: ["Date","Type","Address","Name","Amount"]
                widths: [150,120,180,200,160,width-830]

                model: walletModel.tokenfilterproxy

                itemDelegate: textDelegate

                Component {
                    id:textDelegate

                    Text{
                        text: styleData.value.split("|")[0]
                        //color: styleData.value.split("|")[1] === "6"?"#88FFFFFF": (styleData.selected? "#FFFFFF" : styleData.textColor)
                        color:"#FFFFFF"
						elide:styleData.elideMode
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                        font.letterSpacing: 0.5
                        font.weight: Font.Light
                    }
                }


                menuModel: ListModel
                {
                    ListElement {
                        itemData: qsTr("Copy address")
                    }
                    ListElement {
                        itemData: qsTr("Copy amount")
                    }
                    ListElement {
                        itemData: qsTr("Copy transaction ID")
                    }
                    ListElement {
                        itemData: qsTr("Copy full transaction details")
                    }
                    ListElement {
                        itemData: qsTr("Show transaction details")
                    }
                }

                onMenuPicked:
                {
                    if( menuIndex!= 4)
                        walletModel.tokenfilterproxy.menuPicked(tokenTable.currentRow,menuIndex)
                    else
                    {
                        detail_dialog.detail_dialog_str = walletModel.tokenfilterproxy.getTxDescription(tokenTable.currentRow)
                        detail_dialog.show()
                    }
                }

                onDoubleClicked:
                {
                    detail_dialog.detail_dialog_str = walletModel.tokenfilterproxy.getTxDescription(currentRow)
                    detail_dialog.show()
                }


                }

                CommonDialog
                {
                    id:detail_dialog
                    title:qsTr("Token details")
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





}
