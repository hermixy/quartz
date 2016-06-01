#pragma once

#include <QString>
#include <QHash>

#include <vqcore/common/Macros.h>
#include <quartz_core/QuartzCore.h>


#include "Param.h"
#include "Config.h"

namespace Vq { namespace Quartz {

class ConfigParser
{
public:
    std::size_t parse( const QString &filePath,
                       VQ_OUT QHash< QString, Config::Ptr > &configsOut);

private:

};


} }
