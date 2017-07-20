#pragma once

#include <memory>


#include <core/utils/Macros.h>

//Just to keep clang quiet
class QVariant;

namespace Quartz {

QZ_INTERFACE ITreeNode
{
    virtual int numChildren() const = 0;

    virtual int numFields() const = 0;

    virtual ITreeNode * child( int row ) const = 0;

    virtual QVariant data( int column ) const = 0;

    virtual void setSelected( bool value ) = 0;

    virtual bool isSelected() const = 0;

    virtual ITreeNode * parent() const = 0;

    virtual int indexOfChild( const ITreeNode *child ) const = 0;

    virtual bool isEditable( int column ) const = 0;

    virtual void setData( int column, const QVariant &data );

    virtual void addChild( std::shared_ptr< ITreeNode > child ) = 0;

    virtual void removeChild( const ITreeNode *child ) = 0;

    virtual ~ITreeNode() {}

};

}
