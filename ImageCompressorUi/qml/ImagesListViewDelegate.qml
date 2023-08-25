import QtQuick 2.12
import QtQuick.Layouts 1.0
import BusinessLogic 1.0

Item {
    id:root

    property alias fileName: fileName.text
    property int fileType: ImagesModelItem.UnpackedBmpImage
    property double fileSize
    property bool isProcessing: false

    signal clicked;

    height: 50

    Rectangle {
        id: background

        anchors.fill: parent

        border.color: "grey"
        border.width: 1

        Text {
            id: fileName

            height: parent.height
            anchors {
                left: parent.left
                leftMargin: 20
            }

            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 16
        }

        Text {
            id: isProcessingText

            height: parent.height
            anchors {
                left: fileName.right
                right: fileSize.left
                leftMargin: 20
            }

            text: !root.isProcessing ? "" :
             ("[ " +
            (root.fileType === ImagesModelItem.UnpackedBmpImage ? qsTr("Кодується") : qsTr("Розкодовується"))
            + "... ]")

            verticalAlignment: Text.AlignVCenter
            font {
                pixelSize: 14
                italic: true
            }
            color: "red"
        }

        Text {
            id: fileSize

            height: parent.height
            width: 100
            anchors {
                right: parent.right
                rightMargin: 20
            }

            text: Math.round(root.fileSize) + " KB"

            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 16
        }

        MouseArea {
            anchors.fill: parent

            onClicked: root.clicked()
        }
    }
}
