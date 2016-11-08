#include <QHash>
#include <QStackedLayout>

#include <core/logger/Logger.h>

#include "ContentWidget.h"
#include "AbstractContentProvider.h"
#include "ContentManager.h"

namespace Quartz {

struct ContentManager::Data
{
    QHash< QString, ContentWidget *> m_widgets;

    QVector< ContentWidget *> m_fromPlugins;

    QStackedLayout *m_layout;
};

const QString ContentManager::ADAPTER_NAME{ "Content Manager" };

ContentManager::ContentManager( QWidget *parent )
    : QWidget( parent )
//    , m_data( std::make_unique< Data >() )
    , m_data( new Data{} )
{

}

ContentManager::~ContentManager()
{

}

bool ContentManager::addContent( ContentWidget *content )
{
    bool result = false;
    if( content != nullptr ) {
        m_data->m_widgets.insert( content->id(), content );
        auto index = m_data->m_layout->addWidget( content );
        m_data->m_layout->setCurrentIndex( index );
        result = true;
    }
    return result;
}

bool ContentManager::removeContent( const QString &contentId )
{
    bool result = false;
    auto content = m_data->m_widgets.value( contentId );
    if( content != nullptr ) {
        if( m_data->m_layout->currentWidget() == content
                && m_data->m_layout->count() > 0 ) {
                m_data->m_layout->setCurrentIndex( 0 );
        }
        m_data->m_widgets.remove( contentId );
        m_data->m_layout->removeWidget( content );
        result = true;
    }
    return result;
}

ContentWidget * ContentManager::content( const QString &contentId )
{
    ContentWidget * widget = m_data->m_widgets.value( contentId, nullptr );
    return widget;
}

QVector< ContentWidget *> ContentManager::contentsOfKind( const QString &kind )
{
    QVector< ContentWidget *> content;
    auto it = m_data->m_widgets.begin();
    for( ; it != m_data->m_widgets.end(); ++ it ) {
        if( it.value()->kind() == kind ) {
            content.push_back( it.value() );
        }
    }
    return content;
}

int ContentManager::removeKind( const QString &kind )
{
    int removed = 0;
    auto it = m_data->m_widgets.begin();
    for( ; it != m_data->m_widgets.end(); ++ it ) {
        if( it.value()->kind() == kind ) {
            m_data->m_widgets.erase( it );
            ++ removed;
        }
    }
    return removed;
}

void ContentManager::selectContent(const QString &contentId)
{
    auto widget = m_data->m_widgets.value( contentId );
    if( widget != nullptr ) {
        m_data->m_layout->setCurrentWidget( widget );
    }
}

const QString & ContentManager::pluginType() const
{
    return AbstractContentProvider::PLUGIN_TYPE;
}

const QString & ContentManager::pluginAdapterName() const
{
    return ADAPTER_NAME;
}

bool ContentManager::handlePlugin( IPlugin *plugin )
{
    bool result = false;
    auto provider = dynamic_cast< AbstractContentProvider *>( plugin );
    if( provider != nullptr ) {
        auto content = provider->widget();
        if( addContent( content )) {
            m_data->m_fromPlugins.push_back( content );
            result = true;
        }
    }
    return result;
}

bool ContentManager::finalizePlugins()
{
    auto result = true;
    for( int i = 0; i < m_data->m_fromPlugins.size(); ++ i ) {
        auto content = m_data->m_fromPlugins.at( i );
        result = removeContent( content->id() ) && result;
    }
    return result;
}

void ContentManager::setupLayout()
{
    m_data->m_layout = new QStackedLayout{};
    this->setLayout( m_data->m_layout );
}


}