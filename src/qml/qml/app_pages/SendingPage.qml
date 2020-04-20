import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.1 as Controls_1_1
import "../app_items"
import "../app_dialogs"

Controls_1_4.Tab {


    Rectangle
    {
        id:root
        anchors.fill:parent

        radius: 0
        color: "#212b50"
        property bool showCoincontrol:false

        function cleatAll()
        {
            while(target_listview.count>1)
            {
                target_listview.model.remove(1)
                target_listview.contentItem.children[1].targetItem.destroy()
            }

            target_listview.contentItem.children[0].targetItem.clearFirst()
        }



        CoinControlItem
        {
            id:coinControlItem
            anchors.top:root.top
            anchors.topMargin: 20
            width:parent.width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 28

            visible:showCoincontrol
        }



        ListView {
            id:target_listview
            clip: true
            width:parent.width
            anchors.top:coinControlItem.visible? coinControlItem.bottom : root.top
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 28
            anchors.bottom: transaction_fee.top
            anchors.bottomMargin: 18
            property int triggerPickAddressIndex: -1

            boundsBehavior:Flickable.StopAtBounds
            spacing: 20
            cacheBuffer: 2000

            currentIndex: -1

            delegate:ItemDelegate {
                height:140
                hoverEnabled: false
                property alias targetItem: targetItem
                TargetItem {
                    id:targetItem

                    width: target_listview.width - 20

                    onDeleteTarget:
                    {
                        if(target_listview.count > 1)
                        {
                            target_listview.model.remove(index)
                            targetItem.destroy()
                        }
                        else
                            clearFirst()

                        coinControlItem.updateCoinControlLabelsOnce()
                    }

                    onUpdateSeveral:
                    {
                        coinControlItem.updateCoinControlLabelsOnce()
                        coinControlItem.updateInnerViewOnce()

                    }

                    onPickAddress:
                    {
                        target_listview.triggerPickAddressIndex = index
                        sendAddressDialog.show()
                    }
                }

            }

            model: ListModel {
                ListElement { title: ""; }


            }
            ScrollBar.vertical: MyScrollBar {
                id: scrollBar
                width:10
                height:30
                anchors.right: target_listview.right
                policy:ScrollBar.AsNeeded
                visible: target_listview.count * 140 > target_listview.height
                handle.implicitHeight: 100
            }
        }

        TransactionFeeItem
        {
            id:transaction_fee
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30

        }
        CommonButton
        {
            id:sending_btn
            color: "#FDA205"
            anchors.right: clear_btn.left
            anchors.bottom: parent.bottom
            anchors.rightMargin: 8
            width: 83
            height: 32
            radius: 3
            text:qsTr("Send")
            textSize:12
            letterSpacing:0

            onClicked:
            {
                if(!checkTargetItemsAreReady())
                    return

                if(!requestUnlock())
                {
                    walletModel.coinControlProxy.sendButtonClicked(getRecipient(),
                                                                   coinControlItem.splitBlockCheckBoxChecked,
                                                                   coinControlItem.splitUtxoSize,
                                                                   transaction_fee.swiftTxChecked)

                }

            }

        }

        function getPaymentList()
        {
            var paymentList = new Array

            for(var i = 0;i<target_listview.count;i++)
            {
                paymentList.push(target_listview.contentItem.children[i].targetItem.getPayMent().toString())
            }

            return paymentList
        }


        function checkTargetItemsAreReady()
        {

            for(var i = 0;i<target_listview.count;i++)
            {
                if(!target_listview.contentItem.children[i].targetItem.infoIsReady())
                    return false
            }

            return true

        }

        function getRecipient()
        {
            var array = new Array
            for(var i = 0;i<target_listview.count;i++)
            {
                array.push(target_listview.contentItem.children[i].targetItem.getRecipient())
            }

            return array
        }




        CommonButton
        {
            id:clear_btn
            color: walletModel.isTestNet()?"#202020":"#3D4975"
            anchors.right: add_btn.left
            anchors.bottom: parent.bottom
            anchors.rightMargin: 8
            width: 83
            height: 32
            radius: 3
            text:qsTr("Clear All")
            textSize:12
            letterSpacing:0

            onClicked:
            {
                root.cleatAll()

                coinControlItem.updateCoinControlLabelsOnce()
            }
        }

        CommonButton
        {
            id:add_btn
            color: "#5567A1"
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            width: 100
            height: 32
            radius: 3
            text:qsTr("Add Recipient")
            textSize:12
            letterSpacing:0

            onClicked:
            {
                if(target_listview.count <= 10)
                    target_listview.model.append({ "title": ""})
                else{
                    root_window.warningDialog.content_text = qsTr("You can only add 10 recipients at most")
                    root_window.warningDialog.show()
                }

            }

        }

        SendAddressDialog
        {
            id:sendAddressDialog
            modality:Qt.ApplicationModal
            forPicking: true
            selectionMode:Controls_1_1.SelectionMode.SingleSelection

            onPickAddress:
            {
                target_listview.contentItem.children[target_listview.triggerPickAddressIndex].targetItem.setAddress(address)
            }
        }




        Connections
        {
            target:walletModel.coinControlProxy


            onNotifySendingResult:
            {

                if(type == -1)
                {
                    root_window.warningDialog.title = title
                    root_window.warningDialog.dim_back = false
                    root_window.warningDialog.content_text = msg
                    root_window.warningDialog.show()
                }
                else if(type == 0)
                {
                    root_window.warningDialog.title = title
                    root_window.warningDialog.dim_back = false
                    root_window.warningDialog.content_text = msg
                    root_window.warningDialog.show()
                    coinControlItem.splitUtxoCheckBox.checked = false
                }
                else
                {
                    waitDialog.title = title
                    waitDialog.content_text = msg
                    waitDialog.timeLeft = 3
                    waitDialog.confrim_btn_enabled = false
                    waitDialog.confirm_btn_text = qsTr("Ok") + "  (" + waitDialog.timeLeft + ")"
                    waitDialog.countDownTimer.start()
                    waitDialog.show()

                }

            }



        }





        CommonDialog
        {
            id:waitDialog
            modality: Qt.ApplicationModal
            property int timeLeft: 3
            property alias countDownTimer: countDownTimer
            width:500
            height:600

            Timer
            {
                id:countDownTimer
                interval: 1000
                onTriggered:
                {
                    if(waitDialog.timeLeft > 1)
                    {
                        waitDialog.timeLeft--
                        waitDialog.confirm_btn_text = qsTr("Ok") + "  (" + waitDialog.timeLeft + ")"
                        countDownTimer.start()
                    }
                    else
                    {
                        waitDialog.confirm_btn_text = qsTr("Ok")
                        waitDialog.confrim_btn_enabled = true
                    }

                }
            }


            onCanceled:
            {
                countDownTimer.stop()
            }

            onConfirmed:
            {
                waitDialog.close()
                walletModel.coinControlProxy.confirmSending()
                root.cleatAll()
            }

        }

    }



}
