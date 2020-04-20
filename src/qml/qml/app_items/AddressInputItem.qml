import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

Item
{
    id:root
    property string address_title
    signal rm_address_input()

    Label {
        id:title_label
        font.weight: Font.Medium
        font.pixelSize:11
        anchors.verticalCenter: textField.verticalCenter
        anchors.left: root.left
        color: "#333333"
        text:address_title
        font.letterSpacing: 0.3
    }


    CommonTextField
    {
        id:textField
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: amount_label.left
        anchors.left: title_label.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.top:root.top
    }

    Label
    {
        id:amount_label
        font.weight: Font.Medium
        font.pixelSize:11
        anchors.verticalCenter: textField.verticalCenter
        anchors.right: amount_field.left
        anchors.rightMargin: 2
        color: "#333333"
        text:"Amount:"
        font.letterSpacing: 0.3
    }

    CommonTextField
    {
        id:amount_field
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: coin_type_btn.left
        anchors.rightMargin: 5

        anchors.top:root.top
        width:60
        text:""
    }

    CoinTypeCoin
    {
        id:coin_type_btn
        anchors.right: images_rec.left
        anchors.top:root.top
        width:65

    }

    Rectangle
    {
        id:images_rec
        color: "#FFFFFF"
        height: 33
        width: 30
        anchors.right: root.right
        anchors.verticalCenter: textField.verticalCenter

        RowLayout{
            anchors.centerIn: parent

            spacing: 12

            Image{
                id:delete_img
                source: "qrc:/images/icons/delete.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:16
                Layout.preferredWidth:16
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 20; height: 20

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
                        rm_address_input()
                    }
                }

            }

        }

    }









}
