import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style

import "../app_items"
import "../app_dialogs"

Controls_1_4.Tab {

    Rectangle
    {
        id:background
        anchors.fill: parent
        color: "#212b50"

        Connections
        {
            target:walletModel.contractPage

            onNotifyContractResult:
            {
                if(error)
                {
                    root_window.warningDialog.title = title
                    root_window.warningDialog.content_text = errMsg
                    root_window.warningDialog.dim_back = false
                    root_window.warningDialog.show()
                    return
                }


                root.openPage((qsTr("Result ")+ (root.count-2)))





            }

        }




        Controls_1_4.TabView{
            id:root
            currentIndex: 0
            anchors.fill:parent



            style:Controls_1_4_style.TabViewStyle
            {
                frameOverlap: 1
                tab: Rectangle
                {
                    id:tab_rec
                    color: styleData.index > 2?"#F0F0F0":"#212b50"
                    implicitWidth: 100
                    implicitHeight: 50

                    Text
                    {
                        id: text
                        anchors.horizontalCenter: tab_rec.horizontalCenter
                        anchors.verticalCenter: tab_rec.verticalCenter
                        anchors.verticalCenterOffset: 5
                        text: styleData.title
                        color: styleData.selected ?"#FDA205":"#6B78A8"
                        font.pixelSize: 15
                        font.weight:Font.Normal
                    }

                    Rectangle
                    {
                        color: "#212b50"
                        height: 2
                        width: 65
                        anchors.horizontalCenter: tab_rec.horizontalCenter
                        anchors.bottom: tab_rec.bottom
                        anchors.bottomMargin: 0
                        radius:1
                        visible: styleData.selected
                    }

                    Rectangle
                    {
                        id: close_btnRect
                        color: Qt.rgba(0,0,0,0)
                        width:14
                        height:14
                        radius:7
                        anchors.right: tab_rec.right
                        anchors.rightMargin: 2
                        anchors.topMargin: 10

                        anchors.top:tab_rec.top
                        visible: styleData.index > 2
                        PropertyAnimation {id: animateColor_in; target: close_btnRect; properties: "color"; to: "#469AAC"; duration: 200}
                        PropertyAnimation {id: animateColor_out; target: close_btnRect; properties: "color"; to: "transparent"; duration: 200}

                        Image{
                            id:close_btn
                            source: "qrc:/images/icons/tab_close.png"
                            width:8
                            height:8
                            anchors.right: close_btnRect.right
                            anchors.rightMargin: 3
                            anchors.top:close_btnRect.top
                            anchors.topMargin: 3
                        }


                        MouseArea{
                            anchors.fill: close_btnRect
                            hoverEnabled:true

                            onClicked: {
                                root.closePage(styleData.title,styleData.index)
                            }

                            onEntered: {
                                animateColor_in.start()
                            }

                            onExited: {
                                animateColor_out.start()
                            }
                        }}




                }
                // frame: Rectangle { color: "transparent" }
            }




            Controls_1_4.Tab
            {
                title: qsTr("Create")

                CommonContractPage
                {
                    id:createPage
                    anchors.fill: parent
                    pageType:create

                }


            }

            Controls_1_4.Tab
            {
                title: qsTr("Send To")

                CommonContractPage
                {
                    id:sendToPage
                    anchors.fill: parent
                    pageType:sendTo

                }
            }

            Controls_1_4.Tab
            {
                title: qsTr("Call")

                CommonContractPage
                {
                    id:callPage
                    anchors.fill: parent
                    pageType:call

                }
            }

            function openPage(title_)
            {

                var component

                component = Qt.createComponent("qrc:/qml/app_items/ContractResult.qml")

                if (component.status === Component.Ready){
                   var tab =  addTab(title_,component)
                }

                root.currentIndex = root.count - 1

            }


            function closePage(title_,index_)
            {
                var tab =  getTab(index_)
                tab.sourceComponent = undefined
                root.removeTab(index_)
            }
        }

    }
}
