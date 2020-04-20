import QtQuick 2.0
import QtQuick.Controls 2.3

Popup {
    id:root

    width:listview.contentWidth
    height:70
    property alias model: listview.model
    property int width_: 160
    signal picked_()


    function openMenu()
    {
        if(animateWidth_out.running)
            animateWidth_out.stop()

        animateWidth_in.start()

        root.open()
    }

    function closeMenu()
    {
        if(animateWidth_in.running)
            animateWidth_in.stop()

        animateWidth_out.start()

        root.close()
    }

    onAboutToHide:
    {
        closeMenu()
    }

    PropertyAnimation {id: animateWidth_in; targets: root; properties: "height"; to: 70; duration: 200}
    PropertyAnimation {id: animateWidth_out; targets: root; properties: "height"; to: 70; duration: 200}


    function clear_others()
    {
        for(var i = 0;i<listview.count;i++)
        {
            listview.contentItem.children[i].picked = false
        }

    }

    contentItem: ListView {
        id:listview
        clip: false
        height:70
        width: width_
        boundsBehavior:Flickable.StopAtBounds
        //orientation: ListView.Horizontal




        currentIndex: 0

        delegate:MenuItem {
            height:35
            width:width_
            property bool picked: false

            Label
            {
                id:itemLabel
                //anchors.fill: parent
                width:width_
                height: 35


                text: name
                font.weight: Font.Medium
                font.pixelSize:13
                font.letterSpacing: 0.355
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                color: "#FAFAFA"


            }

            background: Rectangle
            {
                color:hovered?"#37416D":"#333D66"
            }


            MouseArea
            {
                anchors.fill:parent
                hoverEnabled: true
                onClicked: {

                    if(index === 0)
                        tab_change(6)
                    else if(index === 1)
                        tab_change(7)
                    else if(index === 2)
                        tab_change(8)

                    clear_others()
                    picked = true
                    picked_()
                    root.closeMenu()

                }



            }

        }
    }
}
