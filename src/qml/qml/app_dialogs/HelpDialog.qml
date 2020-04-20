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
    title: qsTr("Command-line options")

    confrim_btn_visible:true
    confirm_btn_text: qsTr("Ok")
    cancel_btn_visible:false
    width:700
    height:800

    Component.onCompleted:
    {
        root.width = 700
        root.height = 800
    }

    onConfirmed:
    {
        root.close()
    }

    Item{
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 25

        Image{
            id:logoImg
            source: "qrc:/images/icons/about.png"
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -25
            anchors.left: parent.left
            anchors.leftMargin: 5

            width:50
            height:300

        }

        CommonTextArea
        {
            id:helpArea
            font.weight: Font.Light
            font.pixelSize:12
            anchors.left: logoImg.right
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            anchors.bottomMargin: 60
            textFormat: Qt.RichText
            readOnly: true

            wrapMode: TextEdit.Wrap

            Component.onCompleted:
            {
                utility.getHelpMessage(helpArea.textDocument)
                helpArea.cursorPosition = -1
            }




        }
    }

}
