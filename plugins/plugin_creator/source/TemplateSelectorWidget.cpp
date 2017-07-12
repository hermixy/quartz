#include <QTreeView>
#include <QVBoxLayout>

#include "TemplateManager.h"
#include "TemplateSelectorWidget.h"

namespace Quartz { namespace Plugin { namespace Creator {

struct TemplateSelectorWidget::Data
{
    Data( TemplateSelectorWidget *parent )
        : m_view{ new QTreeView{ parent }}
    {

    }

    QTreeView *m_view;

};

TemplateSelectorWidget::TemplateSelectorWidget( QWidget* parent )
    : QWidget{ parent }
    , m_data{ new Data{ this }}
{
    auto main = new QVBoxLayout{ };
    main->addWidget( m_data->m_view );
    this->setLayout( main );
    main->setContentsMargins( QMargins{} );
    this->setContentsMargins( QMargins{} );
}

TemplateSelectorWidget::~TemplateSelectorWidget()
{

}

} } }