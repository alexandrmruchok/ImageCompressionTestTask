import QtQuick
import BusinessLogic 1.0
import "."
import QtQuick.Controls
import PBWidgets 1.0

Window {
    id:root

    visible: true
    title: qsTr("Image processing")

    FilesMaskComboBox {
        id: filterComboBox

        onMaskTextChanged: ModelManager.model.setFilterMask(maskText);

        anchors
        {
            top: parent.top
            right: parent.right
            margins: 10
        }
    }

    ImagesListView {
        anchors {
            top: filterComboBox.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
            margins: 10
        }
    }

    PBDialog {
        id: errorDialog

        anchors.centerIn: parent

        text: qsTr("Невідомий тип файлу!");

        Connections {
            target: CompressionManager

            function onUnknownFileFormatError() {
                errorDialog.open();
            }
        }
    }

    Component.onCompleted: {
        root.width = 700
        root.height = 700
    }
}
