#include "RequestHandler.hpp"

namespace http {

http::Response* RequestHandler::get_response(const http::Request& req) {
    http::Response* response = new http::Response;
    response->buf.append("Response: \nMethod: ", 19);
    response->buf.insert(response->buf.end(), req._buf.begin() + req._method_start,
                         req._buf.begin() + req._method_end);
    response->buf.append("\nPath: ", 7);
    response->buf.insert(response->buf.end(), req._buf.begin() + req._uri_path_start,
                         req._buf.begin() + req._uri_path_end);
    response->buf.append("\n", 1);
    return response;
}

}  // namespace http
