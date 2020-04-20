import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4

import QtGraphicalEffects 1.0

import "../app_items"

Controls_1_4.Tab {

    Rectangle {
        anchors.fill:parent

        radius: 0
        color: "#212b50"


        Label {
            id:zero_actions_label
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 20
            anchors.leftMargin: 30
            color: "#FFFFFF"
            text:qsTr("Zerocoin Actions:")

        }

        //First Line
        CommonButton
        {
            id:mint_zero_btn
            color: "#FDA205"
            anchors.left: zero_actions_label.left
            anchors.top: zero_actions_label.bottom
            anchors.topMargin: 15
            width: 110
            height: 32
            radius: 3
            text:qsTr("Mint Zerocoin")
            textSize:12
            letterSpacing:0
        }

        CommonTextField
        {
            id:mint_zero_textField
            font.weight: Font.Light
            font.pixelSize: 16
            anchors.left: mint_zero_btn.right
            anchors.leftMargin: 8
            anchors.verticalCenter: mint_zero_btn.verticalCenter
            width:165
            text:"0"
        }


        Label
        {
            id:mint_zero_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.left: mint_zero_textField.right
            anchors.leftMargin: 8
            anchors.verticalCenter: mint_zero_btn.verticalCenter
            text:qsTr("zGKC Available for Minting: 0.00 GKC")
            color: "#FFFFFF"
        }

        CommonButton
        {
            id:mint_zero_reset_btn
            color: "#FDA205"
            anchors.left: mint_zero_label.right
            anchors.leftMargin: 30
            anchors.verticalCenter: mint_zero_btn.verticalCenter
            width: 65
            height: 32
            radius: 3
            text:qsTr("Reset")
            textSize:12
            letterSpacing:0
        }


        //Second Line
        CommonButton
        {
            id:coin_control_btn
            color: "#FDA205"
            anchors.left: mint_zero_btn.left
            anchors.top: mint_zero_btn.bottom
            anchors.topMargin: 15
            width: 110
            height: 32
            radius: 3
            text:qsTr("Coin Control")
            textSize:12
            letterSpacing:0
        }

        Label
        {
            id:quantity_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.left: coin_control_btn.right
            anchors.leftMargin: 10
            anchors.verticalCenter: coin_control_btn.verticalCenter
            anchors.verticalCenterOffset: -12
            color: "#FFFFFF"
            text:qsTr("<b>Quantity:</b>") +qsTr("Coins automatically selected")
        }

        Label
        {
            id:amount_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.left: coin_control_btn.right
            anchors.leftMargin: 10
            anchors.verticalCenter: coin_control_btn.verticalCenter
            anchors.verticalCenterOffset: 12
            color: "#FFFFFF"
            text:qsTr("<b>Amount:</b>&nbsp;&nbsp;") + qsTr("Coins automatically selected")
        }


        CommonButton
        {
            id:rescan_btn
            color: "#FDA205"
            anchors.left: mint_zero_reset_btn.left
            anchors.verticalCenter: coin_control_btn.verticalCenter
            width: 65
            height: 32
            radius: 3
            text:qsTr("ReScan")
            textSize:12
            letterSpacing:0
        }

        //Third Line
        Label
        {
            id:zero_action_textField
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.355
            anchors.left: mint_zero_btn.left
            anchors.top: coin_control_btn.bottom
            anchors.topMargin: 30
            anchors.right: rescan_btn.right
            height: 101
            text:qsTr("ResetMintZerocoin finished: 0 mints updated, 0 mints deleted\nDuration: 0 sec.")
            topPadding: 8
            color:"#FFFFFF"
            leftPadding: 10
            background: Rectangle
            {
                color:"#3B4674"

            }
        }

        //Fourth Line
        CommonButton
        {
            id:ztsr_control_btn
            color: "#FDA205"
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: zero_action_textField.bottom
            anchors.topMargin: 10
            width: 110
            height: 32
            radius: 3
            text:qsTr("zGKC Control")
            textSize:12
            letterSpacing:0
        }


        Label
        {
            id:ztsr_selected_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.left: ztsr_control_btn.right
            anchors.leftMargin: 28
            anchors.verticalCenter: ztsr_control_btn.verticalCenter
            text:qsTr("zGKC Selected:")+ "0"
            color: "#FFFFFF"
        }


        Label
        {
            id:quantity_selected_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.left: security_level_label.left
            anchors.rightMargin: 5
            anchors.verticalCenter: ztsr_control_btn.verticalCenter
            color: "#FFFFFF"
            text:qsTr("Quantity Selected:")+ "0"
        }

        //Fifth Line
        CommonButton
        {
            id:spend_zerocoin_btn
            color: "#FDA205"
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: ztsr_control_btn.bottom
            anchors.topMargin: 10
            width: 120
            height: 32
            radius: 3
            text:qsTr("Spend Zerocoin")
            textSize:12
            letterSpacing:0
        }


        Label
        {
            id:spend_zerocoin_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.left: spend_zerocoin_btn.right
            anchors.leftMargin: 18
            anchors.verticalCenter: spend_zerocoin_btn.verticalCenter
            text:qsTr("Availabel Balance: <b>")+ "0&nbsp;&nbsp;zGKC</b> "
            color: "#FFFFFF"
        }


        Label
        {
            id:security_level_label
            font.weight: Font.Normal
            font.pixelSize:13
            font.letterSpacing: 0.20
            anchors.right: security_leve_textField.left
            anchors.rightMargin: 5
            anchors.verticalCenter: spend_zerocoin_btn.verticalCenter
            color: "#FFFFFF"
            text:qsTr("Security level:")
        }

        CommonTextField
        {
            id:security_leve_textField
            font.weight: Font.Light
            font.pixelSize: 16
            anchors.right: rescan_btn.right
            anchors.verticalCenter: spend_zerocoin_btn.verticalCenter
            width:130
            text:"12"
        }

        //Target  Line

        TargetItem2
        {
            id:target_item
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top:spend_zerocoin_btn.bottom
            anchors.topMargin: 20
            anchors.right: rescan_btn.right

        }

        CommonCheckBox {
            id:convert_change_checkbox
            font.weight: Font.Medium
            font.pixelSize: 11
            font.letterSpacing: 0.5
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top:target_item.bottom
            anchors.topMargin: 5
            checked: true
            text: qsTr("Convert change to Zerocoin (might cost additional fees)")
        }

        CommonCheckBox {
            id:minimize_change_checkbox
            font.weight: Font.Medium
            font.pixelSize: 11
            font.letterSpacing: 0.5
            anchors.left: convert_change_checkbox.right
            anchors.leftMargin: 90
            anchors.verticalCenter: convert_change_checkbox.verticalCenter
            checked: false
            text: qsTr("Minimize change")
        }

        //Right Part

        Label {
            id:zero_stats_label
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.top:parent.top
            anchors.right: parent.right
            anchors.topMargin: 20
            anchors.rightMargin: 480
            color: "#FFFFFF"
            text:qsTr("Zerocoin Stats:")
        }

        Label {
            id:total_zero_balance_label
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.top:zero_stats_label.bottom
            anchors.left: zero_stats_label.left
            anchors.topMargin: 20
            color: "#FFFFFF"
            text:qsTr("Total Zerocoin Balance:")
        }

        Label {
            id:total_zero_balance_amount_label
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.verticalCenter:total_zero_balance_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"<b>0 zGKC</b>"
            color: "#FFFFFF"
        }

        Label {
            id:amount_1_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:total_zero_balance_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>1</b>:")
        }

        Label {
            id:amount_1_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_1_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 1 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }

        Label {
            id:amount_5_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_1_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>5</b>:")
        }

        Label {
            id:amount_5_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_5_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 5 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }

        Label {
            id:amount_10_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_5_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>10</b>:")
        }

        Label {
            id:amount_10_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_10_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 10 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }

        Label {
            id:amount_50_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_10_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>50</b>:")
        }

        Label {
            id:amount_50_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_50_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 50 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }

        Label {
            id:amount_100_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_50_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>100</b>:")
        }

        Label {
            id:amount_100_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_100_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 100 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }


        Label {
            id:amount_500_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_100_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>500</b>:")
        }

        Label {
            id:amount_500_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_500_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 500 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }



        Label {
            id:amount_1000_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_500_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>1000</b>:")
        }

        Label {
            id:amount_1000_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_1000_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 1000 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }


        Label {
            id:amount_5000_title_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: zero_stats_label.left
            anchors.top:amount_1000_title_label.bottom
            anchors.topMargin: 10
            color: "#FFFFFF"
            text:qsTr("Denom. with value <b>5000</b>:")
        }

        Label {
            id:amount_5000_label
            font.weight: Font.Light
            font.pixelSize:13
            anchors.verticalCenter:amount_5000_title_label.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 30
            text:"0 X 5000 = <b>0 zGKC</b>"
            color: "#FFFFFF"
        }


    }

}
