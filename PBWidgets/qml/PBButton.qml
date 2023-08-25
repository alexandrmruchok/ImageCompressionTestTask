import QtQuick.Controls as QtQuick
import QtQuick

Rectangle {
    id: root

    property alias font: text.font

    signal clicked

    color: mouseArea.containsPress ? "black" : "grey"
    property alias text: text.text

    Text {
        id: text

        color: mouseArea.containsPress ? "grey" : "black"

        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea

        onClicked: root.clicked()

        anchors.fill: parent
    }
}
