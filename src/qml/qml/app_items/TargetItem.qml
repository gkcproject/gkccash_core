import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

Item {
    id:root

    signal  deleteTarget()
    signal  pickAddress()
    signal updateSeveral()

    function clearFirst()
    {
        sending_textField.text = ""
        tag_textField.text = ""
        amountField.amountField.text = ""
    }

    function setAddress(address)
    {
        sending_textField.text = address
    }

    function getPayMent()
    {
        return amountField.getFieldAmount()
    }


    function infoIsReady()
    {
       if(!walletModel.validateAddress(sending_textField.text))
       {
           sending_textField.critical = true
           return false
       }

       if(walletModel.validateAmount(amountField.currentUnit, amountField.amountField.text) === 0)
       {
            amountField.amountField.critical = true
           return false
       }

       return true

    }

    function getRecipient()
    {
        var array = new Array

        array.push(sending_textField.text)
        array.push(tag_textField.text)
        array.push(walletModel.getFieldAmount(amountField.coinTypeBtn.index, amountField.valueAmount))
        array.push("")

        return array

    }


    Label
    {
        id:sending_label
        font.weight: Font.Medium
        font.pixelSize:13
        anchors.verticalCenter: sending_textField.verticalCenter
        anchors.left: root.left
        anchors.leftMargin: 10
        color: "#FFFFFF"
        text:qsTr("Send")

    }

    CommonTextField
    {
        id:sending_textField
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: images_rec.left
        anchors.left: root.left
        anchors.leftMargin: 62
        anchors.top:root.top

        onTextChanged:
        {
          tag_textField.text = walletModel.sendingAddressProxyModel.updateLabel(sending_textField.text)

            if(!walletModel.validateAddress(sending_textField.text))
            {
                sending_textField.critical = true
                return
            }
          updateSeveral()
        }

        placeholderText: "Enter a GKC address (e.g. u7VFR83SQbiezrW72hjcWJtcfip5krte2Z)"
    }

    Rectangle{
        id:images_rec
        color: "#37416D"
        height: 33
        width: 100
        anchors.right: root.right
        anchors.verticalCenter: sending_textField.verticalCenter

        RowLayout{
            anchors.centerIn: parent

            spacing: 12
            Image{
                id:contact_img
                source: "qrc:/images/icons/contact.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:18
                Layout.preferredWidth:18
                property bool pressed
                property bool down
                property bool hovered

                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 22; height: 22

                    z: -1
                    anchor: contact_img
                    pressed: contact_img.pressed
                    active: contact_img.down || contact_img.visualFocus || contact_img.hovered
                    color: contact_img.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        contact_img.hovered = true
                    }

                    onExited:
                    {
                        contact_img.hovered = false
                    }

                    onReleased:
                    {
                        contact_img.pressed = false
                    }

                    onPressed:
                    {
                        contact_img.pressed = true
                    }

                    onClicked:
                    {
                        root.pickAddress()
                    }
                }
            }
            Image{
                id:edit_img
                source: "qrc:/images/icons/copyContract.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:18
                Layout.preferredWidth:18
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 22; height: 22

                    z: -1
                    anchor: edit_img
                    pressed: edit_img.pressed
                    active: edit_img.down || edit_img.visualFocus || edit_img.hovered
                    color: edit_img.Material.rippleColor
                }


                ToolTip
                {
                    id:tooltip_paste
                    text:qsTr("Pasted")

                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        edit_img.hovered = true
                    }

                    onExited:
                    {
                        edit_img.hovered = false
                    }

                    onReleased:
                    {
                        edit_img.pressed = false
                    }

                    onPressed:
                    {
                        edit_img.pressed = true
                    }

                    onClicked:
                    {
                        sending_textField.text = walletModel.getClipBoard("string")
                        tooltip_paste.visible = true
                    }


                }

            }
            Image{
                id:delete_img
                source: "qrc:/images/icons/delete.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:18
                Layout.preferredWidth:18
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 22; height: 22

                    z: -1
                    anchor: delete_img
                    pressed: delete_img.pressed
                    active: delete_img.down || delete_img.visualFocus || delete_img.hovered
                    color: delete_img.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        delete_img.hovered = true
                    }

                    onExited:
                    {
                        delete_img.hovered = false
                    }

                    onReleased:
                    {
                        delete_img.pressed = false
                    }

                    onPressed:
                    {
                        delete_img.pressed = true
                    }

                    onClicked:
                    {
                        root.deleteTarget()
                    }
                }

            }

        }

    }



    Label {
        id:tag_label
        font.weight: Font.Medium
        font.pixelSize:13
        anchors.top:sending_label.bottom
        anchors.left: root.left
        anchors.leftMargin: 10
        anchors.topMargin: 28
        color: "#FFFFFF"
        text:qsTr("Label")
    }


    CommonTextField
    {
        id:tag_textField
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: root.right
        anchors.verticalCenter: tag_label.verticalCenter
        anchors.left: root.left
        anchors.leftMargin: 62

        placeholderText: qsTr("Enter a label for this address to add it to your address book")

    }

    Label {
        id:total_label
        font.weight: Font.Medium
        font.pixelSize:13
        anchors.top:tag_label.bottom
        anchors.left: root.left
        anchors.leftMargin: 2
        anchors.topMargin: 28
        verticalAlignment: Label.AlignVCenter
        horizontalAlignment: Label.AlignHCenter

        width:40
        height:25
        color: "#FFFFFF"
        background: Rectangle {
            color: "#9A9A9A"
            border.color: "#67BED4"
            border.width:2
            radius: 2
        }
        text:qsTr("Total")
    }


    AmountField
    {
        id:amountField
        anchors.verticalCenter: total_label.verticalCenter
        width: 300
        height: 32
        anchors.left: root.left
        anchors.leftMargin: 62
    }

}
