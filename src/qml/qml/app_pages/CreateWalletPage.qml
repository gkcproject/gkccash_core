import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.3

import "../app_items"

ApplicationWindow {
    id: window
    visible: true
    x:(Screen.width-680)/2
    y:(Screen.height-864)/2
    width: 680
    height: 864

    title: qsTr("Create or Recover Wallet")
    color: "#F1F3F4"
    flags:  Qt.FramelessWindowHint

    Material.elevation: 8

    property bool indexVisible: true
    property bool createVisible: false
    property bool checkVisible: false
    property bool recoverVisible: false
    property bool bottomVisible: false
    property bool backVisible: false

    property int pageindex: 0

    function clearPage()
    {
        indexVisible = false
        createVisible = false
        checkVisible = false
        recoverVisible = false
        bottomVisible = false
        backVisible = false
    }

    function showIndexPage()
    {
        clearPage()
        indexVisible = true
        pageindex = 0
    }

    function showCreatePage()
    {
        clearPage()
        createVisible = true
        bottomVisible = true
        backVisible = true
        pageindex = 1
    }

    function showCheckPage()
    {
        clearPage()
        checkVisible = true
        bottomVisible = true
        backVisible = true
        pageindex = 2
        check_page.init()
    }

    function showRecoverPage()
    {
        clearPage()
        recoverVisible = true
        recover_page.init()
        bottomVisible = true
        pageindex = 3
    }
    function backFun()
    {
        if(pageindex===1)
        {
            showIndexPage()
        } else if(pageindex === 2)
        {
            showCreatePage()
        }
    }

    function finishFun()
    {
        if(pageindex===1)
        {
            pageindex = 2
            showCheckPage()
        }
        else if(pageindex === 2)
        {
        	console.log("check_page.checkWords")
			check_page.checkWords()
        }
        else if(pageindex === 3)
        {
			recover_page.checkWords()
        }
    }

    function finishCheck()
    {
    	mnemonicManager.CheckMnemonicFinished()
		Qt.quit()
    }

    Component.onCompleted: {
    	check_page.finishCheckWords.connect(finishCheck)
    	recover_page.finishCheckWords.connect(finishCheck)
    }

    Rectangle{
        id:background
        anchors.fill: parent
        color: "#212B50"
        //radius: radius_all == undefined? 2:radius_all
        objectName: "background"


        Rectangle{
            id:title
            height: 60
            x:0
            y:0
            width:parent.width
            color: "#2A3358"

            Image{
                id:logo_img
                source: "qrc:/images/icons/logo.png"
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 16
                width:130
                height:50
            }


            Label{
                anchors.horizontalCenter:parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize:18
                color: "#FFFFFF"
                text:"Create or Recover Wallet"
            }


            Rectangle
            {
                id: close_btnRect
                color: Qt.rgba(0,0,0,0)
                width:32
                height:32
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter

                PropertyAnimation {id: animateColor_in; target: close_btnRect; properties: "color"; to: "#00A0E9"; duration: 200}
                PropertyAnimation {id: animateColor_out; target: close_btnRect; properties: "color"; to: Qt.rgba(0,0,0,0); duration: 200}

                Image{
                    id:close_btn
                    source: "qrc:/images/icons/mainwindow_close.png"
                    width:20
                    height:20
                    anchors.centerIn: parent
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled:true

                    onClicked:
                    {
                        exit_dialog.show()
                    }
                    onEntered:
                    {
                        animateColor_in.start()
                    }
                    onExited:
                    {
                        animateColor_out.start()
                    }
                }
            }

        }

        Rectangle{
            id:index_page
            x:0
            y:60
            width: parent.width
            height: parent.height-60
            color: "#212B50"

            visible:indexVisible

            TextEdit{
                id:label_1
                font.pixelSize:16
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.topMargin: 32
                anchors.leftMargin: 16
                color: "#FFFFFF"
                width:648
            	wrapMode: TextEdit.Wrap
                text:"Let' s create your GKC wallet!"
            }

            TextEdit{
                id:label_2
                font.pixelSize:16
                anchors.top:label_1.bottom
                anchors.left: label_1.left
                anchors.topMargin: 16
                color: "#FFFFFF"
                width:648
            	wrapMode: TextEdit.Wrap
                text:"Flease save these words on paper (order is important). These words serve asyour seed and will allow you a to recover your wallet in case of a computerfailure."
            }

            TextEdit{
                id:label_3
                font.pixelSize:16
                anchors.top:label_2.bottom
                anchors.left: label_2.left
                anchors.topMargin: 16
                color: "#FFFFFF"
                width:648
            	wrapMode: TextEdit.Wrap
                text:"By default we create a 24 word seed."
            }

            TextEdit{
                id:label_4
                font.pixelSize:16
                anchors.top:label_3.bottom
                anchors.left: label_3.left
                anchors.topMargin: 16
                color: "#FFFFFF"
                width:648
            	wrapMode: TextEdit.Wrap
                text:"You can also choose to further encrypt your seed with a passphrase for added security."
            }

            Label{
                id:label_5
                font.pixelSize:16
                anchors.top:label_4.bottom
                anchors.left: label_4.left
                anchors.topMargin: 16
                color: "#FDA205"
                text:"Warning:"
            }

            Label{
                id:label_6
                font.pixelSize:16
                anchors.top:label_5.bottom
                anchors.left: label_5.left
                anchors.topMargin: 8
                color: "#FDA205"
                text:"* Never disclose your seed\n* Never type it on a website \n* Do not store it electronically.\n* If you encrypt your seed with an addi ti onal passphrase, please also recordthis."
            }

            TextEdit{
                id:label_7
                font.pixelSize:16
                anchors.top:label_6.bottom
                anchors.left: label_6.left
                anchors.topMargin: 16
                color: "#FFFFFF"
                width:648
            	wrapMode: TextEdit.Wrap
                text:"If you have an existing seed that you wish to restore, please select Recoverexisting wallet and enter your passphrase (if you have set one for yourseed)."
            }

            TextEdit{
                id:label_8
                font.pixelSize:16
                anchors.top:label_7.bottom
                anchors.left: label_7.left
                anchors.topMargin: 16
                color: "#FFFFFF"
                width:648
            	wrapMode: TextEdit.Wrap
                text:"Advanced: If you don't wish to generate a wallet with a seed phrase backup(not recommended), you can set us mnemonic=o in gkc.conf."
            }

            Rectangle{
                id:createAction
                anchors.top:label_8.bottom
                anchors.left: label_8.left
                anchors.topMargin: 48
                anchors.leftMargin: 79
                width: 200
                height: 178
                color: "#2E3962"
                border.color: "#2E3962"
                border.width: 2
                radius: 10

                Image{
                    source: "qrc:/images/icons/create.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter:parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 32
                    width:56
                    height:56
                }

                Label{
                    font.pixelSize:16
                    anchors.horizontalCenter:parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 32
                    color: "#FFFFFF"
                    text:"Create new wallet"
                }

                MouseArea{
                    anchors.fill: parent

                    onEntered: {
                        createAction.border.color = "#FDA205"
                    }
                    onExited: {
                        createAction.border.color = "#2E3962"
                    }

                    onClicked: {
                    	console.log("showCreatePage")
                        showCreatePage()
                    }
                }
            }

            Rectangle{
                id:recoverAction
                anchors.top:label_8.bottom
                anchors.right: parent.right
                anchors.topMargin: 48
                anchors.rightMargin: 95
                width: 200
                height: 178
                color: "#2E3962"

                Image{
                    source: "qrc:/images/icons/recover.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter:parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 32
                    width:56
                    height:56
                }

                Label{
                    font.pixelSize:16
                    anchors.horizontalCenter:parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 32
                    color: "#FFFFFF"
                    text:"Recover existing wallet"
                }

                MouseArea{
                    anchors.fill: parent

                    onEntered: {
                        recoverAction.border.color = "#FDA205"
                    }
                    onExited: {
                        recoverAction.border.color = "#2E3962"
                    }

                    onClicked: {
                        showRecoverPage()
                    }
                }
            }
        }

        CreateWords{
            id:create_page
            x:0
            y:60
            width: parent.width
            height: parent.height-200

            visible:createVisible
        }

        CheckWords{
            id:check_page
            x:0
            y:60
            width: parent.width
            height: parent.height-200

            visible:checkVisible
        }

        RecoverWords{
            id:recover_page
            x:0
            y:60
            width: parent.width
            height: parent.height-200

            visible:recoverVisible
        }

        Rectangle{
            id:finish_btn
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.bottomMargin: 16
            anchors.rightMargin: 16
            width: 130
            height: 44
            color: "#FDA205"

            visible: bottomVisible

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    finishFun()
                }
            }

            Label{
                font.pixelSize:18
                color: "#FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "Finish"
            }
        }


        Rectangle{
            id:cancel_btn
            anchors.bottom: finish_btn.bottom
            anchors.right: finish_btn.left
            anchors.rightMargin: 16
            width: 130
            height: 44
            color: "#2E3962"

            visible: bottomVisible

            MouseArea{
                anchors.fill: parent
                onClicked: {
					exit_dialog.show()
                }
            }

            Label{
                font.pixelSize:18
                color: "#FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "Cancel"
            }
        }

        Rectangle{
            id:back_btn
            anchors.bottom: cancel_btn.bottom
            anchors.right: cancel_btn.left
            anchors.rightMargin: 16
            width: 130
            height: 44
            color: "#2E3962"

            visible: backVisible

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    backFun()
                }
            }

            Label{
                font.pixelSize:18
                color: "#FFFFFF"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: "Back"
            }
        }
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
}
