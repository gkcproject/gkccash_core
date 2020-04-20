import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

Item {
    id:root
    height:total_zerocoin_label.y+total_zerocoin_label.height
    Label {
        id:sending_label
        font.weight: Font.Medium
        font.pixelSize:15
        anchors.verticalCenter: sending_textField.verticalCenter
        anchors.left: root.left
        color: "#FFFFFF"
        text:qsTr("Sending")
    }


    CommonTextField
    {
        id:sending_textField
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: images_rec.left
        anchors.left: root.left
        anchors.leftMargin: 60
        anchors.top:root.top

    }

    Rectangle{
        id:images_rec
        color: sending_textField.focus?"#3B4674":"#BB3B4674"
        height: 33
        width:  65
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
                }
            }
            Image{
                id:edit_img
                source: "qrc:/images/icons/edit.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:15
                Layout.preferredWidth:15
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 19; height: 19

                    z: -1
                    anchor: edit_img
                    pressed: edit_img.pressed
                    active: edit_img.down || edit_img.visualFocus || edit_img.hovered
                    color: edit_img.Material.rippleColor
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

                }

            }

        }

    }



    Label {
        id:tag_label
        font.weight: Font.Medium
        font.pixelSize:15
        anchors.top:sending_label.bottom
        anchors.left: root.left
        anchors.topMargin: 25
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
        anchors.leftMargin: 60

    }

    Label {
        id:total_label
        font.weight: Font.Medium
        font.pixelSize:15
        anchors.top:tag_label.bottom
        anchors.left: root.left
        anchors.topMargin: 25
        color: "#FFFFFF"
        text:qsTr("Total")
    }


    CommonTextField
    {
        id:total_textField
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: total_zerocoin_label.left
        anchors.verticalCenter: total_label.verticalCenter
        anchors.left: root.left
        anchors.leftMargin: 60

    }

    Label {
        id:total_zerocoin_label
        font.weight: Font.Medium
        font.pixelSize:13
        anchors.verticalCenter: total_textField.verticalCenter
        verticalAlignment: Label.AlignVCenter
        anchors.right: root.right
        height:total_textField.height
        color: "#FFFFFF"
        text:"zGKC"
        background: Rectangle
        {
            color:total_textField.focus?"#3B4674":"#BB3B4674"
        }
    }



}
