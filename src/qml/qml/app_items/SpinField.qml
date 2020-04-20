import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3


Item
{
    id:root
    implicitHeight: 32
    implicitWidth: 200
    property alias amountField: amountField

    property int upORdown: 0
    property int pageType

    property int create:0
    property int sendTo:1


    property int defaultAmount: -1

    Timer{
        id:changeTimer
        triggeredOnStart: false
        repeat: true
        interval: 100
        onTriggered: {
            if(upORdown != 0)
            {
                amountField.text = parseInt(amountField.text) + upORdown
                return
            }
            changeTimer.stop()
        }
    }



    CommonTextField
    {
        id:amountField
        font.weight: Font.Light
        font.pixelSize:16
        anchors.right: spinRec.left
        anchors.rightMargin: 0
        anchors.left: root.left
        horizontalAlignment:TextInput.AlignRight
        anchors.verticalCenter: root.verticalCenter
        validator:DoubleValidator{
            bottom: 0; top: 10000000000;
            notation:DoubleValidator.StandardNotation
            decimals:8
        }

        Component.onCompleted:
        {
            if(defaultAmount === -1)
            {
                if(pageType === create)
                    text = walletModel.contractPage.getDefaultGasLimitOpCreate()
                else
                    text = walletModel.contractPage.getDefaultGasLimitOpSend()

            }
            else
                text = defaultAmount


        }


    }

    Rectangle
    {
        id:spinRec
        color: amountField.focus?"#3B4674":"#BB3B4674"
        height: amountField.height
        width: 15
        anchors.right: parent.right
        anchors.verticalCenter: root.verticalCenter

       ColumnLayout{
            anchors.centerIn: parent

            spacing: 5
            Image{
                id:up_img
                source: "qrc:/images/icons/up.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:12
                Layout.preferredWidth:12
                property bool pressed
                property bool down
                property bool hovered

                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 15; height: 15

                    z: -1
                    anchor: up_img
                    pressed: up_img.pressed
                    active: up_img.down || up_img.visualFocus || up_img.hovered
                    color: up_img.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        up_img.hovered = true
                    }

                    onExited:
                    {
                        up_img.hovered = false
                    }

                    onReleased:
                    {
                        up_img.pressed = false
                        upORdown = 0
                    }

                    onPressed:
                    {
                        changeTimer.start()
                        up_img.pressed = true
                        upORdown = 1
                    }

                    onClicked:
                    {
                        amountField.text = parseInt(amountField.text) + 1
                    }
                }
            }

            Image{
                id:down_img
                source: "qrc:/images/icons/down.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:12
                Layout.preferredWidth:12
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 15; height: 15

                    z: -1
                    anchor: down_img
                    pressed: down_img.pressed
                    active: down_img.down || down_img.visualFocus || down_img.hovered
                    color: down_img.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        down_img.hovered = true
                    }

                    onExited:
                    {
                        down_img.hovered = false
                    }

                    onReleased:
                    {
                        down_img.pressed = false
                        upORdown = 0

                    }

                    onPressed:
                    {
                        changeTimer.start()
                        down_img.pressed = true
                        upORdown = -1
                    }

                    onClicked:
                    {
                        amountField.text = parseInt(amountField.text) - 1
                    }
                }

            }


        }

    }






}
