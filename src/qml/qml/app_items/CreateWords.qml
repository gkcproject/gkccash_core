import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id:root
    color: "#212B50"

    Label{
        id:label_9
        font.pixelSize:16
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 32
        anchors.leftMargin: 16
        color: "#FFFFFF"
        text:"Your vallet has been created sucessfully, Using thefollowing sequence of \nvords, youwill be able to recoveryour wallet at any time."
    }

    Row{
        id:row_1
        anchors.top:label_9.bottom
        anchors.left: label_9.left
        anchors.topMargin: 16
        height: 40
        spacing:16

        Seedword{
            index_str : "1"
            word_str: mnemonicManager.GetMnemonicWord(1)
        }

        Seedword{
            index_str : "2"
            word_str: mnemonicManager.GetMnemonicWord(2)
        }

        Seedword{
            index_str : "3"
            word_str: mnemonicManager.GetMnemonicWord(3)
        }

        Seedword{
            index_str : "4"
            word_str: mnemonicManager.GetMnemonicWord(4)
        }

        Seedword{
            index_str : "5"
            word_str: mnemonicManager.GetMnemonicWord(5)
        }

        Seedword{
            index_str : "6"
            word_str: mnemonicManager.GetMnemonicWord(6)
        }
    }

    Row{
        id:row_2
        anchors.top:row_1.bottom
        anchors.left: row_1.left
        anchors.topMargin: 16
        height: 40
        spacing:16

        Seedword{
            index_str : "7"
            word_str: mnemonicManager.GetMnemonicWord(7)
        }

        Seedword{
            index_str : "8"
            word_str: mnemonicManager.GetMnemonicWord(8)
        }

        Seedword{
            index_str : "9"
            word_str: mnemonicManager.GetMnemonicWord(9)
        }

        Seedword{
            index_str : "10"
            word_str: mnemonicManager.GetMnemonicWord(10)
        }

        Seedword{
            index_str : "11"
            word_str: mnemonicManager.GetMnemonicWord(11)
        }

        Seedword{
            index_str : "12"
            word_str: mnemonicManager.GetMnemonicWord(12)
        }
    }

    Row{
        id:row_3
        anchors.top:row_2.bottom
        anchors.left: row_2.left
        anchors.topMargin: 16
        height: 40
        spacing:16

        Seedword{
            index_str : "13"
            word_str: mnemonicManager.GetMnemonicWord(13)
        }

        Seedword{
            index_str : "14"
            word_str: mnemonicManager.GetMnemonicWord(14)
        }

        Seedword{
            index_str : "15"
            word_str: mnemonicManager.GetMnemonicWord(15)
        }

        Seedword{
            index_str : "16"
            word_str: mnemonicManager.GetMnemonicWord(16)
        }

        Seedword{
            index_str : "17"
            word_str: mnemonicManager.GetMnemonicWord(17)
        }

        Seedword{
            index_str : "18"
            word_str: mnemonicManager.GetMnemonicWord(18)
        }
    }

    Row{
        id:row_4
        anchors.top:row_3.bottom
        anchors.left: row_3.left
        anchors.topMargin: 16
        height: 40
        spacing:16

        Seedword{
            index_str : "19"
            word_str: mnemonicManager.GetMnemonicWord(19)
        }

        Seedword{
            index_str : "20"
            word_str: mnemonicManager.GetMnemonicWord(20)
        }

        Seedword{
            index_str : "21"
            word_str: mnemonicManager.GetMnemonicWord(21)
        }

        Seedword{
            index_str : "22"
            word_str: mnemonicManager.GetMnemonicWord(22)
        }

        Seedword{
            index_str : "23"
            word_str: mnemonicManager.GetMnemonicWord(23)
        }

        Seedword{
            index_str : "24"
            word_str: mnemonicManager.GetMnemonicWord(24)
        }
    }

    Label{
        id:label_11
        font.pixelSize:16
        anchors.top:row_4.bottom
        anchors.left: row_4.left
        anchors.topMargin: 16
        color: "#FFFFFF"
        text:"Please write down these words and keep them in a securelocation such as a safe."
    }

    Label{
        id:label_12
        font.pixelSize:16
        anchors.top:label_11.bottom
        anchors.left: label_11.left
        anchors.topMargin: 16
        color: "#FFFFFF"
        text:"VARNING:Anyone with access to these words can recoveryour wallet and will have \ncontrol over your funds!"
    }

}
