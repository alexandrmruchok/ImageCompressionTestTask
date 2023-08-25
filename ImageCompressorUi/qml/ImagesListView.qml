import QtQuick
import BusinessLogic 1.0

ListView {
    id: root

    model: ModelManager.model

    delegate: ImagesListViewDelegate {
        id: delegate

        required property ImagesModelItem item

        width: root.width

        fileName: item.fileName
        fileType: item.fileType
        fileSize: item.sizeKb
        isProcessing: item.processing

        onClicked: {
            var task = (item.fileType === ImagesModelItem.UnpackedBmpImage)
            ? CompressionManager.runFileCompressing(delegate.item.filePath)
            : CompressionManager.runFileDecompressing(delegate.item.filePath);

            ModelManager.connectTask(task, delegate.item);
        }
    }
}
