import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Item {
    id:root
    property var rec_unit
    property var rec_block
    property bool minimized: false
    property bool swiftTxChecked: swiftx_btn.checked

    implicitHeight: !minimized?(swiftx_btn.y + swiftx_btn.height):(transaction_fee_btn.y+transaction_fee_btn.height + swiftx_btn.height)


    Component.onCompleted:
    {
        root_window.unitNameChanged.connect(coinTypeChange)


    }


    function coinTypeChange()
    {
        if(root_window.unitName === "  GKC"){
            amountField.coinTypeBtn.text = "GKC"
            amountField.coinTypeBtn.index = 0
        }
        else if(root_window.unitName === "  mGKC"){
            amountField.coinTypeBtn.text = "mGKC"
            amountField.coinTypeBtn.index = 1
        }
        else{
            amountField.coinTypeBtn.index = 2
            amountField.coinTypeBtn.text = "μGKC"
        }
        walletModel.coinControlProxy.setValue(0,minimized)

    }


    Connections
    {
        target:walletModel.coinControlProxy

        onUpdateSmartFeeLabels:
        {

            if(returnList.length > 0)
            {
                recommended_btn.text = "Recommended " + returnList[0] + "   " +  returnList[1] + "    " + returnList[2]
                transaction_fee.text = returnList[3];
            }

        }

    }


    Label{
        id:transaction_fee_label
        font.weight: Font.Normal
        font.pixelSize:22
        anchors.left: root.left
        anchors.top: root.top
        color: walletModel.isTestNet()?"#202020":"#FDA205"
        text:qsTr("Transaction Fee")

    }

    Label{
        id:transaction_fee
        font.weight: Font.Normal
        font.pixelSize:15
        anchors.left: transaction_fee_label.right
        anchors.leftMargin: 10
        anchors.verticalCenter: transaction_fee_label.verticalCenter
        color: walletModel.isTestNet()?"#202020":"#FDA205"
        visible: minimized


    }

    CommonButton
    {
        id:transaction_fee_btn
        color: "#FDA205"
        anchors.left: minimized?transaction_fee.right:transaction_fee_label.right
        anchors.verticalCenter: transaction_fee_label.verticalCenter
        anchors.leftMargin: 15
        width: 85
        height: 32
        radius: 3
        text:!minimized?qsTr("Minimize"):qsTr("Choose...")
        textSize:12
        letterSpacing:0

        onClicked:
        {
            minimized = !minimized
            walletModel.coinControlProxy.setValue(0,minimized)
        }

        Component.onCompleted:
        {
            minimized = walletModel.coinControlProxy.getValue(0)
        }
    }

    ButtonGroup {
        id: radioGroup

    }
    ButtonGroup {
        id: radioGroup_custom

    }

    CommonRadioButton{
        id:recommended_btn
        anchors.top:transaction_fee_btn.bottom
        anchors.topMargin: 20
        anchors.left: transaction_fee_label.left
        ButtonGroup.group: radioGroup
        font.weight: Font.Medium
        font.pixelSize: 12

        height:35

        visible:!minimized

        onClicked:
        {
            walletModel.coinControlProxy.setValue(1,0)
        }

        Component.onCompleted:
        {
            checked = !walletModel.coinControlProxy.getValue(1)
        }
    }

    Label{
        id:confirmation_time_label
        font.weight: Font.Medium
        font.pixelSize:12
        anchors.left: recommended_btn.left
        anchors.leftMargin: 30
        anchors.top: recommended_btn.bottom
        color: "#FFFFFF"
        text:qsTr("Confirmation time")
        visible:!minimized
    }


    CommonSlider
    {
        id:confirmation_time_slider
        anchors.left: confirmation_time_label.right
        anchors.leftMargin: 8
        height:16
        anchors.verticalCenter: confirmation_time_label.verticalCenter
        enabled: recommended_btn.checked
        visible:!minimized
        from: 0
        stepSize: 1
        to: 24
        property int lastValue
        Component.onCompleted:
        {
            value = walletModel.coinControlProxy.getValue(3)
        }


        onPressedChanged:
        {
            if(pressed)
                lastValue = value
            else
            {
                if(lastValue !== value)
                    walletModel.coinControlProxy.setValue(3,value)
            }
        }
    }

    Label{
        id:confirmation_time_start_label
        font.weight: Font.Medium
        font.pixelSize:10
        anchors.left: confirmation_time_slider.left
        anchors.leftMargin: 5
        anchors.top: confirmation_time_slider.bottom
        anchors.topMargin: 0
        color: "#FFFFFF"
        text:qsTr("normal")
        visible:!minimized

    }

    Label{
        id:confirmation_time_end_label
        font.weight: Font.Medium
        font.pixelSize:10
        anchors.right: confirmation_time_slider.right
        anchors.rightMargin: 5
        anchors.top: confirmation_time_slider.bottom
        anchors.topMargin: 0
        color: "#FFFFFF"
        text:qsTr("fast")
        visible:!minimized

    }


    CommonRadioButton
    {
        id:custom_btn
        anchors.top:recommended_btn.bottom
        anchors.topMargin: 40
        anchors.left: transaction_fee_label.left
        text: qsTr("Custom:")
        ButtonGroup.group: radioGroup
        font.weight: Font.Medium
        font.pixelSize: 12
        visible:!minimized
        onClicked:
        {
            walletModel.coinControlProxy.setValue(1,1)
        }

        Component.onCompleted:
        {
            checked = walletModel.coinControlProxy.getValue(1)
        }
    }


    CommonRadioButton
    {
        id:perkilo_btn
        anchors.verticalCenter: custom_btn.verticalCenter
        anchors.left: custom_btn.right
        anchors.leftMargin: 62
        text: qsTr("per kilobyte")
        ButtonGroup.group: radioGroup_custom
        font.weight: Font.Medium
        font.pixelSize: 12
        visible:!minimized
        enabled:custom_btn.checked
        onClicked:
        {
            walletModel.coinControlProxy.setValue(2,0)
        }

        Component.onCompleted:
        {
            checked = !walletModel.coinControlProxy.getValue(2)
        }
    }

    CommonCheckBox {
        id:pay_minimum_checkbox
        font.weight: Font.Medium
        font.pixelSize: 12
        font.letterSpacing: 0.5
        anchors.left: perkilo_btn.left
        anchors.top:amountField.bottom
        height:20
        checked: false
        text: qsTr("Pay only the minimum fee of 0.00010000 GKC/kB  (read the tooltip)")
        visible:!minimized
        enabled:custom_btn.checked

        onClicked:
        {
            if(checked)
            {
                perkilo_btn.checked = true
                total_btn.checked = false
                total_btn.enabled = false
                amountField.valueAmount = walletModel.formatAmount(walletModel.getFeePerkilo(),amountField.coinTypeBtn.index)
            }
            else
            {
                total_btn.enabled = true
            }

            walletModel.coinControlProxy.setValue(5,checked)

        }

        Component.onCompleted:
        {
            checked = walletModel.coinControlProxy.getValue(5)
        }

    }

    CommonRadioButton
    {
        id:total_btn
        anchors.verticalCenter: custom_btn.verticalCenter
        anchors.leftMargin: 30
        anchors.left: perkilo_btn.right
        text: qsTr("total at least")
        ButtonGroup.group: radioGroup_custom
        font.weight: Font.Medium
        font.pixelSize: 12
        visible:!minimized
        enabled:custom_btn.checked && !pay_minimum_checkbox.checked
        onClicked:
        {
            walletModel.coinControlProxy.setValue(2,1)
        }
        Component.onCompleted:
        {
            checked = walletModel.coinControlProxy.getValue(2)
        }
    }

    AmountField
    {
        id:amountField
        anchors.verticalCenter: custom_btn.verticalCenter
        width: 300
        height: 32
        anchors.leftMargin: 10
        anchors.left: total_btn.right
        visible:!minimized
        enabled:custom_btn.checked && !pay_minimum_checkbox.checked


        onValueAmountChanged:
        {
            walletModel.coinControlProxy.setValue(4,walletModel.getFieldAmount(amountField.coinTypeBtn.index,valueAmount))
        }

        Component.onCompleted:
        {
            valueAmount = walletModel.formatAmount(walletModel.coinControlProxy.getValue(4),amountField.coinTypeBtn.index)
        }
    }



    CommonCheckBox {
        id:send_as_zero_checkbox
        font.weight: Font.Medium
        font.pixelSize: 12
        font.letterSpacing: 0.5
        anchors.left: custom_btn.left
        anchors.top:pay_minimum_checkbox.bottom

        checked: false
        text: qsTr("Send ad zero-free transaction if possible (confirmation may take longer)")

        visible:!minimized
        onClicked:
        {
            walletModel.coinControlProxy.setValue(6,checked)
        }

        Component.onCompleted:
        {
            checked = walletModel.coinControlProxy.getValue(6)
        }
    }

    CommonCheckBox {
        id:swiftx_btn
        font.weight: Font.Medium
        font.pixelSize: 12
        font.letterSpacing: 0.5
        anchors.left: transaction_fee_label.left
        anchors.top:  !minimized?send_as_zero_checkbox.bottom:transaction_fee_btn.bottom
        anchors.topMargin: 12
        checked: false
        text: qsTr("SwitfX")
        height: 17

    }


}
