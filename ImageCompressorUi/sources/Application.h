#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>
#include <QObject>
#include <QQmlEngine>

#include "ImageCompressionManager.h"
#include "ImagesModelManager.hpp"

class Application: public QGuiApplication
{
    Q_OBJECT

    Q_PROPERTY(ImageProcessing::ImageCompressionManager *manager
               READ imageCompressionManager CONSTANT)
    Q_PROPERTY(ImagesModels::ImagesModelManager* imagesModelsManager
               READ imagesModelsManager CONSTANT)

public:
    explicit Application(int &argc, char **argv);

    ImageProcessing::ImageCompressionManager *imageCompressionManager();
    ImagesModels::ImagesModelManager *imagesModelsManager();

    void registerQmlSingletones();
    void registerQmlTypes();

private:
    ImageProcessing::ImageCompressionManager m_imagesCompressionManager;
    ImagesModels::ImagesModelManager m_imagesModelsManager;
};

#endif // APPLICATION_H
