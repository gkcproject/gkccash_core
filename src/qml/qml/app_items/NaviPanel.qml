import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3

Column{

    NaviItem
    {
        id:index
        icon:(picked||hovered)?"../../images/navi/shouyePicked.png":"../../images/navi/shouye.png"
        title: qsTr("Index")
        picked: true
        index_:0

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
    }

    NaviItem
    {
        id:send
        icon:(picked||hovered)?"../../images/navi/fasongPicked.png":"../../images/navi/fasong.png"
        title: qsTr("Sending")
        picked: false
        index_:1

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
    }

    NaviItem
    {
        id:receive
        icon:(picked||hovered)?"../../images/navi/jieshouPicked.png":"../../images/navi/jieshou.png"
        title: qsTr("Receiving")
        picked: false
        index_:2

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
    }

    NaviItem
    {
        id:record
        icon:(picked||hovered)?"../../images/navi/jilvPicked.png":"../../images/navi/jilv.png"
        title: qsTr("Txes Records")
        picked: false
        index_:3

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
    }

    NaviItem
    {
        id:privacy
        icon:(picked||hovered)?"../../images/navi/privacyPicked.png":"../../images/navi/privacy.png"
        title: qsTr("Privacy")
        picked: false
        index_:4
        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
    }

    NaviItem
    {
        id:mainnode
        icon:(picked||hovered)?"../../images/navi/jiedianPicked.png":"../../images/navi/jiedian.png"
        title: qsTr("Masternodes")
        picked: false
        index_:5

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
    }

    NaviItem
    {
        id:contractNode
        icon:(picked||hovered)?"../../images/navi/heyuePicked.png":"../../images/navi/heyue.png"
        title: qsTr("Smart Contract")
        picked: false
        index_:6

        SecondaryNaviPanel
        {
            id:secondaryPanel
            y: contractNode.height/2 - height/2 - 10
            x:170

            model: ListModel
            {
                ListElement {name:qsTr( "Smart Contract" ) }
                ListElement {name:qsTr( "GRC Token" )  }
            }

            onPicked_:
            {
                reset_all_items()
                contractNode.picked = true
            }


        }
		
        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.openMenu()
        }
    }
	
	NaviItem
	{
		id:entrust
        icon:(picked||hovered)?"../../images/navi/entrustPicked.png":"../../images/navi/entrust.png"
        title: qsTr("Equity Entrust")
        picked: false
        index_:7

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
	}

	NaviItem
	{
		id:notification
        icon:(picked||hovered)?"../../images/navi/newsPicked.png":"../../images/navi/news.png"
        title: qsTr("Notification")
        picked: false
        index_:8

        onHoveredChanged:
        {
            if(hovered)
                secondaryPanel.closeMenu()
        }

        onClicked:
        {
            secondaryPanel.clear_others()
        }
	}

    function reset_all_items()
    {
        index.picked = false
        send.picked = false
        receive.picked = false
        record.picked = false
        privacy.picked = false
        mainnode.picked = false
        contractNode.picked = false
		entrust.picked = false
		notification.picked = false
    }

	function gotoMainNodePage()
	{
		reset_all_items()
		mainnode.picked = true
		tab_change(5)
	}
	
    function gotoTxpage()
    {
        reset_all_items()
        record.picked = true
        tab_change(3)
    }

    function gotoReceivepage()
    {
        reset_all_items()
        receive.picked = true
        tab_change(2)
    }

	function gotoEntrustpage()
	{
		reset_all_items()
        entrust.picked = true
        tab_change(8)
	}

	function gotoNotificationPage()
	{
		reset_all_items()
        ntification.picked = true
        tab_change(9)
	}

    Connections
    {
        target:walletModel

        onTraySignal:
        {
            if(index === 1)
            {
                reset_all_items()
                send.picked = true
                tab_change(1)
            }
            else if(index === 2)
            {
                gotoReceivepage()
            }
            else if(index === 3)
            {
                reset_all_items()
                privacy.picked = true
                tab_change(4)
            }
			else if(index === 5)
			{
				gotoMainNodePage()
			}
			else if(index === 7)
			{
				gotoEntrustpage()
			}
			else if(index == 8)
			{
				gotoNotificationPage()
			}
        }
    }


}
