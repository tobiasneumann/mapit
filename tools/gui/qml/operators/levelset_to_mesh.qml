/*******************************************************************************
 *
 * Copyright      2017 Daniel Bulla	<d.bulla@fh-aachen.de>
 *
******************************************************************************/

/*  This file is part of mapit.
 *
 *  Mapit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Mapit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mapit.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import ".."
import "../components"

ColumnLayout {
    id: root
    //// in ////
    property bool editable
    property string currentEntityPath

    function fromParameters(params) {
        entityChooser.currentEntityPath = params.input
    }

    //// out ////
    property bool valid: entityChooser.valid && detailTextfield.valid
    property var parameters: {
        "input" : entityChooser.currentEntityPath,
        "output" : entityChooser.currentEntityPath + "-ply", //TODO: because input != output at the moment (if entitytypes differ and both streams are open at the same time)
        "detail" : parseFloat(detailTextfield.text)
    }

    //// UI ////
    StyledLabel {
        Layout.fillWidth: true
        text: "Detail:"
    }
    StyledTextField {
        Layout.fillWidth: true
        id: detailTextfield
        text:"0.1"
        //property real num: text.toFixed(8)
        property bool valid: text <= validator.top && text >= validator.bottom
        validator: DoubleValidator {
            bottom: 0.0
            top: 1.0
        }
    }
    HelperTarget {
        id: entityChooser
        currentEntityPath: root.currentEntityPath
    }
}
