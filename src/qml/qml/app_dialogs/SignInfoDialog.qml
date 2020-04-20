import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import "../app_pages"
import "../app_items"
import "../base_items"


CommonDialog
{
    id:root
    title: qsTr("Signature-sign/verify message")

    confrim_btn_visible:false
    cancel_btn_visible:false
    property alias current_index:tabview.currentIndex

    width:642
    height:395

    Item{
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 25

        CommonTabView{
            id:tabview
            tab_width:100

            Controls_1_4.Tab {
                title: qsTr("Sign message")


                Rectangle{
                    id:tab1_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:sign_label
                        anchors.top:parent.top
                        anchors.topMargin: 10
                        width:parent.width
                        text: qsTr("You can sign messages with your addresses to prove you own them. Be careful not to sign anything vague, as phishing attacks may try to trick you into signing your identity over to them. Only sign fully-detailed statements you agree to.")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }

                    CommonTextField
                    {
                        id:targetItem
                        font.weight: Font.Light
                        font.pixelSize:16
                        anchors.right: images_rec.left
                        anchors.top: sign_label.bottom
                        anchors.topMargin: 17
                        anchors.left: parent.left
                    }

                    Rectangle{
                        id:images_rec
                        color: "#FFFFFF"
                        height: 33
                        width:  65
                        anchors.right: tab1_rec.right
                        anchors.verticalCenter: targetItem.verticalCenter

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

                    CommonTextArea
                    {
                        id:message_box

                        anchors.right: parent.right
                        anchors.top: targetItem.bottom
                        anchors.topMargin: 9
                        anchors.left: parent.left
                        height: 100

                    }

                    CommonTextField
                    {
                        id:generate_sign_text
                        font.weight: Font.Light
                        font.pixelSize:12
                        anchors.right: images_rec2.left
                        anchors.top: message_box.bottom
                        anchors.topMargin: 9
                        anchors.left: parent.left
                        placeholderText: qsTr("Click \"Sign Message\" to generate signature")
                    }

                    Rectangle{
                        id:images_rec2
                        color: "#FFFFFF"
                        height: 33
                        width:  32
                        anchors.right: tab1_rec.right
                        anchors.verticalCenter: generate_sign_text.verticalCenter

                        RowLayout{
                            anchors.centerIn: parent

                            spacing: 12
                            Image{
                                id:copy_img
                                source: "qrc:/images/icons/copy.png"
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
                                    anchor: copy_img
                                    pressed: copy_img.pressed
                                    active: copy_img.down || copy_img.visualFocus || copy_img.hovered
                                    color: copy_img.Material.rippleColor
                                }

                                MouseArea
                                {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered:
                                    {
                                        copy_img.hovered = true
                                    }

                                    onExited:
                                    {
                                        copy_img.hovered = false
                                    }

                                    onReleased:
                                    {
                                        copy_img.pressed = false
                                    }

                                    onPressed:
                                    {
                                        copy_img.pressed = true
                                    }
                                }
                            }
                        }

                    }


                    CommonButton
                    {
                        id:sign_btn
                        color: "#EE637F"
                        anchors.right: del_btn.left
                        anchors.rightMargin: 8
                        anchors.verticalCenter: del_btn.verticalCenter
                        text:qsTr("Sign")
                        height: 30
                        onClicked: {
                        }
                    }

                    CommonButton
                    {
                        id:del_btn
                        color: "#469AAC"
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        height: 30
                        text:qsTr("Clear All")
                        onClicked: {
                        }
                    }

                }

            }
            Controls_1_4.Tab {
                title: qsTr("Verify Message")


                Rectangle{
                    id:tab2_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:validate_label
                        anchors.top:parent.top
                        anchors.topMargin: 10
                        width:parent.width
                        text: qsTr("Enter the signing address, message (ensure you copy line breaks, spaces, tabs, etc. exactly) and signature below to verify the message. Be careful not to read more into the signature than what is in the signed message itself, to avoid being tricked by a man-in-the-middle attack.")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }

                    CommonTextField
                    {
                        id:targetItem2
                        font.weight: Font.Light
                        font.pixelSize:16
                        anchors.right: images_rec3.left
                        anchors.top: validate_label.bottom
                        anchors.topMargin: 17
                        anchors.left: parent.left
                    }

                    Rectangle{
                        id:images_rec3
                        color: "#FFFFFF"
                        height: 33
                        width:  32
                        anchors.right: tab2_rec.right
                        anchors.verticalCenter: targetItem2.verticalCenter

                        RowLayout{
                            anchors.centerIn: parent

                            spacing: 12
                            Image{
                                id:contact_img2
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
                                    anchor: contact_img2
                                    pressed: contact_img2.pressed
                                    active: contact_img2.down || contact_img2.visualFocus || contact_img2.hovered
                                    color: contact_img2.Material.rippleColor
                                }

                                MouseArea
                                {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered:
                                    {
                                        contact_img2.hovered = true
                                    }

                                    onExited:
                                    {
                                        contact_img2.hovered = false
                                    }

                                    onReleased:
                                    {
                                        contact_img2.pressed = false
                                    }

                                    onPressed:
                                    {
                                        contact_img2.pressed = true
                                    }
                                }
                            }


                        }

                    }

                    CommonTextArea
                    {
                        id:message_box2

                        anchors.right: parent.right
                        anchors.top: targetItem2.bottom
                        anchors.topMargin: 9
                        anchors.left: parent.left
                        height: 100

                    }

                    CommonTextField
                    {
                        id:validate_sign_text
                        font.weight: Font.Light
                        font.pixelSize:12
                        anchors.right: parent.right
                        anchors.top: message_box2.bottom
                        anchors.topMargin: 9
                        anchors.left: parent.left
                        placeholderText: ""
                    }




                    CommonButton
                    {
                        id:validate_btn
                        color: "#EE637F"
                        anchors.right: del_btn2.left
                        anchors.rightMargin: 8
                        anchors.verticalCenter: del_btn2.verticalCenter
                        text:qsTr("Verify")
                        height: 30
                        onClicked: {
                        }
                    }

                    CommonButton
                    {
                        id:del_btn2
                        color: "#469AAC"
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 0
                        height: 30
                        text:qsTr("Clear All")
                        onClicked: {
                        }
                    }

                }

            }
        }

    }

}
