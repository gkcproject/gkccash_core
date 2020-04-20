import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Styles 1.4 as Controls_Style_1
import QtQuick.Controls 1.4 as Controls_1
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
Controls_1.TextField{
    id:root

    readOnly: true
    horizontalAlignment:TextInput.AlignLeft
    textColor: "#FFFFFF"
    selectByMouse: true
    property bool autoWidth: true
    style: Controls_Style_1.TextFieldStyle{
        background: Rectangle{
            color: "transparent"
            border.color: "transparent"
            border.width: 0

        }

        selectionColor: "#7BB6C3"
        selectedTextColor: "#333333"
        padding.left: 0


    }

    onTextChanged:
    {
        cursorPosition=0
        if(autoWidth)
            width = font.pointSize*length




    }
}
