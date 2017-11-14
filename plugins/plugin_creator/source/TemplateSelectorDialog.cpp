#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>

#include <common/model_view/CheckBoxDeligate.h>
#include <common/model_view/EditorDelegate.h>
#include <common/templating/TemplateInstance.h>
#include <common/templating/Template.h>
#include <common/widgets/QzTreeView.h>

#include "TemplateManager.h"
#include "TemplateSelectorDialog.h"

namespace Quartz { namespace Plugin { namespace Creator {

struct TemplateSelectorDialog::Data
{
    Data( TemplateManager *templateManager,
          TemplateSelectorDialog *parent )
        : m_templateManager{ templateManager }
        , m_filterEdit{ new QLineEdit{ parent }}
        , m_view{ new QzTreeView{ parent }}
    {

    }

    TemplateManager *m_templateManager;

    QLineEdit *m_filterEdit;

    QzTreeView *m_view;

    //    QHash< TemplateInstance *, VarConfigWidget *> m_configWidgets;
};

TemplateSelectorDialog::TemplateSelectorDialog(
        TemplateManager *templateManager,
        QWidget* parent )
    : QDialog{ parent }
    , m_data{ new Data{ templateManager, this }}
{
    m_data->m_view->setModel( m_data->m_templateManager );

    auto okBtn = new QPushButton{ tr( "Select" ), this };
    auto cancelBtn = new QPushButton{ tr( "Cancel"), this };
    auto btnLyt = new QHBoxLayout{};
    btnLyt->addStretch();
    btnLyt->addWidget( okBtn );
    btnLyt->addWidget( cancelBtn );

    auto main = new QVBoxLayout{ };
    main->addWidget( m_data->m_filterEdit );
    main->addWidget( m_data->m_view );
    main->addLayout( btnLyt );
    this->setLayout( main );
    this->setMinimumSize( QSize{ 500, 600 });

    connect( okBtn, &QPushButton::released, [ this ](){
        this->accept();
    });
    connect( cancelBtn, &QPushButton::released, [ this ](){
        this->reject();
    });
}

TemplateSelectorDialog::~TemplateSelectorDialog()
{

}

QVector< Template * > TemplateSelectorDialog::selectedTemplates() const
{
    QVector< Template * > selected;
    for( auto i = 0; i < m_data->m_templateManager->numTemplates(); ++ i ) {
        auto t = m_data->m_templateManager->templateAt( i );
        if( t->isSelected() ) {
            selected.append( t );
        }
    }
    return selected;
}

void TemplateSelectorDialog::clearSelection()
{
    for( auto i = 0; i < m_data->m_templateManager->numTemplates(); ++ i ) {
        auto t = m_data->m_templateManager->templateAt( i );
        if( t != nullptr ) {
            t->setSelected( false );
        }
//        m_data->m_templateManager->templateAt( i )->setSelected( false );
    }
}

} } }
