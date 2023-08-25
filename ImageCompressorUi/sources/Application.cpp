#include <QQmlEngine>

#include "Application.h"

#include "ImagesModelItem.h"

Application::Application(int &argc, char **argv)
    : QGuiApplication{argc, argv}
{
    QString path;
    if (argc > 1)
        path = argv[1];
    else
        path = QCoreApplication::applicationDirPath();

    m_imagesModelsManager.refillModel(path);
}

ImageProcessing::ImageCompressionManager *Application::imageCompressionManager()
{
    return &m_imagesCompressionManager;
}

ImagesModels::ImagesModelManager *Application::imagesModelsManager()
{
    return &m_imagesModelsManager;
}

void Application::registerQmlSingletones()
{
    qmlRegisterSingletonInstance("BusinessLogic", 1, 0, "ModelManager", &m_imagesModelsManager);
    qmlRegisterSingletonInstance
    (
        "BusinessLogic", 1, 0,
        "CompressionManager",
        &m_imagesCompressionManager
    );
}

void Application::registerQmlTypes()
{
    qmlRegisterUncreatableType<ImagesModels::ImagesModelItem>(
                                        "BusinessLogic", 1, 0,
                                        "ImagesModelItem",
                                        "ImagesModelItem object couldn't be created in QML");
    qmlRegisterUncreatableType<ImagesModels::ImagesModel>("BusinessLogic", 1, 0, "ImagesModel",
                                        "ImagesModel object couldn't be created in QML");
    qmlRegisterUncreatableType<ImagesModels::ImagesSortFilterModel>(
                                        "BusinessLogic", 1, 0,
                                        "ImagesSortFilterModel",
                                        "ImagesSortFilterModel object couldn't be created in QML");
}
