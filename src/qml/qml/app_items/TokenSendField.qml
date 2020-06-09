import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

import "../app_dialogs"
import "../app_pages"

Item {
    id:root

    function clearAll()
    {
        payToAddressField.text = ""
        amountField.amountField.text = "0"
        descriptionField.text = ""
        gasLimitSpin.amountField.text = walletModel.contractPage.getDefaultGasLimitOpSend()
        gasPriceField.amountField.text  = "0.0000004"
        gasPriceField.coinTypeBtn.index = 0
        gasPriceField.coinTypeBtn.text = "GKC"


    }


    Connections
    {
        target: walletModel.tokenItemModel


        onExecError:
        {
            //root_window.warningDialog.title = qsTr("Error")
            //root_window.warningDialog.content_text = errorMessage
            //root_window.warningDialog.dim_back = false
            //root_window.warningDialog.show()
        }

    }

    Label
    {
        id:payToAddressLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: root.top
        anchors.left: root.left

        anchors.topMargin: 10
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text: qsTr("payTo")
    }


    CommonTextField
    {
        id:payToAddressField
        anchors.verticalCenter: payToAddressLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: payToAddressLabel.left
        anchors.leftMargin: 120

        onTextChanged:
        {
            confirmBtn.enabled = false

            if(payToAddressField.text === "")
            {
                return
            }

            if(!walletModel.validateAddress(payToAddressField.text)) return

            confirmBtn.enabled = true
        }
    }


    Label
    {
        id:amountLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: payToAddressLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Amount")
    }


    SpinField
    {
        id:amountField
        anchors.verticalCenter: amountLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: amountLabel.left
        anchors.leftMargin: 120
        defaultAmount:0
    }


    Label
    {
        id:descriptionLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: amountLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Description")
    }


    CommonTextField
    {
        id:descriptionField
        anchors.verticalCenter: descriptionLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: descriptionLabel.left
        anchors.leftMargin: 120
    }

    Label
    {
        id:gaslimitLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: descriptionLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Gas Limit")
    }


    SpinField
    {
        id:gasLimitSpin
        anchors.verticalCenter: gaslimitLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: gaslimitLabel.left
        anchors.leftMargin: 120
        pageType: sendTo
    }




    Label
    {
        id:gasPriceLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: gaslimitLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Gas Price")
    }


    AmountField
    {
        id:gasPriceField
        anchors.verticalCenter: gasPriceLabel.verticalCenter
        anchors.left: gasPriceLabel.left
        anchors.leftMargin: 120
        anchors.right: root.right
        anchors.rightMargin: 20
        factor:1
        amountField.text:gasPriceField.coinTypeBtn.index === 0?"0.00000041":gasPriceField.coinTypeBtn.index === 1?"0.0004":"0.4"
        amountField.validator: DoubleValidator{
            bottom: gasPriceField.coinTypeBtn.index === 0?0.0000004:gasPriceField.coinTypeBtn.index === 1?0.0004:0.4
            top: gasPriceField.coinTypeBtn.index === 0?99999999:gasPriceField.coinTypeBtn.index === 1?99999999999:99999999999999
            notation:DoubleValidator.StandardNotation
            decimals:gasPriceField.coinTypeBtn.index === 0?8:gasPriceField.coinTypeBtn.index === 1?5:2
        }//TODO: Here validator is not working, check it out.

    }





    CommonButton
    {
        id:clearBtn
        color: "#3D4975"
        anchors.right: confirmBtn.left
        anchors.rightMargin: 40
        anchors.bottom: root.bottom
        anchors.bottomMargin:10
        text:qsTr("Clear")

        onClicked:
        {
            root.clearAll()
        }

    }


    CommonButton
    {
        id:confirmBtn
        color: enabled?"#FDA205":"#88FDA205"
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.bottom: root.bottom
        anchors.bottomMargin:10
        text:qsTr("Confirm")
        enabled:false

        onClicked:
        {
            if( payToAddressField.text.length === 0){
                payToAddressField.critical = true
                return
            }
            if( amountField.amountField.text.length === 0){
                amountField.amountField.critical = true
                return
            }

            if(requestUnlock())
                return


            var res = walletModel.tokenItemModel.sendToken(payToAddressField.text,
                                                   gasLimitSpin.amountField.text,
                                                   gasPriceField.coinTypeBtn.index,
                                                   gasPriceField.amountField.text,
                                                   amountField.amountField.text,
                                                   descriptionField.text)

            if(res[0] === "Error send token")
            {
            	walletModel.tokenItemModel.clearSendToken();
                root_window.warningDialog.title = res[0]
                root_window.warningDialog.content_text = res[1]
                root_window.warningDialog.dim_back = false
                root_window.warningDialog.show()
            }
			else{
            	waitDialog.title = res[0]
            	waitDialog.content_text = res[1]
            	waitDialog.confrim_btn_enabled = false
        	    waitDialog.timeLeft = 3
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
            if(walletModel.tokenItemModel.sendTokenConfirmed())
                clearAll()
        }

    }


}
