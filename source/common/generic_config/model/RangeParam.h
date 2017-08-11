#pragma once

#include "Param.h"

class QString;

namespace Quartz {

class RangeParam : public Param
{
public:
    RangeParam( const QString &id,
                const QString &name,
                const QString &description );

    ~RangeParam();

    int maxVal() const;

    void setMax( int max );

    int minVal() const;

    void setMin( int min );

    int inc() const;

    void setIncrement( int inc );

    int defaultValue() const;

    void setDefaultValue( int defaultValue );

    ParamType type() const override;

private:
    struct Data;
    std::unique_ptr< Data > m_data;
};

}
