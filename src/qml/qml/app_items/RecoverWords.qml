import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id:root
    color: "#212B50"

    property bool errVisible: false
    property string txtColor: "#FFFFFF"

    signal finishCheckWords()

	function init()
	{
		errVisible = false
		txtColor = "#FFFFFF"
	}

	function checkWords()
	{
		if(!mnemonicManager.CheckMnemonicWords(word_Edit.text))
		{
			errVisible = true
			txtColor = "#EF5653"
		}
		else 
		{			
			finishCheckWords()
		}
	}

    Label{
        id:label_1
        font.pixelSize:16
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 32
        anchors.leftMargin: 16
        color: "#FFFFFF"
        text:"Input words here"
    }

    Rectangle{
        id:edit_rec
        anchors.top: label_1.bottom
        anchors.left: label_1.left
        anchors.right: parent.right
        anchors.topMargin: 24
        anchors.rightMargin: 16
        height: 229
        color: "#2E3962"

        TextEdit{
            id:word_Edit
            anchors.fill: parent
            height: 229
            width:648
            font.pointSize:18
            wrapMode: TextEdit.Wrap
            selectByMouse: true
            color: txtColor
            onCursorRectangleChanged:
            {
				errVisible = false
				txtColor = "#FFFFFF"
			}
        }
    }


    Label{
        id:error_label
        font.pixelSize:16
        anchors.top:edit_rec.bottom
        anchors.left: edit_rec.left
        anchors.topMargin: 24
        color: "#EF5653"
        visible:errVisible
        text:"Wrong number of words or illegal words . Please try again."
    }

}
