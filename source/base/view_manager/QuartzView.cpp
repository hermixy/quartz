
#include "QuartzView.h"

namespace  Quartz {

class QuartzView::Data
{
public:
    Data( const QString &viewId,
          const QString &category,
          const QString &displayName )
        : m_viewId( viewId )
        , m_categoryId( category )
        , m_displayName( displayName )
    {

    }

    QString m_viewId;

    QString m_categoryId;

    QString m_categoryName;

    QString m_displayName;
};

QuartzView::QuartzView( const QString &viewId,
                        const QString &category,
                        const QString &displayName,
                        QWidget *parent )
    : QWidget( parent )
//    , m_data( std::make_unique< Data >( viewId, category, displayName ))
    , m_data( new Data( viewId, category, displayName ))
{

}

QuartzView::~QuartzView()
{

}

const QString & QuartzView::viewId() const
{
    return m_data->m_viewId;
}

const QString & QuartzView::viewCategoryId() const
{
    return m_data->m_categoryId;
}

const QString & QuartzView::viewDisplayName() const
{
    return m_data->m_displayName;
}

const QString & QuartzView::viewCategoryName() const
{
    return m_data->m_categoryName;
}

}
