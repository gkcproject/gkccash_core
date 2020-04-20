import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0

import "../app_pages"
import "../app_items"
import "../base_items"

CommonDialog
{
    id:root
    modality: Qt.ApplicationModal
    width:700

    property bool modify: false
    property bool setting: false
    height:300

    cancel_btn_text: qsTr("Cancel")
    confirm_btn_text: qsTr("Ok")



    title: setting?qsTr("Setting Password"):(modify?qsTr("Modify Password"):qsTr("Enter Password"))
    onClosing:
    {
        passwordField.text = ""
        secondPasswordField.text = ""
        thirdPasswordField.text = ""
        modify = false
        setting = false
    }


    Item{
        parent:root.background_rec
        anchors.fill: parent

        Label {
            id:passwordLabel
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.topMargin: 70
            color: "#333333"
            text:setting?qsTr("Please Enter Password:"):(modify?qsTr("Please Enter Old Password:"):qsTr("Please Enter Password:"))
        }


        CommonTextField
        {
            id:passwordField
            font.weight: Font.Light
            font.pixelSize:16
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: passwordLabel.verticalCenter
            anchors.left: passwordLabel.right
            anchors.leftMargin: 10
            echoMode:TextInput.Password
            validator:RegExpValidator { regExp: /^[a-zA-Z0-9]{6,20}$/ }
        }

        Label {
            id:secondPasswordLabel
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:passwordLabel.bottom
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.topMargin: 30
            color: "#333333"
            visible:setting||modify
            text:setting?qsTr("Please Enter Password Again:"):qsTr("Please Enter New Password:")

        }


        CommonTextField
        {
            id:secondPasswordField
            font.weight: Font.Light
            font.pixelSize:16
            anchors.rightMargin: 30
            anchors.right: parent.right
            anchors.verticalCenter: secondPasswordLabel.verticalCenter
            anchors.left: secondPasswordLabel.right
            anchors.leftMargin: 10
            visible:setting||modify
            echoMode:TextInput.Password
            validator:RegExpValidator { regExp: /^[a-zA-Z0-9]{6,20}$/ }
        }

        Label {
            id:thirdPasswordLabel
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:secondPasswordLabel.bottom
            anchors.left: parent.left
            anchors.leftMargin: 15
            anchors.topMargin: 30
            color: "#333333"
            visible:modify
            text:qsTr("Please Enter Password Again:")
        }


        CommonTextField
        {
            id:thirdPasswordField
            font.weight: Font.Light
            font.pixelSize:16
            anchors.rightMargin: 30
            anchors.right: parent.right
            anchors.verticalCenter: thirdPasswordLabel.verticalCenter
            anchors.left: thirdPasswordLabel.right
            anchors.leftMargin: 10
            visible:modify
            echoMode:TextInput.Password
            validator:RegExpValidator { regExp: /^[a-zA-Z0-9]{6,20}$/ }
        }



    }


    onConfirmed:
    {
        if(setting)
        {
            if(passwordField.text.trim().length < 6)
            {
                passwordField.critical = true
                passwordField.focus = true
                passwordField.ToolTip.visible = true
                passwordField.ToolTip.text = qsTr("at least 6 words")
                return
            }
            if(passwordField.text.trim() !=  secondPasswordField.text.trim())
            {
                secondPasswordField.critical = true
                secondPasswordField.focus = true
                secondPasswordField.ToolTip.visible = true
                secondPasswordField.ToolTip.text = qsTr("password not match")
                return
            }

            walletModel.setWalletEncrypted(passwordField.text.trim())
            close_dialog()
            rebuildSettingMenu()
        }
        else if(modify)
        {
            if(passwordField.text.trim().length < 6)
            {
                passwordField.critical = true
                passwordField.focus = true
                passwordField.ToolTip.visible = true
                passwordField.ToolTip.text = qsTr("at least 6 words")
                return
            }
            if(secondPasswordField.text.trim().length < 6)
            {
                secondPasswordField.critical = true
                secondPasswordField.focus = true
                secondPasswordField.ToolTip.visible = true
                secondPasswordField.ToolTip.text = qsTr("at least 6 words")
                return
            }
            if(thirdPasswordField.text.trim() !=  secondPasswordField.text.trim())
            {
                thirdPasswordField.critical = true
                thirdPasswordField.focus = true
                thirdPasswordField.ToolTip.visible = true
                thirdPasswordField.ToolTip.text = qsTr("password not match")
                return
            }


            var res = walletModel.changePassphrase(passwordField.text.trim(),secondPasswordField.text.trim())

            if(res)
            {
                root_window.warningDialog.title = qsTr("succeed")
                root_window.warningDialog.content_text = qsTr("password changed successfully")
                root.close()
                root_window.warningDialog.show()
            }
            else
            {
                root_window.warningDialog.title = qsTr("failed")
                root_window.warningDialog.content_text = qsTr("check your password")
                root_window.warningDialog.show()
            }
			rebuildSettingMenu()
        }
        else
        {
            if(walletModel.getEncryptionStatus() === 1)
            {
                if(passwordField.text.trim().length < 6)
                {
                    passwordField.critical = true
                    passwordField.focus = true
                    passwordField.ToolTip.visible = true
                    passwordField.ToolTip.text = qsTr("at least 6 words")
                    return
                }
                var res2 = walletModel.setWalletLocked(false, passwordField.text.trim())

                if(res2)
                {
                    root_window.warningDialog.title = qsTr("succeed")
                    root_window.warningDialog.content_text = qsTr("wallet unlocked successfully")
                    root.close()
                    root_window.warningDialog.show()
                }
                else
                {
                    root_window.warningDialog.title = qsTr("failed")
                    root_window.warningDialog.content_text = qsTr("check your password")
                    root_window.warningDialog.show()
                }


            }








            rebuildSettingMenu()
        }






    }

}
