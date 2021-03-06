#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>

#include <core/logger/Logging.h>

#include <common/widgets/QzScroller.h>

#include "QuartzItem.h"
#include "AbstractTitleItemProvider.h"
#include "TitleBar.h"

namespace Quartz {

const QString TitleBar::ADAPTER_NAME{ "qz.title_bar" };


TitleBar::TitleBar( int height, QWidget *parent )
    : QWidget( parent )
    , m_height( height )

{
    QPixmap clspx = style()->standardPixmap( QStyle::SP_TitleBarCloseButton );
    QPixmap maxpx = style()->standardPixmap( QStyle::SP_TitleBarMaxButton );
    QPixmap minpx = style()->standardPixmap( QStyle::SP_TitleBarMinButton );


    m_scroller = new QzScroller( Qt::Horizontal, height, height, this );
    m_minimizeBtn = new QPushButton( this );
    m_maxRestoreBtn = new QPushButton( this );
    m_closeBtn = new QPushButton( this );
    m_closeBtn->setIcon( clspx );
    m_minimizeBtn->setIcon( minpx );
    m_maxRestoreBtn->setIcon( maxpx );

    m_minimizeBtn->setMaximumSize( 20, 20 );
    m_maxRestoreBtn->setMaximumSize( 20, 20 );
    m_closeBtn->setMaximumSize( 20, 20 );
    m_minimizeBtn->setMinimumSize( 20, 20 );
    m_maxRestoreBtn->setMinimumSize( 20, 20 );
    auto btnWidget = new QWidget{ this };
    auto btnLayout = new QHBoxLayout{ btnWidget };
    btnLayout->addWidget( m_minimizeBtn );
    btnLayout->addWidget( m_maxRestoreBtn );
    btnLayout->addWidget( m_closeBtn );
    btnWidget->setLayout( btnLayout );
    btnWidget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    btnWidget->setContentsMargins( QMargins{} );
    btnLayout->setContentsMargins( QMargins{} );


    m_scroller->setContentsMargins( QMargins{} );
    auto *layout = new QHBoxLayout();
    layout->setContentsMargins( QMargins() );
    layout->addWidget( m_scroller, 1 );
    layout->addStretch();
    layout->addWidget( btnWidget, 0 );
    m_closeBtn->setMinimumSize( 20, 20 );
    this->setContentsMargins( QMargins{ 0, 0, 3, 3 });
    this->setLayout( layout );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );


    connect( m_closeBtn,
             SIGNAL( clicked( bool )),
             this,
             SIGNAL( sigCloseRequested() ));
    connect( m_maxRestoreBtn,
             SIGNAL( clicked( bool )),
             this,
             SIGNAL( sigMaxRestoreRequested() ));
    connect( m_minimizeBtn,
             SIGNAL( clicked( bool )),
             this,
             SIGNAL( sigMinimizeRequested() ));


}

void TitleBar::addItem( QuartzItem *item )
{
    if( item != nullptr ) {
        item->setMaximumHeight( m_height );
        item->setContentsMargins( QMargins{} );
        m_items.insert( item->itemId(), item );
        m_scroller->addWidget( item );
    }
}


void TitleBar::removeItem( QuartzItem *item )
{
    if( item != nullptr && m_items.contains( item->itemId() )) {
        m_items.remove( item->itemId() );
        m_scroller->removeWidget( item );
        delete item;
    }
}


void TitleBar::removeItem( const QString &itemId )
{
    QuartzItem *item = m_items.value( itemId );
    if( item != nullptr ) {
        m_items.remove( itemId );
        m_scroller->removeWidget( item );
        delete item;
    }
}


QList< QuartzItem * > TitleBar::items() const
{
    return m_items.values();
}


QList< QuartzItem * > TitleBar::items( const QString category )
{
    QList< QuartzItem *> filteredItems;
    for( QuartzItem *item : m_items.values() ) {
        if( item->itemCategory() == category ) {
            filteredItems.append( item );
        }
    }
    return filteredItems;
}


void TitleBar::removeCategory( const QString &category )
{
    QList< QuartzItem * > itemList = items( category );
    for( QuartzItem *item : itemList ) {
        removeItem( item );
    }
}


void TitleBar::mouseDoubleClickEvent( QMouseEvent */*event*/ )
{
    emit sigMaxRestoreRequested();
}

const QString & TitleBar::extensionType() const
{
    return  AbstractTitleItemProvider::EXTENSION_TYPE;
}

const QString & TitleBar::extensionAdapterName() const
{
    return ADAPTER_NAME;
}

bool TitleBar::handleExtension( Ext::Extension *extension )
{
    bool result = false;
    auto itemProvider = dynamic_cast< AbstractTitleItemProvider *>( extension );
    if( itemProvider != nullptr ) {
        auto items = itemProvider->titleItems();
        foreach( auto item, items ) {
            addItem( item );
            m_extensionItems.push_back( item );
        }
        return true;
    }
    else {
        auto extensionName = extension != nullptr ? extension->extensionId()
                                            : "<null>";
        QZ_ERROR( "Qz:TitleBar" )
                << "Invalid titlebar extension provided: " << extensionName;
    }
    return result;
}

bool TitleBar::finalizeExtension()
{
//    for( int i = 0; i < m_extensionItems.size(); ++ i ) {
//        auto item = m_extensionItems.at( i );
//        removeItem( item );
//    }
    m_extensionItems.clear();
    return  true;
}



}
