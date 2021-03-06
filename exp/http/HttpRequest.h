#pragma once

#include <QString>

#include <memory>

#include "Constants.h"
#include "Common.h"

namespace Quartz { namespace Http {

class MultipartFile;

class HttpRequest
{
public:
    HttpRequest( const QString &version, Method method, const QString &path);

    ~HttpRequest();

    const QString & version() const;

    Method method() const;

    const QString & path() const;

    ProgFunc progress() const;

    QString header( const QString key ) const;

    bool hasHeader( const QString key ) const;

    QString param( const QString &key ) const;

    bool hasFile( const QString &key ) const;

    const MultipartFile * multipartFile( const QString name ) const;

    QByteArray & body();

    void setHeader( const QString &key, const QString &value );

private:
    struct Data;
    std::unique_ptr< Data > m_data;
};

} }
