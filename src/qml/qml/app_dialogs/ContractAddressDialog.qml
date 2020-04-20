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
    width:600
    height: 500
    property var model
    property bool editing: false
    cancel_btn_text: qsTr("Cancel")
    confirm_btn_text: qsTr("Ok")
    property int currentRow: -1

    property alias tagTextField: tag_textField
    property alias addressTextField: addressTextField
    property alias abiTextArea: abiTextArea

    title: editing?qsTr("Modify Contract Address"):qsTr("New Contract Address")

    onClosing:
    {
        tag_textField.text = ""
        addressTextField.text = ""
        close_dialog()
    }




    Item{
        parent:root.background_rec
        anchors.fill: parent

        Label {
            id:tag_label
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 70
            color: "#333333"
            text:qsTr("Label")
        }


        CommonTextField
        {
            id:tag_textField
            font.weight: Font.Light
            font.pixelSize:16
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: tag_label.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 120
        }

        Label {
            id:address_label
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:tag_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 30
            color: "#333333"
            text:qsTr("Contract Address")
        }


        CommonTextField
        {
            id:addressTextField
            font.weight: Font.Light
            font.pixelSize:16
            anchors.rightMargin: 30

            anchors.right: parent.right
            anchors.verticalCenter: address_label.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 120
        }

        Label {
            id:abiLabel
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:address_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 30
            color: "#333333"
            text:qsTr("Interface (ABI)")
        }


        CommonTextArea
        {
            id:abiTextArea
            font.weight: Font.Light
            font.pixelSize:16
            anchors.rightMargin: 30
            anchors.right: parent.right
            anchors.top: abiLabel.top
            anchors.left: parent.left
            anchors.leftMargin: 120
            anchors.bottom:parent.bottom
            anchors.bottomMargin: 50

        }



    }




    onConfirmed:
    {
        if(addressTextField.text.length != 40)
        {
            addressTextField.critical = true
            return
        }


        var res = walletModel.contractfilterproxy.saveContract(editing?1:0,
                                                              tag_textField.text,
                                                              addressTextField.text,
                                                              abiTextArea.text,
                                                              currentRow)
        if(res !== "ok")
        {
            root_window.warningDialog.title = editing?qsTr("Fail modifying"):qsTr("Fail adding")
            root_window.warningDialog.content_text = res
            root_window.warningDialog.dim_back = false
            root_window.warningDialog.show()
            return
        }


        root.close()
    }

    onCanceled:
    {
        root.close()
    }

}
