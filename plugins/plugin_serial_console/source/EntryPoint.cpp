
#include <core/ext/PluginEnv.h>
#include <core/ext/Plugin.h>

#include <base/QzAppContext.h>

#include <plugin_base/PluginContext.h>

#include "Plugin.h"

void initResource() {
    Q_INIT_RESOURCE( serial_console );
}

extern "C" {

Q_DECL_EXPORT PluginWrapper getPluginWrapper(
        PluginInputWrapper *input )
{
    initResource();
    auto plugin =
        std::unique_ptr< Quartz::Ext::SerialConsole::Plugin >{
            new Quartz::Ext::SerialConsole::Plugin{} };
    auto pluginPtr = plugin.get();
    Quartz::Ext::PluginContext::init(
                std::move( plugin ),
                std::move( input->env ),
                dynamic_cast< Quartz::QzAppContext *>( input->appContext ));
    return PluginWrapper{ pluginPtr };
}

Q_DECL_EXPORT void destroy()
{
    Quartz::Ext::PluginContext::destroy();
}

}
