#include <memory>

#include <QHash>
#include <QDir>
#include <QLibrary>
#include <QSet>
#include <QDebug>

#include "../logger/Logging.h"
#include "../QzCoreContext.h"
#include "AbstractPlugin.h"
#include "IPluginAdapter.h"
#include "PluginManager.h"
#include "AbstractPluginBundle.h"
#include "BundleEnv.h"

namespace Quartz {

struct BundleInfo
{
    BundleInfo()
        : m_bundle{ nullptr }
        , m_library{}
    {

    }

    BundleInfo( AbstractPluginBundle *bundle,
                std::shared_ptr< QLibrary > library )
        : m_bundle{ bundle }
        , m_library{ library }
    {

    }

    bool isValid() const
    {
        return ! ( m_bundle == nullptr || m_library == nullptr );
    }

    AbstractPluginBundle *m_bundle;

    std::shared_ptr< QLibrary > m_library;
};

using PluginMap = QHash< QString, std::shared_ptr< AbstractPlugin >>;
using AdapterMap = QHash< QString, std::shared_ptr< IPluginAdapter >>;
using BundleInfoMap = QHash< QString, BundleInfo >;

typedef PluginBundleWrapper ( *PluginFunc )( BundleInputWrapper * );
typedef void ( *BundleDestroyerFunc )();

static const char * PLUGIN_GET_FUNC_NAME = "getBundleWrapper";
static const char * BUNDLE_DESTROY_FUNC_NAME = "destroy";


class PluginManager::Impl
{
public:
    bool load( const QDir &dir );

    bool destroy();

    std::size_t loadBundles( const QDir &pluginRoot,
                             QZ_OUT BundleInfoMap &bundleInfoOut );

    BundleInfo getBundle( const QString &pluginRoot,
                          const QString &pluginFilePath );
    bool processBundles( const BundleInfo &bundleInfo,
                         const BundleInfoMap &loadedBundles,
                         QZ_OUT QSet< QString > &processedBundles );

    bool initBundle( AbstractPluginBundle *bundle );

    void registerPluginAdapter( std::shared_ptr< IPluginAdapter > adapter );

    PluginMap m_plugins;

    AdapterMap m_adapters;

    BundleInfoMap m_bundles;

    bool m_active;
};

bool PluginManager::Impl::load( const QDir &dir )
{
    bool result = true;
    auto dirList = dir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot,
                                      QDir::SortFlag::NoSort );
    BundleInfoMap loadedBundles;
    foreach(const QFileInfo &info, dirList) {
        if( info.isReadable() ) {
            QDir pluginDir{ info.absoluteFilePath() };
            auto numLoaded = loadBundles( pluginDir, loadedBundles );
            QZ_DEBUG( "Qz:Core:Ext" )
                    << "Loaded " << numLoaded << " plugins from "
                    << info.absoluteFilePath();
        }
        else {
            QZ_WARN( "Qz:Core:Ext" ) << "Could not read plugin directory at "
                                     << info.absoluteFilePath();
            result = false;
        }
    }
    QSet< QString > processed;
    for( auto it = loadedBundles.begin(); it != loadedBundles.end(); ++ it ) {
        const auto &binfo = it.value();
        if( ! processed.contains( binfo.m_bundle->bundleId() )) {
            result = processBundles( binfo,
                                     loadedBundles,
                                     processed ) && result;
        }
    }
    //unload failed bundles
    for( auto it = loadedBundles.begin();
         it != loadedBundles.end();
         ++ it ) {
        auto &binfo = it.value();
        if( ! m_bundles.contains( binfo.m_bundle->bundleId() )) {
            auto lib = binfo.m_library;
            if( lib != nullptr ) {
                auto destroyFunc = reinterpret_cast< BundleDestroyerFunc >(
                            lib->resolve( BUNDLE_DESTROY_FUNC_NAME ));
                if( destroyFunc != nullptr ) {
                    destroyFunc();
                }
                else {
                    QZ_DEBUG( "Qz:Core:Ext" )
                            << "Could not find symbol for destroy function in "
                               " library: " << lib->fileName();
                }
                result = result && lib->unload();
            }
            QZ_DEBUG( "Qz:Core:Ext" )
                    << "Unloaded library for bundle with id " <<
                       binfo.m_bundle->bundleId()
                    << " which failed to initialize";
        }
        else {
            initBundle( binfo.m_bundle );
        }
    }
    return result;
}

std::size_t PluginManager::Impl::loadBundles(
        const QDir &bundleRoot,
        QZ_OUT BundleInfoMap &bundleInfoOut )
{
    QStringList extensions;
#ifdef Q_OS_LINUX
    extensions << "*.so";
#elif defined Q_OS_WIN
    extensions << "*.dll";
#else
    extensions << "*.dylib" << "*.so";
#endif

#ifdef QT_DEBUG
    QDir bundleDir{ bundleRoot.absoluteFilePath( "debug" )};
#else
//    QDir pluginDir{ dirInfo.absoluteFilePath() };
        QDir pluginDir = pluginRoot;
#endif
    std::size_t numLoaded = 0;
    QZ_DEBUG( "Qz:Core:Ext" )
            << "Searching for plugins at " << bundleDir.absolutePath();
    auto fileList = bundleDir.entryInfoList( extensions,
                                             QDir::Files,
                                             QDir::NoSort );
    foreach( const auto &info, fileList ) {
        if( info.fileName().startsWith( "plugin_" )
                || info.fileName().startsWith( "libplugin_" )) {

            auto binfo = getBundle( bundleRoot.absolutePath(),
                                    info.absoluteFilePath() );
            if( binfo.isValid() ) {
                bundleInfoOut.insert( binfo.m_bundle->bundleId(), binfo );
                ++ numLoaded;
            }
        }
    }
    return numLoaded;
}

BundleInfo PluginManager::Impl::getBundle( const QString &bundleRoot,
                                           const QString &bundleFilePath )
{
    auto lib = std::make_shared< QLibrary >( bundleFilePath );
    lib->load();
    BundleInfo binfo{};
    if( lib->isLoaded() ) {
        auto func = reinterpret_cast< PluginFunc >(
                    lib->resolve( PLUGIN_GET_FUNC_NAME ));
        if( func != nullptr ) {
            auto bundleEnv = std::unique_ptr< BundleEnv >{
                new BundleEnv{ bundleRoot, lib->fileName(), "0.1.0000" }};
            BundleInputWrapper input{ std::move( bundleEnv ),
                                      QzCoreContext::get() };
            auto bundle = func( &input ).bundle;
            if( bundle != nullptr ) {
                binfo = BundleInfo{ bundle, lib };
            }
            else {
                QZ_WARN( "Qz:Core:Ext" )
                        << "Library at " << bundleFilePath << " does not "
                           "contain any bundle, unloading...";
                lib->unload();
            }
        }
        else {
            lib->unload();
            QZ_ERROR( "Qz:Core:Ext" )
                    << "Plugin library at " << bundleFilePath  << " does "
                    << "not export entry point";
        }
    }
    else {
        QZ_ERROR( "Qz:Core:Ext" )
                << "Failed to load plugin library at " << bundleFilePath;
    }
    return binfo;
}

bool PluginManager::Impl::processBundles(
        const BundleInfo &bundleInfo,
        const BundleInfoMap &loadedBundles,
        QZ_OUT QSet< QString > &processedBundles )
{
    if( processedBundles.contains( bundleInfo.m_bundle->bundleId() )) {
        return true;
    }
    bool result = true;
    const auto &hardDeps = bundleInfo.m_bundle->dependencies();
    for( int i = 0; i < hardDeps.size(); ++ i ) {
        const auto & depEntry = hardDeps.at( i );
        auto depInfo = m_bundles.value( depEntry.first );
        if( depInfo.isValid() ) {
            result = processBundles( depInfo,
                                     loadedBundles,
                                     processedBundles );
        }
        else {
            result = false;
        }
        if( ! result && depEntry.second == DependencyType::Optional ) {
            QZ_DEBUG( "Qz:Core:Ext" )
                    << "Optional dependency " << depEntry.first << " for "
                    << bundleInfo.m_bundle->bundleId() << " not present, "
                       "hence ignored.";
            continue;
        }
        else if( ! result ) {
            QZ_ERROR( "Qz:Core:Ext" )
                    << "Could not initialize bundle with id "
                    << bundleInfo.m_bundle->bundleId()
                    << "; could not find required dependncy with id "
                    << depEntry.first;
            break;
        }

    }

    if( result ) {
        m_bundles.insert( bundleInfo.m_bundle->bundleId(), bundleInfo );
        QZ_DEBUG( "Qz:Core:Ext" )
                << "Successfuly processed bundle with id "
                << bundleInfo.m_bundle->bundleId();
    }
    processedBundles.insert( bundleInfo.m_bundle->bundleId() );

    return result;
}

bool PluginManager::Impl::initBundle( AbstractPluginBundle *bundle )
{
    const auto &adapters = bundle->adapters();
    for( int i = 0; i < adapters.size(); ++ i ) {
        const auto adapter = adapters.at( i );
        if( ! m_adapters.contains( adapter->pluginType() )) {
            m_adapters.insert( adapter->pluginType(), adapter );
        }
    }
    auto result = true;
    const auto plugins = bundle->plugins();
    for( int i = 0; i < plugins.size(); ++ i ) {
        auto plugin = plugins.at( i );

            auto adapter = m_adapters.value( plugin->pluginType() );
            if( adapter != nullptr ) {
                result = adapter->handlePlugin( plugin.get() );
            }
            else {
                QZ_ERROR( "Qz:Core:Ext" )
                        << "Could not find adapter for plugin type "
                        << plugin->pluginType() << ", required by plugin "
                        << plugin->pluginId() << ". Ignoring...";
                result = false;
            }
    }
    return result;
}

void PluginManager::Impl::registerPluginAdapter(
        std::shared_ptr< IPluginAdapter > adapter )
{
    if( adapter ) {
        m_adapters[ adapter->pluginType() ] = adapter;
    }
}

bool PluginManager::Impl::destroy()
{
    if( ! m_active ) {
        return false;
    }
    bool result = true;
    for( auto it = m_adapters.begin();
         it != m_adapters.end();
         ++ it ) {
        auto &handler = it.value();
        result = result && handler->finalizePlugins();
    }
    foreach( auto &bundle, m_bundles ) {
        auto lib = bundle.m_library;
        if( lib != nullptr ) {
            auto destroyFunc = reinterpret_cast< BundleDestroyerFunc >(
                        lib->resolve( BUNDLE_DESTROY_FUNC_NAME ));
            if( destroyFunc != nullptr ) {
                destroyFunc();
            }
            else {
                QZ_DEBUG( "Qz:Core:Ext" )
                        << "Could not find symbol for destroy function in "
                           " library: " << lib->fileName();
            }
            result = result && lib->unload();
        }
    }
    m_active = false;
    return result;
}

//========================== PluginManager ====================================
PluginManager::PluginManager()
//    : m_data( std::make_unique< PluginManager::Data >() )
    : m_impl( std::unique_ptr< Impl >( new Impl() ))
{

}


PluginManager::~PluginManager()
{
    destroy();
}

AbstractPlugin * PluginManager::plugin( const QString &id ) const
{
    auto &plugin = m_impl->m_plugins.value( id );
    return plugin.get();
}

IPluginAdapter * PluginManager::adapter( const QString &id ) const
{
    return m_impl->m_adapters.value(
                id, std::shared_ptr< IPluginAdapter >{} ).get();
}

void PluginManager::registerPluginAdapter(
        std::shared_ptr< IPluginAdapter > adapter )
{
    m_impl->registerPluginAdapter( adapter );
}

void PluginManager::registerPluginAdapter( IPluginAdapter *adapter )
{
    std::shared_ptr< IPluginAdapter > adptr{ adapter,
                                             []( IPluginAdapter *){ }};
    m_impl->registerPluginAdapter(  adptr );
}

bool PluginManager::loadFrom( const QString &location )
{
    bool result = false;
    QFileInfo info{ location };
    if( info.isDir() && info.isReadable() ) {
        result = m_impl->load( QDir{ location });
    }
    else {
        QZ_ERROR( "Qz:Core:Ext" )
                << "Invalid plugin root directory " << location << " given";
    }
    return result;
}

bool PluginManager::destroy()
{
    return m_impl->destroy();
}

}
