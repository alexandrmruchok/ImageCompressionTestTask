#include <QDir>

#include "ImagesModelManager.hpp"

#include "ImagesModelItem.h"

namespace ImagesModels
{

static QFileInfoList getFilesInfo(const QString &path);

ImagesModelManager::ImagesModelManager(QObject *parent)
    : QObject(parent)
    , m_proxyModel{m_model}
{
    connect
    (
        &m_filesWatcher, &QFileSystemWatcher::directoryChanged,
        this, [this](const QString &path)
        {
            Q_UNUSED(path)

            this->refillModel();
        }
    );
}

ImagesSortFilterModel *ImagesModelManager::model()
{
    return &m_proxyModel;
}

void ImagesModelManager::refillModel()
{
    auto entriesInfo = getFilesInfo(m_path);

    updateOrRemoveExistedModelItems(entriesInfo);

    appendToModel(entriesInfo);
}

void ImagesModelManager::refillModel(const QString &path)
{
    if (path != m_path)
    {
        if (!m_path.isEmpty())
            m_filesWatcher.removePath(m_path);

        m_filesWatcher.addPath(path);
        m_path = path;
    }

    refillModel();
}

void ImagesModelManager::connectTask
(
    ImageProcessing::FileProcessingTask *task,
    ImagesModelItem *item
)
{
    if (!task)
        return;

    item->setProcessing(true);
    task->setParent(this);

    connect(task, &ImageProcessing::FileProcessingTask::destroyed, item, [item, this]
    {
        item->setProcessing(false);
        refillModel();
    });

    connect(task, &ImageProcessing::FileProcessingTask::completed, item, [item, this, task]
    {
        item->setProcessing(false);
        refillModel();
        task->deleteLater();
    });

    connect(task, &ImageProcessing::FileProcessingTask::error,
            item, [item, this, task](ImageCompression::ErrorCode error)
    {
        Q_UNUSED(error)

        item->setProcessing(false);
        refillModel();
        task->deleteLater();
    });
}

void ImagesModelManager::updateOrRemoveExistedModelItems(QFileInfoList &infoList)
{
    auto indexesForRemoving = QList<int>();
    for (auto i = 0; i < m_model.rowCount(); i++)
    {
        auto currentItem = m_model.item(i);
        auto indexInFileSystem = infoList.indexOf(currentItem->fileInfo());
        if (indexInFileSystem >= 0)
        {
            currentItem->updateSize();
            infoList.remove(indexInFileSystem);
        } else
            indexesForRemoving << i;
    }

    for (auto currentIndex: indexesForRemoving)
        m_model.removeRow(currentIndex);
}

void ImagesModelManager::appendToModel(const QFileInfo &info)
{
    auto item = new ImagesModelItem(info, &m_model);

    m_model.append(*item);
}

void ImagesModelManager::appendToModel(const QFileInfoList &infoList)
{
    for (const auto &currentFileInfo: infoList)
    {
        appendToModel(currentFileInfo);
    }
}

static QFileInfoList getFilesInfo(const QString &path)
{
    QDir dir{path};

    dir.setNameFilters({"*.bmp", "*.barch", "*.png"});
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::SortFlag::Name);

    return dir.entryInfoList();
}

} // namespace ImagesModels
