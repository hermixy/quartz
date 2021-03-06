
#include <base/selector/Node.h>

#include "NodeProvider.h"

namespace Quartz { namespace Ext { namespace Creator {

const QString NodeProvider::EXTENSION_ID{
    "qzp.creator.provider.node" };
const QString NodeProvider::EXTENSION_NAME{
    "Quartz Creator Node" };

NodeProvider::NodeProvider()
    : AbstractGeneralNodeProvider{ EXTENSION_ID, EXTENSION_NAME }
{

}

NodeProvider::~NodeProvider()
{

}

bool NodeProvider::init()
{
    return true;
}

bool NodeProvider::destroy()
{
    return true;
}

QVector< std::shared_ptr< NodeInfo >> NodeProvider::nodes() const
{
    auto nodeInfo = std::make_shared< NodeInfo >( QStringList{},
                                                  "Creator",
                                                  "qzp.creator.content.main",
                                                  QIcon{} );
    QVector< std::shared_ptr< NodeInfo >> nodes;
    nodes.push_back( nodeInfo );
    return nodes;
}

} } }
