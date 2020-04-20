import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

Rectangle {
    id:control
    implicitHeight: 32
    implicitWidth: 83
    radius: 3

    property bool pressed
    property bool hovered

    property alias text: text_label.text
    property alias textColor: text_label.color
    property alias textSize: text_label.font.pixelSize
    property alias letterSpacing: text_label.font.letterSpacing
    property alias label: text_label

    signal  clicked()

    Label{
        id: text_label
        font.pixelSize: 12
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color:"#FFFFFF"
        font.letterSpacing: 0
    }


    Ripple {
        clipRadius: 2
        width: parent.width
        height: parent.height
        pressed: control.pressed
        anchor: control
        active: control.down || control.visualFocus || control.hovered
        color: control.Material.rippleColor
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        onEntered:control.hovered=true
        onExited:control.hovered=false
        onPressed: control.pressed=true
        onReleased: control.pressed=false
        onClicked: control.clicked()


    }



}
