#pragma once

#include <memory>

#include <QWidget>

#include "../QuartzBase.h"

namespace Quartz {

class QUARTZ_BASE_API QuartzView : public QWidget
{
    Q_OBJECT

public:
    QuartzView( const QString &viewId,
                const QString &category,
                const QString &displayName,
                QWidget *parent = nullptr );

    ~QuartzView();

    const QString & viewId() const;

    const QString & viewCategoryId() const;

    const QString & viewDisplayName() const;

    const QString & viewCategoryName() const;

private:
    struct Data;
    std::unique_ptr< Data > m_data;

};

}
