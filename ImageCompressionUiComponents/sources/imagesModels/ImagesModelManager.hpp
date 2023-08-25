#ifndef IMAGESMODELMANAGER_H
#define IMAGESMODELMANAGER_H

#include <QFileSystemWatcher>
#include <QSortFilterProxyModel>

#include "FileProcessingTask.h"
#include "ImagesModel.h"
#include "ImagesSortFilterModel.h"

namespace ImagesModels
{

class ImagesModelManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(ImagesSortFilterModel* model READ model CONSTANT)

public:
    ImagesModelManager(QObject *parent = nullptr);

    ImagesSortFilterModel *model();

    void refillModel();
    void refillModel(const QString &path);

public slots:
    void connectTask(ImageProcessing::FileProcessingTask *task, ImagesModels::ImagesModelItem *item);

private:
    void updateOrRemoveExistedModelItems(QFileInfoList& infoList);
    void appendToModel(const QFileInfo &info);
    void appendToModel(const QFileInfoList& infoList);

    ImagesModel m_model;
    ImagesSortFilterModel m_proxyModel;

    QString m_path{};

    QFileSystemWatcher m_filesWatcher;
};

} // namespace ImagesModels

#endif // IMAGESMODELMANAGER_H
