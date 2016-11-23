
#include "AbstractPlugin.h"

namespace Quartz {

struct AbstractPlugin::Data
{
    Data( const QString &pluginId,
          const QString &pluginName,
          const QString &pluginType,
          const QStringList &dependencies )
        : m_pluginId( pluginId )
        , m_pluginName( pluginName )
        , m_pluginType( pluginType )
        , m_dependencies( dependencies )
    {

    }

    const QString &m_pluginId;

    const QString &m_pluginName;

    const QString &m_pluginType;

    const QStringList &m_dependencies;
};


AbstractPlugin::AbstractPlugin( const QString &pluginId,
                                const QString &pluginName,
                                const QString &pluginType,
                                const QStringList &dependencies )
    : m_data( new Data{ pluginId, pluginName, pluginType, dependencies })
{

}

AbstractPlugin::~AbstractPlugin()
{

}

const QString &AbstractPlugin::pluginId() const
{
    return m_data->m_pluginId;
}

const QString &AbstractPlugin::pluginName() const
{
    return m_data->m_pluginName;
}

const QString &AbstractPlugin::pluginType() const
{
    return m_data->m_pluginType;
}

const QStringList &AbstractPlugin::dependencies() const
{
    return m_data->m_dependencies;
}


}