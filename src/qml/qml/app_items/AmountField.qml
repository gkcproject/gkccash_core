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
    property alias coinTypeBtn: coinTypeBtn
    property int currentUnit: 0

    property int upORdown: 0
    property alias valueAmount:amountField.text


    property int factor: walletModel.getFeePerkilo()

    Component.onCompleted:
    {
        root_window.unitNameChanged.connect(coinTypeChange)
    }

    function getFieldAmount()
    {
        return walletModel.getFieldAmount(coinTypeBtn.index,amountField.text)
    }

    Timer{
        id:changeTimer
        triggeredOnStart: false
        repeat: true
        interval: 100
        onTriggered: {
            if(upORdown != 0)
            {
                getAmount(upORdown)
                return
            }
            changeTimer.stop()
        }
    }


    function coinTypeChange()
    {
        if(root_window.unitName === "  GKC"){
            coinTypeBtn.text = "GKC"
            coinTypeBtn.index = 0
        }
        else if(root_window.unitName === "  mGKC"){
            coinTypeBtn.text = "mGKC"
            coinTypeBtn.index = 1
        }
        else{
            coinTypeBtn.index = 2
            coinTypeBtn.text = "μGKC"
        }

    }



    Component.onDestruction:
    {
        root_window.unitNameChanged.disconnect(coinTypeChange)
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
            bottom: 0; top: 1000000000;
            notation:DoubleValidator.StandardNotation
            decimals:coinTypeBtn.index == 0?8:coinTypeBtn.index == 1?5:2
        }

    }

    Rectangle
    {
        id:spinRec
        color: amountField.focus?"#3B4674":"#BB3B4674"
        height: amountField.height
        width: 15
        anchors.right: coinTypeBtn.left
        anchors.rightMargin: 10
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
                        getAmount(1)
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
                        getAmount(-1)
                    }
                }

            }


        }

    }


    CoinTypeCoin
    {
        id:coinTypeBtn
        anchors.verticalCenter: root.verticalCenter
        anchors.right: root.right
        width: 70
        height: 32
        color: "#FDA205"


        onIndexChanged:
        {
            getAmount(0)
            root.currentUnit = index
        }
    }

    function getAmount(direction)
    {
        var unit = coinTypeBtn.index
        amountField.text = walletModel.getAmount(root.currentUnit,unit,amountField.text,direction,factor)
    }



}
