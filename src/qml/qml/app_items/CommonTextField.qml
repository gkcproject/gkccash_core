import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4 as Controls_Style_1
import QtQuick.Controls 1.4 as Controls_1
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
Controls_1.TextField{
    id:root

    width: 400
    height:33
    clip: true
    textColor: "#FFFFFF"
    selectByMouse: true

    property bool critical: false

    property bool is_default_parsing: false

    property alias hintmenu: hintmenu
    property string tableName
    property string key
    property var backgroundColor:undefined
    property var backgroundFocusColor:undefined
    property alias hintmodel:hintlistview.model


    property string candidates
    property bool showDropImg: false

    signal down_pressed()
    signal up_pressed()

//    onActiveFocusChanged:
//    {
//        console.log("activeFocus:",activeFocus)
//    }

    Image
    {
        id:dropImg
        visible: showDropImg
        source: "qrc:/images/icons/down.png"
        width:18
        height:18
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin:10

        fillMode: Image.PreserveAspectFit
        property bool pressed
        property bool down
        property bool hovered
        Ripple {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 20; height: 20

            z: -1
            anchor: dropImg
            pressed: dropImg.pressed
            active: dropImg.down || dropImg.visualFocus || dropImg.hovered
            color: dropImg.Material.rippleColor
        }

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
            onEntered:
            {
                dropImg.hovered = true
            }

            onExited:
            {
                dropImg.hovered = false
            }

            onReleased:
            {
                dropImg.pressed = false
            }

            onPressed:
            {
                dropImg.pressed = true
            }

            onClicked:
            {
               if(!hintmenu.opened)
                   hintmenu.open()
            }
        }




    }



    Keys.enabled: true

    Keys.onPressed: {
        switch(event.key)
        {
        case Qt.Key_Down:
            event.accepted=true

            if(!hintmenu.opened)
            {
                down_pressed()
                return
            }

            if(!hintmenu.focus && root.focus){
                hintmenu.focus=true
                //root.focus=false
                if(hintlistview.count>1)
                    hintlistview.currentIndex=1
            }
            else{
                if(hintlistview.currentIndex<hintlistview.count-1)
                    hintlistview.currentIndex=hintlistview.currentIndex+1
            }
            break;
        case Qt.Key_Up:
            event.accepted=true

            if(!hintmenu.opened)
            {
                up_pressed()
                return
            }

            if(!hintmenu.focus && root.focus){
                return
            }
            else{
                if(hintlistview.currentIndex>0)
                    hintlistview.currentIndex=hintlistview.currentIndex-1
            }
            break;
        case Qt.Key_Return:
            if(!hintmenu.opened){
                return
            }
            else{

                if(hintlistview.currentItem != null)
                    candidates = hintlistview.currentItem.myData.modelData
                else
                    candidates = ""
            }
            break;
        }
    }

    Popup {
        id:hintmenu
        y:root.height
        padding: 0
        width:root.width
       // height:hintlistview.contentHeight>200?220:hintlistview.contentHeight+20
        contentItem: ListView {
            id:hintlistview
            width:root.width
            clip: true
            implicitHeight: contentHeight>120?120:contentHeight
            implicitWidth: root.width

            boundsBehavior:Flickable.StopAtBounds

            ScrollBar.vertical: MyScrollBar {
                id: scrollBar
                width:6
                height:30

                anchors.right: hintlistview.right
                handle.implicitHeight: 80

                policy:ScrollBar.AsNeeded
                visible: hintlistview.count * 22 > hintlistview.height
            }


            delegate:MenuItem {
                width: root.width
                height:22
                property variant myData: model
                Label
                {
                    id:item_label
                    anchors.fill: parent
                    text: modelData
                    font.weight: Font.Medium
                    font.pixelSize:11
                    font.letterSpacing: 0.355
                    verticalAlignment: Label.AlignVCenter
                    horizontalAlignment: Label.AlignLeft
                    leftPadding: 10

                }

                background: Rectangle
                {
                    color:hintlistview.currentIndex == index?"#8B469AAC":(hovered?"#D3D3D3":"#FFFFFF")
                }


                MouseArea{
                    anchors.fill:parent
                    hoverEnabled: true
                    onClicked: {
                        root.text = item_label.text
                         hintmenu.close()
                    }





                }

            }



        }
    }



    style: Controls_Style_1.TextFieldStyle{
        background: Rectangle{
            color: (backgroundColor == undefined)?(root.focus?"#3B4674":"#BB3B4674"):(root.focus?backgroundFocusColor:backgroundColor)

            border.color: critical?"#EE637F":(root.focus?"#4B4674":(root.hovered?"#7B3B4674":"#3B4674"))
            border.width: critical?2:1
            radius:2

        }

        placeholderTextColor:"#6B78A8"
        selectionColor: "#7BB6C3"
        selectedTextColor: "#FFFFFF"
        padding.left: 10


    }

    onFocusChanged: {
       // if(focus) root.selectAll();
    }



    onTextChanged: {

        if(is_default_parsing){
            is_default_parsing=false
            return
        }


        if(root.text==""){
            hintmenu.close()
            return
        }

//        rpcConsole.findhint(tableName,key,root.text)

//        if(rpcConsole.hintRowCount()>0){
//            hintlistview.model = rpcConsole.hintModel
//            hintmenu.open()
//            hintmenu.focus=false
//            root.focus=true
//        }
//        else{
//            hintmenu.close()
//        }



        if(text.length>0)critical=false

    }

}
