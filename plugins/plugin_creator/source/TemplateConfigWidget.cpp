
#include <QSet>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>

#include <common/templating/Template.h>
#include <common/templating/TemplateInstance.h>
#include <common/model_view/ArrayModel.h>
#include <common/generic_config/ui/GenConfigDelegate.h>
#include <common/generic_config/model/Config.h>
#include <common/generic_config/model/Param.h>
#include <common/widgets/QzTreeView.h>

#include "TemplateConfigWidget.h"

namespace Quartz { namespace Plugin { namespace Creator {

const QVector< QString > TI_HEADERS{
    QObject::tr( "Instance" )
};

struct TemplateConfigWidget::Data
{
    explicit Data( TemplateConfigWidget *parent )
        : m_filter{ new QLineEdit{ parent }}
        , m_view{ new QzTreeView{ parent }}
        , m_emptyConfig{ "none", "None" }
        , m_configView{ new QzTreeView{ parent }}
//        , m_configWidget{ new GenConfigWidget{ &m_emptyConfig, parent }}
        , m_tmodel{ new ArrayModel{ 2, false, true, TI_HEADERS, parent }}
        , m_configModel{ new ConfigModel{ parent }}
    {

    }

    QLineEdit *m_filter;

    QzTreeView *m_view;

    Config m_emptyConfig;

    QzTreeView *m_configView;

    ArrayModel *m_tmodel;

    ConfigModel *m_configModel;

    QHash< QString, std::shared_ptr< TemplateInstance >> m_instances;
};

TemplateConfigWidget::TemplateConfigWidget(  QWidget *parent )
    : QWidget{ parent }
    , m_data{ new Data{ this }}
{
    auto mainLayout = new QHBoxLayout{};
    auto leftLayout = new QVBoxLayout{};
    leftLayout->addWidget( m_data->m_filter );
    leftLayout->addWidget( m_data->m_view );
    mainLayout->addLayout( leftLayout );
    mainLayout->addWidget( m_data->m_configView );

    m_data->m_view->setModel( m_data->m_tmodel );
    m_data->m_view->setRootIsDecorated( false );
    m_data->m_configView->setModel( m_data->m_configModel );
    m_data->m_configView->setRootIsDecorated( false );
    m_data->m_configView->setItemDelegate( new GenConfigDelegate{ this });

    mainLayout->setContentsMargins( QMargins{} );
    leftLayout->setContentsMargins( QMargins{} );

    this->setLayout( mainLayout );

    connect( m_data->m_view->selectionModel(),
             &QItemSelectionModel::currentChanged,
             [ this ]( const QModelIndex &current, const QModelIndex &){
        auto ti = static_cast< TemplateInstance *>(
                    current.internalPointer() );
        if( ti != nullptr ) {
            if( ti->instanceOf()->config() != nullptr ) {
                m_data->m_configModel->setConfig(
                            ti->instanceOf()->config() );
            } else {
                m_data->m_configModel->setConfig( &m_data->m_emptyConfig );
            }
        }
    });
}

TemplateConfigWidget::~TemplateConfigWidget()
{

}

void TemplateConfigWidget::createInstanceOf( Template *tmpl )
{
    auto name = tmpl->name();
    auto index = 1;
    //If there is a name clash, we suffix numbers
    do {
        if( ! m_data->m_instances.contains( name )) {
            break;
        }
        name = tmpl->name() + "_" + QString::number( index );
    } while( index <= 1000 ); //1000 should be enough
    auto inst = std::make_shared< TemplateInstance >( name, tmpl );
    m_data->m_instances.insert( name, inst );
    m_data->m_tmodel->addRoot( inst.get() );
}

int TemplateConfigWidget::numInstances() const
{
    return m_data->m_tmodel->rootCount();
}

TemplateInstance * TemplateConfigWidget::instanceAt( int index )
{
    TemplateInstance *tinst = nullptr;
    if( index < m_data->m_tmodel->rootCount() ) {
        tinst = static_cast< TemplateInstance * >(
                    m_data->m_tmodel->rootAt( index ));
    }
    return tinst;
}

void TemplateConfigWidget::clear()
{
    m_data->m_tmodel->clear();
    m_data->m_instances.clear();
    m_data->m_configModel->setConfig( &m_data->m_emptyConfig );
}

//////////////////////// Config Model ///////////////////////////

const QVector< QString > CONFIG_HEADERS{
    QObject::tr( "Name" ),
    QObject::tr( "Value" )
};

ConfigModel::ConfigModel( QObject *parent )
    : AbstractTreeModel{ 2, false, true, CONFIG_HEADERS, parent }
    , m_config{ nullptr }
{

}

ConfigModel::~ConfigModel()
{

}

void ConfigModel::setConfig( Config *config )
{
    beginResetModel();
    m_config = config;
    endResetModel();
}

TreeNode * ConfigModel::rootAt( int index ) const
{
    if( m_config != nullptr && index < m_config->numChildParams() ) {
        return m_config->childParamAt( index );
    }
    return nullptr;
}

int ConfigModel::rootCount() const
{
    if( m_config != nullptr ) {
        return m_config->numChildParams();
    }
    return 0;
}

} } }
