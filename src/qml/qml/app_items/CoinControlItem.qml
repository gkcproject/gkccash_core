import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as  Controls_Style_1_4

import "../app_pages"
import "../app_dialogs"


Item
{
    id:root
    height:customChangeAddressLabel.y+50
    property alias splitUtxoCheckBox:splitUtxoCheckBox
    property alias splitBlockCheckBoxChecked:splitUtxoCheckBox.checked
    property alias splitUtxoSize:splitUtxoField.text

    CoinControlDialog
    {
        id:coinControlDialog

    }

    function updateCoinControlLabelsOnce()
    {
        var msg
        msg = walletModel.coinControlProxy.updateCoinControlLabels(getPaymentList())
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

    function updateInnerViewOnce()
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

        coinControlDialog.updateDialogViewOnce()

    }




    Connections
    {
        target:walletModel.coinControlProxy

        onShowCoinControl:
        {
            autoHint.visible = !show
        }

        onUpdateLabelBlockSize:
        {
            uuxoSizeLabel.text = "UTXO Size: " + size
            updateInnerViewOnce()
        }

        onUpdateCoinControlLabelsSig:
        {
            updateCoinControlLabelsOnce()
            coinControlDialog.updateDialogViewOnce()

        }



    }


    Label {
        id:coinControlTitle
        font.weight: Font.Medium
        font.pixelSize:16
        font.letterSpacing: 0.355
        anchors.left: parent.left
        anchors.top: parent.top
        color:walletModel.isTestNet()?"#202020":"#FFFFFF"
        text:qsTr("Coin Control Features")
    }

    CommonButton
    {
        id:openCoinControlBtn
        color: "#FDA205"
        anchors.top: coinControlTitle.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        text:qsTr("Open Coin Control")
        width:120

        onClicked:
        {
            coinControlDialog.show()
        }

    }


    Label {
        id:autoHint
        font.weight: Font.Medium
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: openCoinControlBtn.right
        anchors.leftMargin: 15
        anchors.verticalCenter: openCoinControlBtn.verticalCenter
        color: "#F0F0F0"
        text:qsTr("Coins automatically selected")
        visible:true
    }

    Item{
        id:statusTable
        height:!visible?0:byteLabel.y+30
        anchors.left: openCoinControlBtn.left
        anchors.leftMargin: 15
        anchors.right:parent.right
        anchors.rightMargin:15
        anchors.top:openCoinControlBtn.bottom
        anchors.topMargin: 20
        visible:!autoHint.visible


        Label {
            id:quantityLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: parent.left
            anchors.top: parent.top
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
            text:""
        }

        Label {
            id:priorityLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: amountLabel.left
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
            text:""
        }

        Label {
            id:dustLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: feeLabel.left
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
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
            color: "#FFFFFF"
            text:""
        }


        Label {
            id:changeLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: afterFeeLabel.left
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#FFFFFF"
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
            color: "#FFFFFF"
            text:""
        }

    }

    CommonCheckBox
    {
        id:customChangeAddressCheckBox
        font.weight: Font.Light
        font.pixelSize: 11
        font.letterSpacing: 0.5
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        anchors.left: openCoinControlBtn.left
        anchors.leftMargin:5
        onClicked:
        {
            customChangeAddressHint.text = walletModel.coinControlProxy.updatecustomChangeAddress(checked,customChangeAddressField.text)
        }
    }

    Label
    {
        id:customChangeAddressLabel
        font.weight: Font.Normal
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: customChangeAddressCheckBox.right
        anchors.leftMargin:0
        anchors.top: autoHint.visivle ? autoHint.bottom : statusTable.bottom
        anchors.topMargin:10
        color: "#FFFFFF"
        text: qsTr("Custom change address")
    }

    CommonTextField
    {
        id:customChangeAddressField
        font.weight: Font.Light
        font.pixelSize:14
        anchors.left: customChangeAddressLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        placeholderText: qsTr("Enter a GKC address (e.g. Uek2swfjkwerwherjhbk32)")
        width: 300
        enabled:customChangeAddressCheckBox.checked

        onTextChanged:
        {
            customChangeAddressHint.text = walletModel.coinControlProxy.updatecustomChangeAddress(true,customChangeAddressField.text)
        }
    }

    Label
    {
        id:customChangeAddressHint
        font.weight: Font.Normal
        font.pixelSize:10
        font.letterSpacing: 0.355
        anchors.left: customChangeAddressField.left
        anchors.top: customChangeAddressField.bottom
        anchors.topMargin:2
        color: "#FFFFFF"
    }

    CommonCheckBox
    {
        id:splitUtxoCheckBox
        font.weight: Font.Light
        font.pixelSize: 11
        font.letterSpacing: 0.5
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        anchors.left: customChangeAddressField.right
        anchors.leftMargin:30
        onClicked:
        {
            walletModel.coinControlProxy.updateSplitUtxo(checked,splitUtxoField.text,afterFeeContent.text)
        }

    }


    Label
    {
        id:splitUtxoLabel
        font.weight: Font.Normal
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: splitUtxoCheckBox.right
        anchors.leftMargin:0
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        color: "#FFFFFF"
        text: qsTr("Split UTXO")
    }

    CommonTextField
    {
        id:splitUtxoField
        font.weight: Font.Light
        font.pixelSize:14
        anchors.left: splitUtxoLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        placeholderText: "# of outputs"
        width: 100
        enabled:splitUtxoCheckBox.checked

        onTextChanged:
        {
            walletModel.coinControlProxy.updateSplitUtxo(true,splitUtxoField.text,afterFeeContent.text)
        }
    }

    Label
    {
        id:uuxoSizeLabel
        font.weight: Font.Normal
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: splitUtxoField.right
        anchors.leftMargin:20
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        color: "#FFFFFF"
        text: "UTXO Size: 0.000"
    }

}
