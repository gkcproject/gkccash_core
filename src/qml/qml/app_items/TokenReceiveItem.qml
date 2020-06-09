import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

Item {
    id:root

    property alias qRImg: qRImg
    property alias titleLabel: titleLabel
    property alias addressLabel: addressLabel




    Image{
        id:qRImg
        cache:false
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 20
        width: 300
        height:300
        property bool pressed
        property bool down
        property bool hovered
        Ripple {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 22; height: 22

            z: -1
            anchor: qRImg
            pressed: qRImg.pressed
            active: qRImg.down || qRImg.visualFocus || qRImg.hovered
            color: qRImg.Material.rippleColor
        }

    }





    Label
    {
        id:titleLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.verticalCenter: qRImg.verticalCenter
        anchors.verticalCenterOffset: -30
        anchors.left: qRImg.right
        anchors.leftMargin: 20
        color: "#FFFFFF"
    }

    Label
    {
        id:addressLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.verticalCenter: qRImg.verticalCenter
        anchors.verticalCenterOffset: 30
        anchors.left: qRImg.right
        anchors.leftMargin: 20
        color: "#FFFFFF"
    }



    CommonButton
    {
        id:copyAddressBtn
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.bottom:root.bottom
        anchors.bottomMargin:10
        implicitWidth: 100
        text:qsTr("Copy Address")
        color: "#FDA205"
		ToolTip
        {
            id:tooltip_paste
            text:qsTr("Pasted")
        }
        
        onClicked:
        {
        	walletModel.setClipBoard(addressLabel.text)
        	tooltip_paste.visible = true
        }
    }





}
