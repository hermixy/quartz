
#include "Constants.h"
#include "HttpRequest.h"
#include "MultipartFile.h"

namespace Quartz { namespace Http {

struct HttpRequest::Data
{
    QString m_version;

    Method m_method;

    QString m_path;

    Headers m_headers;

    QString m_body;

    Param m_params;

    QHash< QString, std::shared_ptr< MultipartFile >> m_files;

    ProgFunc m_progCallback;

};

HttpRequest::HttpRequest()
    : m_data{ new Data{} }
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::setHeader( const QString &key, const QString &value )
{
    m_data->m_headers.insert( key, value );
}

QString HttpRequest::header( const QString key ) const
{
    return m_data->m_headers.value( key );
}

const QString & HttpRequest::version() const
{
    return m_data->m_version;
}

Method HttpRequest::method() const
{
    return m_data->m_method;
}

const QString & HttpRequest::path() const
{
    return m_data->m_path;
}

ProgFunc HttpRequest::progress() const
{
    return m_data->m_progCallback;
}

QString HttpRequest::param( const QString &key ) const
{
    return m_data->m_params.value( key );
}

bool HttpRequest::hasFile( const QString &key ) const
{
    return m_data->m_files.contains( key );
}

const MultipartFile * HttpRequest::multipartFile( const QString name ) const
{
    MultipartFile *file = nullptr;
    if( m_data->m_files.contains( name )) {
        file = m_data->m_files.value( name ).get();
    }
    return file;
}

QString & HttpRequest::body()
{
    return m_data->m_body;
}

} }
