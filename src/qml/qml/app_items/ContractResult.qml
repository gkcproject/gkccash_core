import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

import "../app_pages"

Rectangle
{
    id:root
    color: "#FAFAFA"

    property string resultStr
    Component.onCompleted:
    {
        resultStr = walletModel.contractPage.getLastResult()
    }

    Label
    {
        id:title
        font.weight: Font.Bold
        font.pixelSize:15
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "#333333"
        text:qsTr("Result")
    }

    CommonTextArea
    {
        id:resultText
        font.weight: Font.Bold
        font.pixelSize:15
        anchors.top:title.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.margins: 20
        anchors.topMargin: 10

        textColor: "#333333"
        textFormat:Text.RichText
        text:root.resultStr
        readOnly: true
    }





}
