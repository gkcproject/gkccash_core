import QtQuick 2.0
 import QtQuick.Controls 2.3
 import QtQuick.Controls.Styles 1.4 as Controls_Style_1
 import QtQuick.Controls 1.4 as Controls_1


 Controls_1.TextArea
 {
     id:root
     property bool critical: false
     font.weight: Font.Light
     font.pixelSize:16
     textMargin:10
     property bool hovered: false
     wrapMode: TextEdit.Wrap
     property bool borderVisible: true
     property string backgroundColor:""
     property string backgroundFocusColor:""

     property bool linkSignalBlocked: false


     //focus:true
     onLinkActivated:
     {
         if(!linkSignalBlocked)
             Qt.openUrlExternally(link)


     }


     onLinkHovered:
     {


     }


     onTextChanged:
     {
         if(text.length>0)critical=false
     }


     MouseArea
     {
         id:mouseArea
         propagateComposedEvents:true
         anchors.fill: parent
         anchors.rightMargin: contentHeight>height?15:0
         cursorShape: containsMouse?Qt.IBeamCursor:Qt.ArrowCursor
         hoverEnabled: true
         onEntered:
         {
             root.hovered = true
         }


         onExited:
         {
             root.hovered = false
         }


         onClicked:
         {
             mouse.accepted = false
         }


         onPressed:
         {
             mouse.accepted = false
         }


         onReleased:
         {
             mouse.accepted = false
         }






     }


     style: Controls_Style_1.TextAreaStyle {
         textColor: root.backgroundColor === "" ? "#333333":"#FFFFFF"
         selectionColor: "#7BB6C3"
         selectedTextColor: "#FFFFFF"
         backgroundColor: root.backgroundColor === "" ? (root.focus?"#F0F0F0":"#FAFAFA"): (root.focus?root.backgroundFocusColor:root.backgroundColor)




         frame: Rectangle
         {
             radius:2
             color:critical?"#EE637F":(root.focus?"#3B4674":(root.hovered?"#BB3B4674":"#3B4674"))
             visible:borderVisible


         }


         scrollBarBackground:Rectangle {


             width:10
             height:10
             color:"#F0F0F0"
         }


         handleOverlap:0


         handle:Rectangle {
             implicitWidth:8
             implicitHeight:10
             color: styleData.pressed?"#787878":styleData.hovered?"#969696":"#B5B5B5"
             radius: 2
         }








         incrementControl: Rectangle {}
         decrementControl: Rectangle {}
     }




 }
