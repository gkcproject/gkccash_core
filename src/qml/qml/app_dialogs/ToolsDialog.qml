import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import TrafficGraphWidget 1.0
import "../app_pages"
import "../app_items"
import "../base_items"


CommonDialog
{
    id:root
    title: "Tools Window"
    //background_rec.anchors.bottomMargin: 150

    confrim_btn_visible:false
    cancel_btn_visible:false
    property alias current_index:tabview.currentIndex
    // property string text
    width:800
    height:800
    // property alias message_field:messageField
    //    onCurrent_indexChanged:
    //    {
    //        root.setWidth(tabview.currentIndex==1?800:642)
    //        root.setHeight(tabview.currentIndex==1?550:tabview.currentIndex==2?500:395)
    //    }

    property bool peerTimerStarted: true




    onVisibleChanged:
    {
        if(!visible)
        {
            if(!peerTimerStarted)
                return
            rpcConsole.peerTableModel_sorted.stopAutoRefresh()
            peerTimerStarted = false
        }

    }

    Item{
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 25

        CommonTabView{
            id:tabview
            tab_width:100
            currentIndex: -1


            Controls_1_4.Tab {
                title: qsTr("Information")


                Rectangle{
                    id:tab1_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:info_label
                        font.weight: Font.Light
                        font.pixelSize:14
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        anchors.bottomMargin: 100
                        lineHeight: 1.03
                        width:200
                        wrapMode:Text.WrapAnywhere
                        font.letterSpacing:0.3
                        text:qsTr("<b>General</b><br>Client name<br>Client version<br>&nbsp;&nbsp; \
                             Using OpenSSL version<br>&nbsp;&nbsp;Using BerkeleyDB version<br>Build date<br>Startup time<br> \
                             <b>Network</b><br>Name<br>Number of connections<br>Number of Masternodes<br> \
                             <b>Block chain</b><br>Current number of blocks<br>Last block time")
                    }

                    TextEdit
                    {
                        id:info_textArea
                        font.weight: Font.Light
                        font.pixelSize:14
                        font.letterSpacing:0.3
                        anchors.left: parent.left
                        textFormat: Qt.RichText
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 100
                        anchors.topMargin: 6
                        anchors.leftMargin: 250
                        width:350
                        wrapMode:Text.WrapAnywhere
                        readOnly: true
                        selectByMouse: true

                    }

                    Label
                    {
                        id:openDebugLog_label
                        font.weight: Font.Bold
                        font.pixelSize:14
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        anchors.bottomMargin: 35
                        lineHeight: 1.035
                        width:200
                        height:30
                        wrapMode:Text.WrapAnywhere
                        font.letterSpacing:0.3
                        text:qsTr("<b>Debug log file</b>")
                    }

                    CommonButton
                    {
                        id:openDebugLog_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        text:qsTr("Open")
                        width: 100
                        height: 28
                        textSize:11
                        onClicked:
                        {
                            rpcConsole.openDebugLogFile()
                        }
                    }



                    Component.onCompleted:
                    {
                        info_textArea.text = rpcConsole.getStartUpInfo()
                    }

                    Connections
                    {
                        target: rpcConsole

                        onInfoUpdate:
                        {
                            info_textArea.text = info
                        }
                    }

                }

            }

            Controls_1_4.Tab {
                title: qsTr("Console")


                Rectangle{
                    id:tab2_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    CommonTextArea
                    {
                        id:messageArea
                        font.weight: Font.Light
                        font.pixelSize:13
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        anchors.bottomMargin: 50
                        textFormat: Qt.RichText
                        readOnly: true

                        wrapMode: TextEdit.Wrap
                        Component.onCompleted:
                        {
                            rpcConsole.startFromQML()
                        }

                        Connections
                        {
                            target: rpcConsole

                            onNewMessage:
                            {
                                messageArea.text += message

                                if(messageArea.flickableItem.contentHeight>height)
                                    messageArea.flickableItem.contentY = messageArea.flickableItem.contentHeight - height + 55 //srcoll to the end of textarea
                            }
                        }


                    }

                    CommonTextField
                    {
                        id:terminal
                        font.weight: Font.Light
                        font.pixelSize:16
                        anchors.left: messageArea.left
                        anchors.top: messageArea.bottom
                        anchors.topMargin: 2
                        anchors.right: messageArea.right
                        anchors.rightMargin: 34

                        anchors.bottom:parent.bottom
                        anchors.bottomMargin: 10
                       	//backgroundColor:"#F0F0F0"
                        //backgroundFocusColor:"#FFFFFF"
                        property int history_offset:0
                        property bool textTriggeredbyUpandDown:false


                        onDown_pressed:
                        {
                            textTriggeredbyUpandDown = true
                            text = rpcConsole.browseHistory(1)
                            hintmenu.close()
                        }

                        onUp_pressed:
                        {
                            textTriggeredbyUpandDown = true
                            text = rpcConsole.browseHistory(-1)
                            hintmenu.close()
                        }


                        onTextChanged:
                        {
                            if(textTriggeredbyUpandDown == true)
                            {
                                textTriggeredbyUpandDown = false
                                return
                            }
                            if(text=="")
                            {
                                hintmenu.close()
                                return
                            }

                            rpcConsole.findhint(text)

                            if(rpcConsole.hintRowCount()>0)
                            {
                                hintmodel = rpcConsole.hintModel
                                hintmenu.open()
                                hintmenu.focus=false
                                focus=true
                            }
                            else
                            {
                                hintmenu.close()
                            }

                        }

                        onAccepted:
                        {
                            history_offset = 0

                            if(candidates != "")
                            {
                                text = candidates
                                hintmenu.close()
                                candidates=""
                                return
                            }


                            if(text=="")
                                return

                            rpcConsole.on_lineEdit_returnPressed(text)

                            text="";

                        }
                    }


                    Image{
                        id:delete_img
                        source: "qrc:/images/icons/delete.png"
                        fillMode: Image.PreserveAspectFit
                        anchors.right: messageArea.right
                        anchors.rightMargin: 2
                        anchors.verticalCenter: terminal.verticalCenter
                        width: 18
                        height:18
                        property bool pressed
                        property bool down
                        property bool hovered
                        Ripple {
                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            width: 22; height: 22

                            z: -1
                            anchor: delete_img
                            pressed: delete_img.pressed
                            active: delete_img.down || delete_img.visualFocus || delete_img.hovered
                            color: delete_img.Material.rippleColor
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered:
                            {
                                delete_img.hovered = true
                            }

                            onExited:
                            {
                                delete_img.hovered = false
                            }

                            onReleased:
                            {
                                delete_img.pressed = false
                            }

                            onPressed:
                            {
                                delete_img.pressed = true
                            }
                            onClicked:
                            {
                                terminal.text=""
                                messageArea.text=""
                                rpcConsole.startFromQML()

                            }
                        }

                    }
					Action {
        				id: clearAction
        				text: qsTr("&Clear")
        				icon.name: "edit-clear"
        				shortcut: "Ctrl+L"
        				onTriggered: 
						{
                            terminal.text=""
                            messageArea.text=""
                            rpcConsole.startFromQML()
                        }
    				}


                }

            }

            Controls_1_4.Tab {
                title: qsTr("Net Info")


                Rectangle{
                    id:tab3_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Rectangle
                    {
                        id:traffic_rec
                        color: "white"
                        anchors.left: parent.left
                        anchors.bottom: time_span_slider.top
                        anchors.top:parent.top
                        anchors.right: traffic_label.left
                        anchors.margins: 10
                        property alias painter: painter

                        TrafficGraphWidget
                        {
                            id: painter
                            anchors.fill: parent

                            Component.onCompleted:
                            {
                                console.log("client_model setting by QML")
                                //                              client_model = rpcConsole.getClientModel()
                                //                              setGraphRangeMins(30)
                            }
                        }

                        function clear()
                        {
                            painter.clear()
                        }
                        function setTrafficGraphRange(min)
                        {
                            painter.setGraphRangeMins(min)
                        }

                    }

                    Component.onCompleted:
                    {
                        traffic_label.text = rpcConsole.getStartUpTraffic()
                    }

                    Connections
                    {
                        target: rpcConsole

                        onTrafficUpdate:
                        {
                            traffic_label.text = traffic
                        }
                    }

                    Label
                    {
                        id:traffic_label
                        font.weight: Font.Light
                        font.pixelSize:13
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.top: parent.top
                        width:150
                        anchors.topMargin: 10
                        wrapMode:Text.WrapAnywhere
                        font.letterSpacing:0.3
                        text:"Totals\n  Received:\t0B\n  Sent:\t0B"
                    }

                    CommonSlider
                    {
                        id:time_span_slider
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10
                        width:400
                        height:16
                        from: 1
                        value: 6
                        to: 288
                        live:false

                        onValueChanged:
                        {
                            time_span_label.text = rpcConsole.setTrafficGraphRange(value*5)
                            traffic_rec.setTrafficGraphRange(value*5)

                        }
                    }

                    Label
                    {
                        id:time_span_label
                        font.weight: Font.Light
                        font.pixelSize:14
                        anchors.left: time_span_slider.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: time_span_slider.verticalCenter
                        wrapMode:Text.WrapAnywhere
                        font.letterSpacing:0.3
                        text:"30m"
                    }

                    CommonButton
                    {
                        id:clear_btn
                        color: "#469AAC"
                        anchors.right: traffic_rec.right
                        anchors.verticalCenter: time_span_slider.verticalCenter
                        text:qsTr("Clear")
                        width: 60
                        height: 28
                        textSize:12
                        onClicked:
                        {
                            traffic_rec.clear()
                        }
                    }


                }



            }

            Controls_1_4.Tab {
                title: qsTr("Peer Info")

                onVisibleChanged:
                {
                    if(visible){
                        if(peerTimerStarted)
                            return

                        rpcConsole.peerTableModel_sorted.startAutoRefresh()
                        peerTimerStarted = true
                    }
                    else{
                        if(!peerTimerStarted)
                            return
                        rpcConsole.peerTableModel_sorted.stopAutoRefresh()
                        peerTimerStarted = false
                    }
                }


                Rectangle
                {
                    id:tab4_rec
                    anchors.fill: parent
                    color: "#FAFAFA"


                    CommonTableView
                    {
                        id:address_table
                        anchors.top:parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 10
                        width:420
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10
                        roles:  ["address","version","ping"]
                        titles: ["Address/Hostname","Version","Ping Time"]
                        widths: [160,120,90,width-380]
                        model: rpcConsole.peerTableModel_sorted
                        selectionMode:Controls_1_4.SelectionMode.SingleSelection
                        //dontPerformSortInner:true

                        //                        onSortIndicatorColumnChanged: {
                        //                            rpcConsole.peerTableModel_sorted.sortPeerTable(getColumnbyRole(),sortIndicatorOrder)
                        //                        }
                        //                        onSortIndicatorOrderChanged: {
                        //                            rpcConsole.peerTableModel_sorted.sortPeerTable(getColumnbyRole(),sortIndicatorOrder)
                        //                        }

                        function getColumnbyRole()
                        {
                            if(getColumn(sortIndicatorColumn).role === "address") return  0
                            else if (getColumn(sortIndicatorColumn).role === "version")return  1
                            else if (getColumn(sortIndicatorColumn).role === "ping")return  2
                        }

                        onCurrentRowChanged:
                        {
                            if(rowCount <= 0 )
                                return

                            if(currentRow != -1)
                            {
                                detail_title.text = rpcConsole.peerTableModel_sorted.getPeerTitleInfo(currentRow)
                                detail_text.text = rpcConsole.peerTableModel_sorted.getPeerDetailedInfo(currentRow)
                            }


                        }

                    }

                    Label
                    {
                        id:detail_title
                        anchors.top:parent.top
                        anchors.left: address_table.right
                        anchors.right: parent.right
                        horizontalAlignment:Text.AlignHCenter
                        height:60
                        anchors.margins: 20
                        color: "#469AAC"
                        font.weight: Font.Medium
                        font.pixelSize:14
                        font.letterSpacing:0.3

                    }

                    Label
                    {
                        id:detail_text_leading
                        anchors.top:detail_title.bottom
                        anchors.left: address_table.right
                        width:100
                        anchors.margins: 20
                        font.weight: Font.Medium
                        font.pixelSize:13
                        font.letterSpacing:0.3
                        text:"Direction\nProtocol\nVersion\nServices\nStarting Height\nSync Height\nBan Score\nConnection Time\nLast Send\nLast Receive\nBytes Send\nBytes Received\nPing Time\n"
                        visible: address_table.currentRow != -1
                    }

                    Label
                    {
                        id:detail_text
                        anchors.top:detail_title.bottom
                        anchors.left: detail_text_leading.right
                        anchors.right: parent.right
                        anchors.margins: 20
                        anchors.leftMargin: 30
                        font.weight: Font.Medium
                        font.pixelSize:13
                        font.letterSpacing:0.3

                    }


                }


            }

            Controls_1_4.Tab {
                title: qsTr("Wallet Repair")


                Rectangle
                {
                    id:tab5_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:wallet_repair_label
                        anchors.top:parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 20
                        font.weight: Font.Light
                        font.pixelSize:14
                        font.letterSpacing:0.3
                        wrapMode: Label.WordWrap
                        text:qsTr("<b>Wallet repair options.</b><br> \
                             The buttons below will restart the wallet with command-line options to repair the wallet, fix issues with corrput blockchain files or missing/obsolete transactions.<br>\
                             Wallet In Use:  ") + rpcConsole.getWalletPath()

                    }

                    CommonButton
                    {
                        id:salvage_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: wallet_repair_label.bottom
                        anchors.topMargin: 80
                        text:qsTr("Salvage wallet")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            rpcConsole.walletSalvage();
                        }
                    }

                    CommonButton
                    {
                        id:rescan_block_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: salvage_btn.bottom
                        anchors.topMargin: 15
                        text:qsTr("Rescan blockchain files")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            rpcConsole.walletRescan();
                        }
                    }

                    CommonButton
                    {
                        id:recover_tx_1_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: rescan_block_btn.bottom
                        anchors.topMargin: 15
                        text:qsTr("Recover transactions 1")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            rpcConsole.walletZaptxes1();
                        }
                    }

                    CommonButton
                    {
                        id:recover_tx_2_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: recover_tx_1_btn.bottom
                        anchors.topMargin: 15
                        text:qsTr("Recover transactions 2")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            rpcConsole.walletZaptxes2()
                        }
                    }

                    CommonButton
                    {
                        id:upgrade_format_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: recover_tx_2_btn.bottom
                        anchors.topMargin: 15
                        text:qsTr("Upgrade wallet format")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            rpcConsole.walletUpgrade()
                        }
                    }

                    CommonButton
                    {
                        id:rebuild_index_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: upgrade_format_btn.bottom
                        anchors.topMargin: 15
                        text:qsTr("Rebuild index")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            rpcConsole.walletReindex()
                        }
                    }


                    CommonButton
                    {
                        id:delete_local_blockchain_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.top: rebuild_index_btn.bottom
                        anchors.topMargin: 15
                        text:qsTr("Delete local blockchain Folders")
                        width: 220
                        height: 28
                        textSize:13
                        onClicked:
                        {
                            confirm_dialog.show()
                        }


                        CommonDialog
                        {
                            id:confirm_dialog
                            title: qsTr("resync wallet")
                            confirm_btn_text: qsTr("Yes")
                            cancel_btn_text: qsTr("No")
                            content_text: qsTr("This will delete your local blockchain folders and the wallet will synchronize the complete Blockchain from scratch.\nConfirm?")
                            width:400
                            height: 300
                            modality: Qt.WindowModal
                            onConfirmed:
                            {
                                rpcConsole.walletResync();
                            }

                        }
                    }

                    Label
                    {
                        id:options_label
                        anchors.left: salvage_btn.right
                        anchors.leftMargin: 20
                        anchors.top: wallet_repair_label.bottom
                        anchors.topMargin: 80
                        font.weight: Font.Light
                        font.pixelSize:14
                        font.letterSpacing:0.3
                        lineHeight: 2.18
                        text:qsTr("-salvagewallet:
-rescan:
-zapwallettxes=1:
-zapwallettxes=2:
-upgradewallet:
-reindex:
-resync:")

                    }

                    Label
                    {
                        id:description_label
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        width:250
                        anchors.top: wallet_repair_label.bottom
                        anchors.topMargin: 80
                        font.weight: Font.Light
                        font.pixelSize:13
                        font.letterSpacing:0.3
                        lineHeight: 0.97
                        wrapMode: Label.WordWrap
                        text:qsTr("Attempt to recover private keys from a corrupt wallet.dat.
Rescan the block chain for missing wallet transactions.
Recover transactions from blockchain(keep meta-data e.g. account owner)
Recover transactions from blockchain(drop meta-data)
Upgrade wallet to latest format on startup.(Note:this is NOT an update of the wallet itself!)
Rebuild block chain index from current blk001??.dat files.
Deletes all local blockchain folders so the wallet synchronizes from scratch.")

                    }


                }

            }


            /*Controls_1_4.Tab {
                title: qsTr("Add Address")


                Rectangle
                {
                    id:tab6_rec
                    anchors.fill: parent
                    color: "#FAFAFA"
                    Label
                    {
                        id:show_BIP39Words_label
                        anchors.top:parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 20
                        font.weight: Font.Light
                        font.pixelSize:14
                        font.letterSpacing:0.3
                        wrapMode: Label.WordWrap
                        text:qsTr("<b>Add address from bip39 words:</b>")
                    }


                    CommonButton
                    {
                        id:showWordsbtn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        anchors.top: show_BIP39Words_label.bottom
                        anchors.topMargin: 20
                        text:qsTr("Add")
                        width: 100
                        height: 28
                        textSize:11
                        onClicked:
                        {
                            if(!requestUnlock())
                            {
                                walletModel.addAddressByWords(wordsArea.text.trim())
                            }
                        }
                    }


                    Label
                    {
                        id:wordsLabel
                        font.weight: Font.Normal
                        font.pixelSize:13
                        anchors.top:showWordsbtn.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 20
                        anchors.leftMargin: 20
                        color: "#333333"
                        text:qsTr("BIP39 Words:")
                    }

                    CommonTextArea
                    {
                        id:wordsArea
                        font.weight: Font.Light
                        font.pixelSize:14
                        anchors.top: wordsLabel.top
                        anchors.left: wordsLabel.right
                        anchors.leftMargin: 15
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        height:100
                        wrapMode: TextEdit.Wrap

                        ToolTip.text: qsTr("Please input 12 words, split by comma.")

                        onTextChanged:
                        {
                            if(text.length == 0)
                            ToolTip.visible = true


                        }

                        Connections
                        {
                            target:walletModel

                            onBadWords:
                            {

                                root_window.warningDialog.title = qsTr("Error")
                                root_window.warningDialog.content_text = qsTr("invalid input, Please check your words!")
                                root_window.warningDialog.show()

                            }

                            onExistingAddress:
                            {
                                root_window.warningDialog.title = qsTr("Error")
                                root_window.warningDialog.content_text = qsTr("You have this address in your wallet already!")
                                root_window.warningDialog.show()
                            }

                            onAddAddressSuccessful:
                            {
                              //  root_window.question(qsTr("Success"),qsTr("You have successfully add address:\n\t") + address + qsTr("\nto your wallet!\n Please wait for transaction rescan!"),function(){walletModel.rescanWallet();})
                                root_window.warningDialog.title = qsTr("Success")
                                root_window.warningDialog.content_text = qsTr("You have successfully add address:\n\t") + address + qsTr("\nto your wallet!\n Plaese rescan blockchain files to get transactions for this address.")
                                root_window.warningDialog.show()
                            }

                        }




                    }



                }

            }*/


        }

    }

}
