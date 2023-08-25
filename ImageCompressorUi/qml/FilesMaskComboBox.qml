import QtQml.Models
import QtQuick

import PBWidgets 1.0

Item {
    id: root

    property alias maskText: comboBox.currentText

    height: comboBox.height
    width: comboBox.width + tittleText.width

    Text {
        id: tittleText

        anchors {
            verticalCenter: comboBox.verticalCenter
            right: comboBox.left
            rightMargin: 10
        }

        text: qsTr("Розширення файлу: ");

        font.pixelSize: 16
        verticalAlignment: Text.AlignVCenter
    }

    PBComboBox {
        id: comboBox

        anchors.right: parent.right

        model: ListModel {
            id: model
            ListElement { text: "*" }
            ListElement { text: "*.bmp" }
            ListElement { text: "*.png" }
            ListElement { text: "*.barch" }
        }
    }
}
