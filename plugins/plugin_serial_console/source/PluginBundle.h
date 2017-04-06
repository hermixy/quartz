#pragma once

#include <core/extension_system/AbstractPluginBundle.h>

namespace Quartz {

class QzAppContext;

namespace Plugin { namespace SerialConsole {

class PluginBundle : public AbstractPluginBundle
{

public:
    PluginBundle();

    ~PluginBundle();

    const PluginList & plugins() const override;

    const DependencyList & dependencies() const override;

    const AdapterList &adapters() const override;

    static const QString BUNDLE_ID;

    static const QString BUNDLE_NAME;

private:
    struct Data;
    std::unique_ptr< Data > m_data;
};

} } }
