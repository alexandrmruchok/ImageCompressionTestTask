#ifndef PBWIDGETS1_PLUGIN_H
#define PBWIDGETS1_PLUGIN_H

#include <QQmlExtensionPlugin>

class PBWidgetsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override
    {
        Q_UNUSED(uri);
    };
};

#endif // PBWIDGETS1_PLUGIN_H
