import QtQuick 2.10
import QtQuick.Templates 2.3 as T
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

T.CheckBox {
    id: control
	property alias textColor: text_label.color
    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            contentItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? background.implicitHeight : 0,
                             Math.max(contentItem.implicitHeight,
                                      indicator ? indicator.implicitHeight : 0) + topPadding + bottomPadding)
    baselineOffset: contentItem.y + contentItem.baselineOffset
    font.weight: Font.Medium
    spacing: 5
    padding: 5
    topPadding: padding + 7
    bottomPadding: padding + 7

    indicator: CommonCheckIndicator {
        x: 0
        y: control.topPadding + (control.availableHeight - height) / 2
        control: control
        width:12
        height:12
        border.color:control.enabled?"#6B78A8":"#F0F0F0"
        border.width: control.checked ? width / 2 : 1


        radius:3
        Ripple {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 16; height: 16

            z: -1
            anchor: control
            pressed: control.pressed
            active: control.down || control.visualFocus || control.hovered
            color: control.checked ? control.Material.highlightedRippleColor : control.Material.rippleColor
        }
    }

    contentItem: Text {
		id:text_label
        leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0

        text: control.text
        font: control.font
        color: control.enabled ? "#FFFFFF":"#F0F0F0"
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}
