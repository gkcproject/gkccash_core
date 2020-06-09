import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as  Controls_Style_1_4
import QtGraphicalEffects 1.0


Controls_1_4.TableView {
    id:tableview


    selectionMode:Controls_1_4.SelectionMode.ExtendedSelection

    property bool add_index_column: false
    property bool dontPerformSortInner:false
    property int  index_column_width: 30
    property var roles
    property var widths
    property var titles
    property alias menuModel: popuper.model
    property alias popuperWidth: popuper.width
	property alias mouseArea:mouseArea

    frameVisible:false
    alternatingRowColors: false

    signal menuPicked(int tableRow,int menuIndex)



    onSortIndicatorColumnChanged: {
        if(dontPerformSortInner)
            return
        //model.sortColumn(getColumn(sortIndicatorColumn).role,sortIndicatorOrder)
    }
    onSortIndicatorOrderChanged: {
        if(dontPerformSortInner)
            return
        //model.sortColumn(getColumn(sortIndicatorColumn).role,sortIndicatorOrder)
    }


    Component.onCompleted: {

        if(add_index_column)
            tableview.addColumn(prepare_column_elements("index", "order number", index_column_width))

        tableview.add_column()

        //console.log("roles.length:",roles.length)
    }

    CommonPopup
    {
        id:popuper

        width: 100
        itemHeight: 30

        onMenuPicked:
        {
            tableview.menuPicked(tableview.currentRow,index)
        }

    }


    MouseArea
    {
        //TODO:Make this into each  row and hoverable, so we can change each row back color, when hovered!!


        id:mouseArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top:parent.top
        anchors.topMargin:32

        acceptedButtons: Qt.RightButton

        onClicked:
        {
            if(popuper.model === undefined)
                return

           tableview.currentRow = rowAt(mouseX,mouseY+32)
           tableview.selection.clear()
           tableview.selection.select(tableview.currentRow)

            popuper.x = mouseX
            popuper.y = mouseY+32


            if(!popuper.opened )
            {
                if(popuper.model.count>0)
                    popuper.open()
            }
            else
                popuper.close()
        }
    }




    headerDelegate: Rectangle{
        id: headerRect
        height: 32
        radius: 3
        color:"#3D4975"

        // visible:styleData.column !== -1





        Rectangle{
            height: 32
            color:"#3D4975"
            width:10
            anchors.left: parent.left
            visible: styleData.column !== 0
            anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle{
            height: 32
            color:"#3D4975"
            width:10
            anchors.right: parent.right
            visible: styleData.column !== -1
            anchors.verticalCenter: parent.verticalCenter
        }

        Text{
            id:title_text
            text: styleData.value
            anchors.centerIn: parent
            elide:Text.ElideRight
            //width:parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
            font.letterSpacing: 0.5
            color: "#FFFFFF";
            font.weight: Font.Medium


        }

        /*Image{
            id:sort_img
            source:sortIndicatorOrder==Qt.DescendingOrder?"qrc:/images/icons/sort_descending.png":"qrc:/images/icons/sort_ascending.png"
            visible:styleData.column === sortIndicatorColumn?true:false
            fillMode: Image.PreserveAspectFit
            anchors.left:title_text.right
            anchors.leftMargin: 2
            anchors.verticalCenter: title_text.verticalCenter
            width:10
            height:10

        }*/

    }


    rowDelegate: Rectangle{

        color: styleData.selected? "#273157":(styleData.alternate ? "#212b50" :"#212b50")
        height:32
        //radius: 3

        Rectangle
        {
            anchors.bottom: parent.bottom
            height: 1
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#2A3358"
            visible: styleData.row !== (rowCount-1)
        }


    }

    itemDelegate:Text{
        id:cell_item
        text: styleData.value !== undefined? styleData.value:""

        color: styleData.selected? "#F0F0F0" : "#FFFFFF"
        elide:styleData.elideMode
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        font.letterSpacing: 0.5
        font.weight: Font.Light
    }

    style:Controls_Style_1_4.TableViewStyle{
        id:tableviewStyle

        //minimumHandleLength:20

        corner:Rectangle {
            width:15
            height:15

            color:"#3D4975"
        }

        scrollBarBackground:Rectangle {

            width:10
            height:10
           // color: styleData.hovered?"red":"gray"
            color:"#F1F1F1"
        }
       // transientScrollBars:true
        handleOverlap:0

        handle:Rectangle {
            implicitWidth:8
            implicitHeight:10
            color: styleData.pressed?"#4D4975":styleData.hovered?"#9B3D4975":"#3B3D4975"
            radius: 2
        }

        backgroundColor: "#212b50"



        incrementControl: Rectangle {}
        decrementControl: Rectangle {}




    }



    // onDoubleClicked:set_editor()


    function prepare_column_elements(role,title,width){

        var columnString = 'import QtQuick 2.10; import QtQuick.Controls 1.4; TableViewColumn {role: "' + role

        if(role === "index")
        {
            columnString += '";movable:false; title: "' + title + '"; width: '+ width + ';horizontalAlignment:Text.AlignHCenter}'
        }
        else
        {
            columnString += '"; title: "' + title + '"; width: '+ width + ';horizontalAlignment:Text.AlignHCenter}'
        }
        var column = Qt.createQmlObject(
                    columnString
                    , tableview
                    , "dynamicSnippet1")
        return column

    }

    function add_column(){

        for(var i=0;i<roles.length;i++){

            tableview.addColumn(prepare_column_elements(roles[i], titles[i], widths.length===0?100:widths[i]))
        }
    }

}
