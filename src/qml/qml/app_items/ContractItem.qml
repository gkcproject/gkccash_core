import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

import "../app_pages"

Item{
    id:root
    height:paramList_.y+paramList_.height
    property CommonTextArea abiCodeText

    property alias paramList: paramList_

    property int pageType
    property int create: 0
    property int sendTo: 1
    property int call: 2


    signal  functionChanged(int index)

    Label
    {
        id:functionLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.verticalCenter: comboBox.verticalCenter

        color: "#FFFFFF"
        text:qsTr("Function")
        visible:comboBox.visible
    }

    Connections
    {
        target: abiCodeText
        onTextChanged:
        {
            if(comboBox.visible && comboBox.listview.contentItem.children[0] !== undefined)
            {
                comboBox.text = comboBox.listview.contentItem.children[0].item_label.text
                comboBox.listview.currentIndex = 0
                comboBox.currentIndexChanged(0)
            }

            if(pageType === root.create && comboBox.listview.contentItem.children[0] !== undefined)
            {
                renewParams(0)


            }

            if(pageType === root.create)
            {

                if(abiCodeText.critical || abiCodeText.text.length === 0)
                    paramList_.visible = false
                else
                    paramList_.visible = true
            }




        }


    }


    CommonComboBox
    {
        id:comboBox
        model:  pageType === root.create?walletModel.contractPage.createFunctionList:(pageType === root.sendTo?walletModel.contractPage.sendtoFunctionList:walletModel.contractPage.callFunctionList)
        visible:listview.count>0 && pageType !== root.create
        anchors.left: parent.left
        anchors.leftMargin: 120
        anchors.top:parent.top
        anchors.topMargin: 20
        width:300
        color:"#D0D0D0"

        onVisibleChanged:
        {
            if(!visible)
                paramList_.visible = false
            else
                paramList_.visible = true
        }

        onCurrentIndexChanged:
        {
            if(comboBox.listview.currentIndex == -1)
                return

           renewParams(index)

//            console.log("paramCount:"+paramCount)
//            console.log("nameArray:"+nameArray)
//            console.log("typeArray:"+typeArray)
//            console.log("expandingArray:"+expandingArray)
//            console.log("inputCountArray:"+inputCountArray)
//            console.log("regexpArray:"+regexpArray)

        }
    }


    ListView {
        id:paramList_
        anchors.top: comboBox.visible?comboBox.bottom:parent.top
        anchors.topMargin: 10
        anchors.left:parent.left
        anchors.right: parent.right
        height:contentHeight
        cacheBuffer: 5000 //TODO:this is a bad limit because of listview buffering system, but common use will be fine.

        spacing: 15

        model:paramsModel
        delegate:ContractParam
        {
            width:paramList_.width
            paramName:name_
            typeName:type_
            expanding:expanding_
            inputCount:inputCount_
            regexp:regexp_


        }

        Component.onCompleted:
        {
            if(pageType === root.create)
                renewParams(0)
        }

    }

    ListModel
    {
        id:paramsModel

    }

    function renewParams(index)
    {
        var res = walletModel.contractPage.functionChanged(pageType,index)

        var nameArray = new Array
        var typeArray = new Array
        var expandingArray = new Array
        var inputCountArray = new Array
        var regexpArray = new Array


        var paramCount = res.length/5

        paramsModel.clear()
       // console.log("paramCount:"+paramCount)

        for(var i = 0;i < paramCount;i++)
        {
            nameArray.push(res[i])
            typeArray.push(res[paramCount + i])
            expandingArray.push(res[paramCount*2 + i])
            inputCountArray.push(res[paramCount*3 + i])
            regexpArray.push(res[paramCount*4 + i])

            paramsModel.append({ "name_": res[i],
                                   "type_":res[paramCount + i],
                                   "expanding_":res[paramCount*2 + i],
                                   "inputCount_":res[paramCount*3 + i],
                                   "regexp_":res[paramCount*4 + i]
                               })





        }


        if(pageType == root.sendTo)
            root.functionChanged(index)

    }

    function getCurrentFunction()
    {
        return comboBox.listview.currentIndex
    }

    function checkParams()
    {

        for(var i = 0 ;i<paramList_.contentItem.children.length;i++)
        {
            if(paramList_.contentItem.children[i] instanceof ContractParam && !paramList_.contentItem.children[i].checkParams())
                    return false
        }

        return true
    }


    function getParams()
    {
        var array = new Array
        for(var i = 0 ;i<paramList_.contentItem.children.length;i++)
        {
            if(paramList_.contentItem.children[i] instanceof ContractParam)
                array.push(paramList_.contentItem.children[i].getParams())
        }

        return array
    }


}



