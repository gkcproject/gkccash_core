import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0
import "qml/app_pages"
import "qml/app_items"
import "qml/base_items"
import "qml/app_dialogs"

import "qml"

ApplicationWindow {
    id: root_window
    visible: true
    x:300
    y:150
    width: Screen.width-600
    height: Screen.height-300

    title: qsTr("Gkc Core")
    color: "#F1F3F4"

    opacity: 0
    Material.elevation: 8
    property bool is_Maximized: false
    property alias root_window_dim_switch: root_window_dim.visible
    property int radius_all
    property string unitName:"  GKC"
    property alias warningDialog: warningDialog
    property alias footer_: footer
    property alias  title_item: title_item

    PropertyAnimation
    {
        id: show_window; target: root_window; properties: "opacity"; to: 1; duration: 300

        onStopped:
        {
            if(!walletModel.alreadyShowed("2.0.0"))
                msgDialog.show()
        }
    }


    StartUpDialog
    {
        modality: Qt.ApplicationModal
        id:msgDialog
        dim_back: true
    }



    onVisibilityChanged:
    {
        if(visibility === Window.Windowed)
        {
            title_item.maximum_btn.source = "images/icons/mainwindow_maximum.png"
            is_Maximized = false
        }
        else if(visibility === Window.Maximized)
        {
            title_item.maximum_btn.source = "images/icons/mainwindow_restore.png"
            is_Maximized = true
        }
    }

    Connections
    {
        target: walletModel

        onHideWindow:
        {
            shutdown_dialog.show()
            root_window.opacity=0
        }

    }

    function tab_change(index)
    {
        body.currentIndex = index
        if(index==0)
        {
        	indexPage.item.init()
        }
		if(index==5)
		{
			mainnodePage.item.init()
		}
		else if(index==8)
		{
			entrustPage.item.init()
		}
		else if(index==9)
		{
			notificationPage.item.init()
		}
    }

    function gotoTransactionPage(index)
    {
        naviPanel.gotoTxpage()

        txPage.item.historyTable.currentRow = walletModel.transaction_ProxyModel.getRow(walletModel.transaction_ProxyModel.mapFromSource(index))
        txPage.item.historyTable.selection.clear()
        txPage.item.historyTable.selection.select(txPage.item.historyTable.currentRow)
    }

	function gotoTxpageReward()
    {
        naviPanel.gotoTxpage()
        txPage.item.gotoEntrustReward()
    }

    function gotoReceivepage()
    {
        naviPanel.gotoReceivepage()
    }
	
	function gotoEntrustpage()
	{		
		entrustPage.item.init()
		naviPanel.gotoEntrustpage()
	}

	function gotoNotificationPage()
	{		
		notificationPage.item.init()
		naviPanel.gotoNotificationPage()
	}

    onClosing:
    {
        exit_dialog.show()
    }

    Timer{
        id:pretend_timer
        triggeredOnStart: false
        interval: 100
        onTriggered: {
            show_window.start()
            root_window.x = 300
            root_window.y = 150
        }

    }
	
    Component.onCompleted:
    {
        pretend_timer.start()
    }

    Rectangle{
        id:background
        anchors.fill: parent
        color: "transparent"
        radius: radius_all == undefined? 2:radius_all
        objectName: "background"
        anchors.margins: is_Maximized?8:0

        Rectangle
        {
            id:navi_background
            x:0
            y:65
            height:parent.height-65
            width: 180
            color:walletModel.isTestNet()?"#3F6D2A":"#2A3358"
        }


//            Image{
//                id:logo_img
//                source: "qrc:/images/icons/logo.png"
//                fillMode: Image.PreserveAspectFit
//                anchors.horizontalCenter: parent.horizontalCenter
//                anchors.top: parent.top
//                anchors.topMargin: 30
//                width:120
//            }

//            Label
//            {
//                id:testnetLabel
//                anchors.top: logo_img.bottom
//                anchors.topMargin:20
//                anchors.horizontalCenter: parent.horizontalCenter

//                font.pixelSize:22
//                text:qsTr("TestNet")
//                color:"#F0F0F0"
//                font.weight:Font.bonusBtnBold
//                visible:walletModel.isTestNet()
//            }

//            Image{
//                id:leftCorner_img
//                source: "qrc:/images/icons/leftCornerImg.png"
//                fillMode: Image.PreserveAspectFit
//                anchors.bottom: parent.bottom
//                width:162
//            }
//        }

        TitleItem
        {
            id:title_item
            x:0
            y:0
            height:70
            width:parent.width
        }

        NaviPanel
        {
            id:naviPanel
            anchors.left: parent.left
            anchors.top:navi_background.top
            anchors.topMargin: 30
            width:180
        }

        Controls_1_4.TabView
        {
            id:body
            z:-1
            anchors.top: title_item.bottom
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 45
            anchors.left: parent.left
            anchors.leftMargin: 180
            anchors.right: parent.right
            tabsVisible: false
            frameVisible: false



            Connections
            {
                target: bitcoinapp

                onNumBlocksChanged:
                {
                    set_progress_message(title,msg,(progress/progress_max).toFixed(2),tooltip);
                }


            }

            IndexPage{id:indexPage}
            SendingPage {id:sendingPage}
            ReceivingPage {}
            TransactionRecordPage {id:txPage}
            PrivacyPage {}
            MainNodePage {id:mainnodePage}
            SmartContractPage{}
            TokenPage{}
			EntrustPage{id:entrustPage}
			NotificationPage{id:notificationPage}

            currentIndex: 0
        }




        Footer
        {
            id:footer
            radius:radius_all
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: body.left
            height:65
            z:-2
            color:"#212b50"
        }


        //  }

    }


    ColorOverlay
    {
        id:root_window_dim
        anchors.fill: background
        source: background
        color: "#7B303030"
        visible: false
    }




    function changeCoincontrol(show)
    {
        sendingPage.item.showCoincontrol = show
        walletModel.coinControlProxy.setEnabled(show)
    }

    function set_progress_message(title_str,context_str,value_double,tip_str)
    {
        footer.progressbar_title = title_str
        footer.progressbar_context = context_str
        footer.progressbar_value = value_double
        footer.progressbar_tip_str = tip_str
    }

    CommonDialog
    {
        id:shutdown_dialog
        title: "Gkc Core"
        confrim_btn_visible: false
        cancel_btn_visible: false
        content_text: qsTr("Gkc Core is shutting down...") + "<br /><br />" +
                      qsTr("Do not shut down the computer until this window disappears.");
        width:480
        height: 250
        close_btnRect_visible:false

    }

    CommonDialog
    {
        id:exit_dialog
        title: qsTr("Quit Gkc")
        confirm_btn_text: qsTr("Ok")
        cancel_btn_text: qsTr("Cancel")
        content_text: qsTr("Confirm quitting Gkc Core")
        modality: Qt.ApplicationModal

        width:300
        height: 180
        dim_back:true
        onConfirmed:
        {
            exit_dialog.close()
            Qt.quit()
        }

    }

    CommonDialog
    {
        id:warningDialog
        title: qsTr("Attention")
        confirm_btn_text: qsTr("Ok")
        cancel_btn_visible: false
        modality: Qt.ApplicationModal
        width:400
        height: 220
        dim_back:true
        onConfirmed:
        {
            warningDialog.close()
        }

    }

    CommonDialog
      {
          id:questionDialog
          width:300
          height: 180
          confirm_btn_text: qsTr("Ok")
          cancel_btn_text: qsTr("Cancel")
          cancel_btn_visible: false
          modality: Qt.ApplicationModal
          property var callBackFunc



          onCanceled:
          {
              if(callBackFunc !== undefined)
                  callBackFunc = undefined
          }

          onClosed:
          {
              if(callBackFunc !== undefined)
                  callBackFunc = undefined
          }

          onConfirmed:
          {
              questionDialog.close()
              if(callBackFunc !== undefined)
              {
                  callBackFunc()
                  callBackFunc = undefined
              }

          }
      }


      function question(title,msg,func)
      {
          questionDialog.title = title
          questionDialog.content_text = msg
          questionDialog.show()
          if(func !== undefined)
              questionDialog.callBackFunc = func

      }

      function requestUnlock()
      {
        return title_item.requestUnlock()
      }




    //    Button
    //    {
    //        id:root_btn
    //        anchors.centerIn: parent


    //        onClicked:
    //        {
    //            root_log.show()
    //        }

    //    }


    //    MenuDialogBitcoinApplication
    //    {

    //        id:root_log
    //        x:root_btn.x

    //        y:root_btn.y+root_btn.height

    //        model:ListModel
    //        {
    //            ListElement {
    //                modelData: "Apple"
    //                cost: 2.45
    //            }
    //            ListElement {
    //                modelData: "PeerxcvfsxdsfsdfsdfsdfsdfsdBitcoinApplicationfsdfsdf"
    //                cost: 2.45
    //            }
    //            ListElement {
    //                modelData: "Fuck"
    //                cost: 2.45
    //            }

    //        }
    //    }

}


