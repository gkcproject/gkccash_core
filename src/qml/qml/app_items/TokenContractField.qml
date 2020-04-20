import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

Item {
    id:root

    function clearAll()
    {
        contractAddressField.text = ""
        tokenNameField.text = ""
        tokenSymbolField.text = ""
        decimalField.text = ""
        senderField.text  = ""
    }

    Label
    {
        id:contractAddressLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: root.top
        anchors.left: root.left

        anchors.topMargin: 10
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text: qsTr("Contract Address")
    }


    CommonTextField
    {
        id:contractAddressField
        anchors.verticalCenter: contractAddressLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: contractAddressLabel.left
        anchors.leftMargin: 120

        onTextChanged:
        {


            confirmBtn.enabled = false
            tokenNameField.text = ""
            tokenSymbolField.text = ""
            decimalField.text = ""

            if(contractAddressField.text === "")
            {
                return
            }


            var returnArray;
            returnArray = walletModel.tokenfilterproxy.addressChanged(contractAddressField.text)

            if(returnArray.length === 0)return

            tokenNameField.text = returnArray[0]
            tokenSymbolField.text = returnArray[1]
            decimalField.text = returnArray[2]

            confirmBtn.enabled = true



        }
    }


    Label
    {
        id:tokenNameLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: contractAddressLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Token Name")
    }


    CommonTextField
    {
        id:tokenNameField
        anchors.verticalCenter: tokenNameLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: tokenNameLabel.left
        anchors.leftMargin: 120
    }


    Label
    {
        id:tokenSymbolLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: tokenNameLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Token Symbol")
    }


    CommonTextField
    {
        id:tokenSymbolField
        anchors.verticalCenter: tokenSymbolLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: tokenSymbolLabel.left
        anchors.leftMargin: 120
    }

    Label
    {
        id:decimalLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: tokenSymbolLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Decimals")
    }


    CommonTextField
    {
        id:decimalField
        anchors.verticalCenter: decimalLabel.verticalCenter
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.left: decimalLabel.left
        anchors.leftMargin: 120
    }


    Label
    {
        id:senderLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top: decimalLabel.bottom
        anchors.left: root.left
        anchors.topMargin: 35
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Token Address")
    }

    AddressTextField
    {
        id:senderField
        anchors.verticalCenter: senderLabel.verticalCenter
        anchors.left: senderLabel.left
        anchors.leftMargin: 120
        anchors.right: root.right
        anchors.rightMargin: 20
        showDropImg:true

        hintmodel: walletModel.receivingAddressProxyModel
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

        onClicked:
        {

            if( contractAddressField.text.length === 0){
                contractAddressField.critical = true
                return
            }
            if( senderField.text.length === 0){
                senderField.critical = true
                return
            }

            var res
            res = walletModel.tokenfilterproxy.confirmAdd(contractAddressField.text,
                                                          tokenNameField.text,
                                                          tokenSymbolField.text,
                                                          decimalField.text,
                                                          senderField.text)

            if(res.length === 0 || res[0] === "Log events")
            {
                root.clearAll()
            }
            else
            {
                root_window.warningDialog.title = res[0]
                root_window.warningDialog.content_text = res[1]
                root_window.warningDialog.dim_back = false
                root_window.warningDialog.show()
            }


        }

    }

}
