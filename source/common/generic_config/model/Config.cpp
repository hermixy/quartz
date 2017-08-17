#include <QVector>
#include <QHash>
#include <QString>

#include "Config.h"
#include "Param.h"
#include "Group.h"


namespace Quartz {

struct Config::Data
{
    Data( const QString &id,
          const QString &name )
        : m_id{ id }
        , m_name{ name }
    {

    }
    QString m_id;

    QString m_name;

    QVector< std::shared_ptr< Param >> m_params;

    QVector< std::shared_ptr< Group >> m_groups;

    QHash< QString, Param *> m_allParams;
};

Config::Config( const QString &id, const QString &name )
    : m_data{ new Data{ id, name }}
{

}

Config::~Config()
{

}

const QString & Config::id() const
{
    return m_data->m_id;
}

const QString & Config::name() const
{
    return m_data->m_name;
}

int Config::numChildParams() const
{
    return m_data->m_params.size();
}

Param * Config::childParamAt( int index ) const
{
    if( index < m_data->m_params.size() ) {
        return m_data->m_params.at( index ).get();
    }
    return nullptr;
}

void Config::addChildParameter( std::shared_ptr< Param > param )
{
    m_data->m_params.push_back( param );
}

int Config::numGroups() const
{
    return m_data->m_groups.size();
}

Group *Config::groupAt( int index ) const
{
    Group *group = nullptr;
    if( index < m_data->m_groups.size() ) {
        group = m_data->m_groups.at( index ).get();
    }
    return group;
}

void Config::addGroup( std::shared_ptr< Group > group )
{
    if( group != nullptr ) {
        m_data->m_groups.append( group );
    }
}

const Param * Config::param( const QString &id ) const
{
    Param *param = m_data->m_allParams.value( id, nullptr );
    return param;

}

bool Config::registerParam( Param *param )
{
    bool result = false;
    if( ! m_data->m_allParams.contains( param->id() )) {
        m_data->m_allParams.insert( param->id(), param );
        result = true;
    }
    return result;

}

}
