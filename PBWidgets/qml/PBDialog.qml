import QtQuick
import QtQuick.Controls

Popup {
    id: root

    property alias text: text.text

    width: 400
    height: 200

    closePolicy: Popup.CloseOnEscape
    modal: true

    contentItem: Item {
        id: content

        Text {
            id: text

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            font.pixelSize: 16

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: button.top
            }
        }

        PBButton {
            id: button

            width: 100
            height: 30

            text: qsTr("Ok")

            font.pixelSize: 16

            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 30
            }

            onClicked: {
                root.close()
            }
        }
    }
}
