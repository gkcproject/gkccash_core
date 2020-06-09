import QtQuick 2.0
import QtQuick.Controls 1.2

TextField
{
    id:root
    property string dateValue
    inputMethodHints:Qt.ImhDate
    inputMask: "0000-00-00"
    implicitHeight: 23
    implicitWidth: 150
    signal dateChanged()
    signal calendarVisibleChanged(bool visible)
	signal btnDown()

    function close_calender()
    {
        calendar.visible = false
    }

    Calendar{
        id: calendar
        anchors.topMargin: 0
        anchors.top: parent.bottom
        visible: false
        activeFocusOnTab: true
        onReleased: {
            text = date;
            text = text.substr(0, 10);
            dateValue = text;
            visible = false;
        }

        onVisibleChanged:
        {
            calendarVisibleChanged(visible)
        }

    }

    Button{
        id: downBtn
        width: 22
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        iconSource: "qrc:/images/icons/drop.png"
        onClicked: {
			calendar.visible = !calendar.visible
			calendar.selectedDate = root.text
			btnDown()
		}
    }
	
    onDateValueChanged: {
        text = dateValue;
        calendar.selectedDate = dateValue;
        dateChanged()
    }
}
