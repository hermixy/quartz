project( qz_http )
cmake_minimum_required(VERSION 3.1)
cmake_policy( SET CMP0020 NEW )
include( $ENV{QUARTZ_ROOT}/cmake/quartz.cmake )


qz_add_qt()
qz_add_core()


vq_classes (
    HttpRequest
    HttpResponse
    HttpConnection
    HttpServer
    HttpClient
    MultipartFile
)

vq_sources(
)
vq_headers(
    Constants.h
    Common.h
    http_parser.h
    httplib.h
)

vq_create_app()
qz_install()
