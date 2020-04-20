import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style

import "../app_items"
import "../app_dialogs"


Item
{
    id:root
    height:inputListview.y+inputListview.height

    property string typeName
    property string paramName
    property bool expanding
    property int inputCount
    property string regexp




    Label
    {
        id:typeLabel
        font.weight: Font.Light
        font.pixelSize:13
        anchors.left: parent.left
        anchors.leftMargin: 30
        width:80
        anchors.top:parent.top
        color: "#FFFFFF"
        text:typeName
        elide: Text.ElideRight
        ToolTip.text: typeName
        ToolTip.visible: typeName ? ma1.containsMouse : false
        MouseArea {
            id: ma1
            anchors.fill: parent
            hoverEnabled: true
        }
    }


    Label
    {
        id:nameLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.left: typeLabel.right
        anchors.leftMargin: 10
        width:80
        anchors.top:parent.top
        color: "#FFFFFF"
        text:paramName
        elide: Text.ElideRight
        ToolTip.text: paramName
        ToolTip.visible: paramName ? ma2.containsMouse : false
        MouseArea {
            id: ma2
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    ListView {
        id:inputListview
        clip: true
        anchors.top:parent.top
        anchors.left: nameLabel.text === ""?typeLabel.right:nameLabel.right
        anchors.leftMargin: nameLabel.text === ""?10:15
        anchors.right: parent.right
        anchors.rightMargin: 30
        height:contentHeight
        width:root.width

        boundsBehavior:Flickable.StopAtBounds
        spacing: 15
        cacheBuffer: 1000

        currentIndex: -1

        delegate:ItemDelegate {
            height:35
            width:inputListview.width
            hoverEnabled: false
            // property alias inputRec: inputLayout
            property alias paramField: paramField


            Rectangle{
                id:inputRec
                color: "#FFFFFF"
                height: 33
                width: delImg.visible?60:30
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                property alias inputLayout: inputLayout
                visible:root.expanding

                RowLayout{
                    id:inputLayout
                    anchors.centerIn: parent


                    spacing: 12
                    Image{
                        id:addImg
                        source: "qrc:/images/icons/add.png"
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
                            anchor: addImg
                            pressed: addImg.pressed
                            active: addImg.down || addImg.visualFocus || addImg.hovered
                            color: addImg.Material.rippleColor
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {
                                addImg.hovered = true
                            }

                            onExited:
                            {
                                addImg.hovered = false
                            }

                            onReleased:
                            {
                                addImg.pressed = false
                            }

                            onPressed:
                            {
                                addImg.pressed = true
                            }

                            onClicked:
                            {
                                if(inputListview.count>20)
                                {
                                    root_window.warningDialog.title = qsTr("Attention")
                                    root_window.warningDialog.content_text = qsTr("Dynamic array can only add 20 options at most")
                                    root_window.warningDialog.dim_back = false
                                    root_window.warningDialog.show()
                                    return
                                }

                                inputListview.model.append({ "title": ""})
                            }
                        }

                    }
                    Image{
                        id:delImg
                        source: "qrc:/images/icons/del.png"
                        fillMode: Image.PreserveAspectFit
                        Layout.preferredHeight:18
                        Layout.preferredWidth:18
                        property bool pressed
                        property bool down
                        property bool hovered
                        visible: inputListview.count > 1

                        Ripple {
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            width: 22; height: 22

                            z: -1
                            anchor: delImg
                            pressed: delImg.pressed
                            active: delImg.down || delImg.visualFocus || delImg.hovered
                            color: delImg.Material.rippleColor
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {
                                delImg.hovered = true
                            }

                            onExited:
                            {
                                delImg.hovered = false
                            }

                            onReleased:
                            {
                                delImg.pressed = false
                            }

                            onPressed:
                            {
                                delImg.pressed = true
                            }

                            onClicked:
                            {
                                if(inputListview.count>1)
                                    inputListview.model.remove(index)

                            }

                        }

                    }



                }



            }


            CommonTextField
            {
                id:paramField
                font.weight: Font.Light
                font.pixelSize:16
                anchors.left: root.expanding?inputRec.right:parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                visible: inputListview.count > 0 || !root.expanding
				textColor:"#000000"
                backgroundColor:"#C0C0C0"
                backgroundFocusColor:"#FFFFFF"
                // validator: regValidator
            }

            RegExpValidator
            {
                id:regValidator  //TODO: check if this changes dynamically
                //regExp: root.regexp
            }
        }

        model: ListModel {
            ListElement { title: ""; }
        }


    }

    Component.onCompleted:
    {
        if(inputCount > 1)
            for(var i = 1;i<inputCount;i++)
            {
                inputListview.model.append({ "title":""})
            }

        //        console.log("---------------------------")

        //        console.log("typeName:"+typeName)
        //        console.log("paramName:"+paramName)

        //        console.log("expanding:"+expanding)

        //        console.log("inputCount:"+inputCount)

        //        console.log("regexp:"+regexp)
        //        console.log("---------------------------")

    }



    function checkParams()
    {
        for(var i = 0 ;i<inputListview.contentItem.children.length;i++)
        {
            if(inputListview.contentItem.children[i].paramField instanceof CommonTextField &&
                    inputListview.contentItem.children[i].paramField.enabled &&
                    inputListview.contentItem.children[i].paramField.text === "")
            {
                inputListview.contentItem.children[i].paramField.critical = true
                return false
            }
        }

        return true
    }


    function getParams()
    {
        var array = new Array
        for(var i = 0 ;i<inputListview.contentItem.children.length;i++)
        {
            if(inputListview.contentItem.children[i].paramField instanceof CommonTextField &&
                    inputListview.contentItem.children[i].paramField.enabled &&
                    inputListview.contentItem.children[i].paramField.text !== "")
            {
                array.push(inputListview.contentItem.children[i].paramField.text)
            }
        }

        return array
    }


}
