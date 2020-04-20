import QtQuick 2.10
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

/*
   An effect for standard Material Design elevation shadows. Useful for using as \c layer.effect.
 */
Item {
    id: effect

    /*
       The source the effect is applied to.
     */
    property var source

    /*
       The elevation of the \l source Item.
     */
    property int elevation: 0

    /*
       Set to \c true if the \l source Item is the same width as its parent and the shadow
       should be full width instead of rounding around the corner of the Item.

       \sa fullHeight
     */
    property bool fullWidth: false

    /*
       Set to \c true if the \l source Item is the same height as its parent and the shadow
       should be full height instead of rounding around the corner of the Item.

       \sa fullWidth
     */
    property bool fullHeight: false

    /*
       \internal

       The actual source Item the effect is applied to.
     */
    readonly property Item sourceItem: source.sourceItem

    /*
     * The following shadow values are taken from Angular Material
     *
     * The MIT License (MIT)
     *
     * Copyright (c) 2014-2016 Google, Inc. http://angularjs.org
     *
     * Permission is hereby granted, free of charge, to any person obtaining a copy
     * of this software and associated documentation files (the "Software"), to deal
     * in the Software without restriction, including without limitation the rights
     * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
     * copies of the Software, and to permit persons to whom the Software is
     * furnished to do so, subject to the following conditions:
     *
     * The above copyright notice and this permission notice shall be included in all
     * copies or substantial portions of the Software.
     *
     * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
     * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
     * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
     * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
     * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
     * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
     * SOFTWARE.
     */
    /*
       \internal

       The shadows to use for each possible elevation. There are three shadows that when combined
       make up the elevation.
     */
    readonly property var _shadows: [
        [{offset: 0, blur: 0, spread: 0},
         {offset: 0, blur: 0, spread: 0},
         {offset: 0, blur: 0, spread: 0}],

        [{offset: 1, blur: 3, spread: 0},
         {offset: 1, blur: 1, spread: 0},
         {offset: 2, blur: 1, spread: -1}],

        [{offset: 1, blur: 5, spread: 0},
         {offset: 2, blur: 2, spread: 0},
         {offset: 3, blur: 1, spread: -2}],

        [{offset: 1, blur: 8, spread: 0},
         {offset: 3, blur: 4, spread: 0},
         {offset: 3, blur: 3, spread: -2}],

        [{offset: 2, blur: 4, spread: -1},
         {offset: 4, blur: 5, spread: 0},
         {offset: 1, blur: 10, spread: 0}],

        [{offset: 3, blur: 5, spread: -1},
         {offset: 5, blur: 8, spread: 0},
         {offset: 1, blur: 14, spread: 0}],

        [{offset: 3, blur: 5, spread: -1},
         {offset: 6, blur: 10, spread: 0},
         {offset: 1, blur: 18, spread: 0}],

        [{offset: 4, blur: 5, spread: -2},
         {offset: 7, blur: 10, spread: 1},
         {offset: 2, blur: 16, spread: 1}],

        [{offset: 5, blur: 5, spread: -3},
         {offset: 8, blur: 10, spread: 1},
         {offset: 3, blur: 14, spread: 2}],

        [{offset: 5, blur: 6, spread: -3},
         {offset: 9, blur: 12, spread: 1},
         {offset: 3, blur: 16, spread: 2}],

        [{offset: 6, blur: 6, spread: -3},
         {offset: 10, blur: 14, spread: 1},
         {offset: 4, blur: 18, spread: 3}]
    ]

    /*
       \internal

       The current shadow based on the elevation.
     */
    readonly property var _shadow: _shadows[Math.max(0, Math.min(elevation, _shadows.length - 1))]

    // Nest the shadows and source view in two items rendered as a layer
    // so the shadow is not clipped by the bounds of the source view
    Item {
        property int margin: -100

        x: margin
        y: margin
        width: parent.width - 2 * margin
        height: parent.height - 2 * margin

        // By rendering as a layer, the shadow will never show through the source item,
        // even when the source item's opacity is less than 1
        layer.enabled: true

        // The box shadows automatically pick up the size of the source Item and not
        // the size of the parent, so we don't need to worry about the extra padding
        // in the parent Item
        BoxShadow {
            offsetY: _shadow[0].offset
            blurRadius: _shadow[0].blur
            spreadRadius: _shadow[0].spread
            color: Qt.rgba(0,0,0, 0.2)

            fullWidth: effect.fullWidth
            fullHeight: effect.fullHeight
            source: effect.sourceItem
        }

        BoxShadow {
            offsetY: _shadow[1].offset
            blurRadius: _shadow[1].blur
            spreadRadius: _shadow[1].spread
            color: Qt.rgba(0,0,0, 0.14)

            fullWidth: effect.fullWidth
            fullHeight: effect.fullHeight
            source: effect.sourceItem
        }

        BoxShadow {
            offsetY: _shadow[2].offset
            blurRadius: _shadow[2].blur
            spreadRadius: _shadow[2].spread
            color: Qt.rgba(0,0,0, 0.12)

            fullWidth: effect.fullWidth
            fullHeight: effect.fullHeight
            source: effect.sourceItem
        }

        ShaderEffect {
            property alias source: effect.source

            x: (parent.width - width)/2
            y: (parent.height - height)/2
            width: sourceItem.width
            height: sourceItem.height
        }
    }
}
