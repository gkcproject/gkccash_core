import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Rectangle{
    id:root
    width: 92
    height: 40
    color: "#2E3962"

    property string index_str: ""
    property string word_str: ""
    property bool indexVisible: true
    property bool selected: false
    property string word_color: "#FFFFFF"

    signal selectedReceived(string word)

    function init()
    {
        root.opacity = 1
        word_label.opacity = 1
        word_color = "#FFFFFF"
        selected = true
    }

    MouseArea{
        anchors.fill: parent

        onClicked: {
            if(selected)
            {
                root.opacity = 0.2
                word_label.opacity = 0.2
                selectedReceived(word_str)
                selected = false
            }
        }
    }

    Label{
        id:index_label
        font.pixelSize:10
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 4
        anchors.leftMargin: 4
        color: "#FDA205"
        text:index_str
        visible: indexVisible
    }

    Label{
        id:word_label
        font.pixelSize:16
        anchors.horizontalCenter:parent.horizontalCenter
        anchors.verticalCenter:parent.verticalCenter
        color: word_color
        text:word_str
    }
}
