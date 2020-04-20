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
import "../app_pages"
import "../app_items"
import "../base_items"


CommonDialog
{
    id:root
    title: qsTr("MultiSignature Address Interactions")

    confrim_btn_visible:false
    cancel_btn_visible:false
    property alias current_index:tabview.currentIndex

    onCurrent_indexChanged:
    {
        root.setWidth(tabview.currentIndex==1?800:642)
        root.setHeight(tabview.currentIndex==1?550:tabview.currentIndex==2?500:395)
    }

    Item{
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 25

        CommonTabView{
            id:tabview
            tab_width:185
            currentIndex: -1
            Controls_1_4.Tab {
                title: qsTr("Create MultiSignature Address")


                Rectangle{
                    id:tab1_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    CommonTextField
                    {
                        id:minimal_sign_field
                        font.weight: Font.Light
                        font.pixelSize:16
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        width: 50
                        height:26
                    }

                    Label
                    {
                        id:minimal_sign_label
                        anchors.verticalCenter: minimal_sign_field.verticalCenter

                        anchors.left: minimal_sign_field.right
                        anchors.leftMargin: 5

                        text: qsTr("Enter the minimal number of signatures required to sign transactions")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }



                    CommonButton
                    {
                        id:add_address_key_btn
                        color: "#469AAC"
                        anchors.left: minimal_sign_field.left
                        anchors.top:minimal_sign_field.bottom
                        anchors.topMargin: 8
                        text:qsTr("Add Address/Key")
                        width: 100
                        height: 28
                        textSize:9
                        onClicked: {
                        }
                    }

                    Label
                    {
                        id:add_address_label
                        anchors.verticalCenter: add_address_key_btn.verticalCenter

                        anchors.left: add_address_key_btn.right
                        anchors.leftMargin: 5

                        text: qsTr("Local addresses or public keys that can sign")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }


                    ListView {
                        id:address_listview
                        clip: true
                        width:parent.width
                        anchors.top:add_address_key_btn.bottom
                        anchors.topMargin: 5
                        anchors.left: add_address_key_btn.left
                        anchors.right: parent.right
                        height:75

                        boundsBehavior:Flickable.StopAtBounds

                        currentIndex: -1

                        delegate:ItemDelegate {
                            height:38
                            hoverEnabled: false
                            AddressItem {
                                width: address_listview.width - 20
                                address_title: title
                            }

                        }

                        model: ListModel {
                            ListElement { title: "Address/Key 1:"; }
                            ListElement { title: "Address/Key 2:"; }
                            ListElement { title: "Address/Key 3:"; }
                            ListElement { title: "Address/Key 4:"; }
                            ListElement { title: "Address/Key 5:"; }

                        }
                        ScrollBar.vertical: MyScrollBar {
                            id: scrollBar
                            width:10
                            height:30
                            anchors.right: address_listview.right
                            policy:ScrollBar.AsNeeded
                            visible: address_listview.count * 38 > address_listview.height
                            handle.implicitHeight: 100
                        }
                    }

                    CommonButton
                    {
                        id:create_btn
                        color: "#469AAC"
                        anchors.left: address_listview.left
                        anchors.top:address_listview.bottom
                        anchors.topMargin: 8
                        text:qsTr("Create")
                        width: 80
                        height: 28
                        textSize:9
                        onClicked: {
                        }
                    }

                    Label
                    {
                        id:status_label
                        anchors.verticalCenter: create_btn.verticalCenter

                        anchors.left: create_btn.right
                        anchors.leftMargin: 10

                        text: qsTr("Status:")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }
                    Label
                    {
                        id:status_field
                        font.weight: Font.Medium
                        font.pixelSize:11
                        font.letterSpacing: 0.355
                        anchors.left: status_label.right
                        anchors.verticalCenter: create_btn.verticalCenter
                        anchors.right: parent.right
                        height: 40
                        text:qsTr("a Multisignature address must require at least one key to redeem")
                        topPadding: 8
                        leftPadding: 10
                        background: Rectangle
                        {
                            color:"#F0F0F0"
                        }
                    }


                    Label
                    {
                        id:note_label
                        anchors.top:status_field.bottom
                        anchors.topMargin: 10
                        width:parent.width
                        text: qsTr("Use below to quickly import an address by its redeem. Don't forget to add a label before clicking import! Keep in mind, the wallet will rescan the blockchain to find transactions containing the new address.Please be patient after clicking import.")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }

                    CommonButton
                    {
                        id:redeem_btn
                        color: "#469AAC"
                        anchors.left: note_label.left
                        anchors.top:note_label.bottom
                        anchors.topMargin: 20
                        text:qsTr("Import Redeem")
                        width: 100
                        height: 28
                        textSize:9
                        onClicked: {
                        }
                    }

                    CommonTextField
                    {
                        id:redeem_field
                        font.weight: Font.Light
                        font.pixelSize:16
                        anchors.left: redeem_btn.right
                        anchors.leftMargin: 20
                        anchors.right: parent.right
                        anchors.verticalCenter: redeem_btn.verticalCenter
                        height:35
                    }

                }

            }
            Controls_1_4.Tab {
                title: qsTr("Create MultiSignature TX")


                Rectangle{
                    id:tab2_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:input_label
                        anchors.top:parent.top
                        anchors.topMargin: 22
                        width:95
                        text: "Inputs:"
                        font.weight: Font.Light
                        font.pixelSize: 11
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }

                    CommonButton
                    {
                        id:coin_control_btn
                        color: "#469AAC"
                        anchors.left: input_label.right
                        anchors.verticalCenter: input_label.verticalCenter
                        anchors.leftMargin: 2
                        width: 145
                        height: 30
                        radius: 3
                        text:qsTr("Coin Control")
                        textSize:11
                        letterSpacing:0
                    }

                    Label
                    {
                        id:quantity_label
                        font.weight: Font.Medium
                        font.pixelSize:11
                        font.letterSpacing: 0.20
                        anchors.left: coin_control_btn.right
                        anchors.leftMargin: 4
                        anchors.verticalCenter: coin_control_btn.verticalCenter
                        anchors.verticalCenterOffset: -12
                        color: "#333333"
                        text:qsTr("Quantity Selected:  0")
                    }

                    Label
                    {
                        id:amount_label
                        font.weight: Font.Medium
                        font.pixelSize:11
                        font.letterSpacing: 0.20
                        anchors.left: coin_control_btn.right
                        anchors.leftMargin: 4
                        anchors.verticalCenter: coin_control_btn.verticalCenter
                        anchors.verticalCenterOffset: 12
                        color: "#333333"
                        text:qsTr("Amount:                 0")
                    }


                    CommonButton
                    {
                        id:add_a_raw_btn
                        color: "#469AAC"
                        anchors.right: parent.right
                        anchors.verticalCenter: input_label.verticalCenter
                        width: 145
                        height: 30
                        radius: 3
                        text:qsTr("Add a Row Input")
                        textSize:11
                        letterSpacing:0

                        onClicked:
                        {
                            raw_list_model.append({ "title": ". Tx Hash:"})
                        }
                    }


                    ListView {
                        id:rawinput_listview
                        clip: true
                        width:parent.width
                        anchors.top:add_a_raw_btn.bottom
                        anchors.topMargin: 10
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height:count>0?130:0

                        boundsBehavior:Flickable.StopAtBounds

                        currentIndex: -1

                        delegate:ItemDelegate {
                            height:38
                            hoverEnabled: false
                            RawInputItem
                            {
                                width: rawinput_listview.width - 20
                                input_title: index+title

                                onRm_raw_input:
                                {
                                    console.log("remove index:"+index)

                                    raw_list_model.remove(index)
                                }
                            }

                        }
                        ListModel {
                            id:raw_list_model
                            ListElement { title: ". Tx Hash:"; }
                            ListElement { title: ". Tx Hash:"; }
                            ListElement { title: ". Tx Hash:"; }
                        }


                        model: raw_list_model
                        ScrollBar.vertical: MyScrollBar {
                            width:10
                            height:30
                            anchors.right: rawinput_listview.right
                            policy:ScrollBar.AsNeeded
                            visible: rawinput_listview.count * 38 > rawinput_listview.height
                            handle.implicitHeight: 100
                        }
                    }





                    Label
                    {
                        id:address_amount_label
                        anchors.top:rawinput_listview.bottom
                        anchors.topMargin: 15
                        width:95
                        text: qsTr("Address/Amount:")
                        font.weight: Font.Light
                        font.pixelSize: 11
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }

                    CommonButton
                    {
                        id:add_destination_btn
                        color: "#469AAC"
                        anchors.left: address_amount_label.right
                        anchors.verticalCenter: address_amount_label.verticalCenter
                        anchors.leftMargin: 2
                        width: 145
                        height: 30
                        radius: 3
                        text:qsTr("Add Destination")
                        textSize:11
                        letterSpacing:0
                        onClicked:
                        {
                            address_list_model.append({ "title": ". Address:"})
                        }
                    }

                    ListView {
                        id:address_listview2
                        clip: true
                        width:parent.width
                        anchors.top:add_destination_btn.bottom
                        anchors.topMargin: 10
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height:count>0?130:0

                        boundsBehavior:Flickable.StopAtBounds

                        currentIndex: -1

                        delegate:ItemDelegate {
                            height:38
                            hoverEnabled: false
                            AddressInputItem
                            {
                                width: address_listview2.width - 20
                                address_title: index+title

                                onRm_address_input:
                                {
                                    console.log("remove index:"+index)

                                    address_list_model.remove(index)
                                }
                            }

                        }
                        ListModel {
                            id:address_list_model
                            ListElement { title: ". Address:"; }
                            ListElement { title: ". Address:"; }
                            ListElement { title: ". Address:"; }
                        }


                        model: address_list_model
                        ScrollBar.vertical: MyScrollBar {
                            width:10
                            height:30
                            anchors.right: address_listview2.right
                            policy:ScrollBar.AsNeeded
                            visible: address_listview2.count * 38 > address_listview2.height
                            handle.implicitHeight: 100
                        }
                    }



                    CommonButton
                    {
                        id:create_btn2
                        color: "#469AAC"
                        anchors.left: address_listview2.left
                        anchors.top:address_listview2.bottom
                        anchors.topMargin: 25
                        text:qsTr("Create")
                        width: 80
                        height: 28
                        textSize:9
                        onClicked: {
                        }
                    }

                    Label
                    {
                        id:status_label2
                        anchors.verticalCenter: create_btn2.verticalCenter

                        anchors.left: create_btn2.right
                        anchors.leftMargin: 10

                        text: qsTr("Status:")
                        font.weight: Font.Light
                        font.pixelSize: 9
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }

                    Label
                    {
                        id:status_field2
                        font.weight: Font.Medium
                        font.pixelSize:11
                        font.letterSpacing: 0.355
                        anchors.left: status_label2.right
                        anchors.verticalCenter: create_btn2.verticalCenter
                        anchors.right: parent.right
                        height: 50
                        text:qsTr("Invalid Tx Hash.")
                        topPadding: 8
                        leftPadding: 10
                        background: Rectangle
                        {
                            color:"#F0F0F0"
                        }
                    }




                }

            }
            Controls_1_4.Tab {
                title: qsTr("Sign MultiSignature TX")


                Rectangle{
                    id:tab3_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:tx_label
                        anchors.top: parent.top
                        anchors.topMargin: 20


                        anchors.left: parent.left

                        text: qsTr("Transaction Hex:")
                        font.weight: Font.Medium
                        font.pixelSize: 11
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }


                    CommonTextField
                    {
                        id:tx__field
                        font.weight: Font.Light
                        font.pixelSize:16
                        anchors.right: parent.right
                        anchors.left: tx_label.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: tx_label.verticalCenter
                        width: 50
                        height:28
                    }

                    CommonButton
                    {
                        id:sign_multi_btn
                        color: "#469AAC"
                        anchors.left: tx_label.left
                        anchors.top:tx_label.bottom
                        anchors.topMargin: 25
                        text:qsTr("Sign")
                        width: 60
                        height: 28
                        textSize:11
                        onClicked: {
                        }
                    }

                    CommonButton
                    {
                        id:commit_sign_btn
                        color:walletModel.isTestNet()?"#202020":"#1E5569"
                        anchors.left: sign_multi_btn.right
                        anchors.leftMargin: 15
                        anchors.verticalCenter: sign_multi_btn.verticalCenter
                        text:qsTr("Commit")
                        width: 60
                        height: 28
                        textSize:11
                        onClicked: {
                        }
                    }



                    CommonButton
                    {
                        id:add_private_key_btn
                        color: "#EE637F"
                        anchors.left: commit_sign_btn.right
                        anchors.leftMargin: 15
                        anchors.verticalCenter: sign_multi_btn.verticalCenter

                        text:qsTr("Add Private Key")
                        width: 100
                        height: 28
                        textSize:11
                        onClicked:
                        {
                            private_keys_list_model.append({"title":"1"})
                        }
                    }

                    Label
                    {
                        id:tx_notice_label
                        anchors.verticalCenter: sign_multi_btn.verticalCenter

                        anchors.left: add_private_key_btn.right
                        anchors.leftMargin: 25

                        text: qsTr("Sign with private keys only (Not Recommended)")
                        font.weight: Font.Medium
                        font.pixelSize: 11
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.5
                    }

                    Label
                    {
                        id:status3_label
                        anchors.left: sign_multi_btn.left
                        anchors.top:sign_multi_btn.bottom
                        anchors.topMargin: 40
                        text: qsTr("Status:")
                        font.weight: Font.Medium
                        font.pixelSize: 11
                        wrapMode: Text.WrapAnywhere
                        lineHeight: 0.8
                        font.letterSpacing:0.3
                    }
                    Label
                    {
                        id:status3_field
                        font.weight: Font.Medium
                        font.pixelSize:11
                        font.letterSpacing: 0.355
                        anchors.left: status3_label.right
                        anchors.leftMargin: 6
                        anchors.verticalCenter: status3_label.verticalCenter
                        anchors.right: parent.right
                        height: 60
                        text:""
                        topPadding: 8
                        leftPadding: 10
                        background: Rectangle
                        {
                            color:"#F0F0F0"
                        }
                    }

                    ListView {
                        id:private_keys_listview
                        clip: true
                        anchors.top:status3_field.bottom
                        anchors.topMargin: 15
                        anchors.left: status3_label.left
                        anchors.right: parent.right
                        width: parent.width
                        height:200

                        boundsBehavior:Flickable.StopAtBounds

                        currentIndex: -1

                        delegate:ItemDelegate {
                            height:38
                            hoverEnabled: false
                            PrivateItem {
                                width: private_keys_listview.width
                                private_keys_title: "Key "+index+" :"

                                onRm_private_key:
                                {
                                    console.log("remove index:"+index)

                                    private_keys_list_model.remove(index)
                                }
                            }



                        }
                        ListModel {
                            id:private_keys_list_model
                            ListElement {title:"1"}
                            ListElement {title:"1"}
                            ListElement {title:"1"}
                            ListElement {title:"1"}
                            ListElement {title:"1"}
                        }

                        model:private_keys_list_model

                        ScrollBar.vertical: MyScrollBar {
                            id: scrollBar3
                            width:10
                            height:30
                            anchors.right: private_keys_listview.right
                            policy:ScrollBar.AsNeeded
                            visible: private_keys_listview.count * 38 > private_keys_listview.height
                            handle.implicitHeight: 100
                        }
                    }








                }

            }

        }

    }

}
