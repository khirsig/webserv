#include "Response.hpp"

#include "StatusCodes.hpp"

namespace http {

Response::Response(core::ByteBuffer& buf) : _buf(buf) {}

Response::~Response() {}

void Response::init(const Request& request, config::Server& server, config::Location& location) {
    connection_state = request.connection_state;

    bool folder = (request._uri_path_decoded[request._uri_path_decoded.size() - 1] == '/');
    config::Redirect* redir;
    if (folder)
        redir = _find_redir_folder(&location);
    else
        redir = _find_redir_file(&location, request._uri_path_decoded);
    if (redir) {
        _respond_redir(*redir);
        return;
    }

    // _buf.append("HTTP/1.1 200 OK\nContent-Length: 9\nContent-Type: text/plain\n\nRESPONSE\n");
    _buf.append(
        "HTTP/1.1 200 OK\r\nServer: nginx\r\nDate: Thu, 20 Oct 2022 10:13:17 GMT\r\nContent-Type: "
        "text/html\r\nContent-Length: 152\r\nLast-Modified: Fri, 23 Sep 2022 11:13:48 "
        "GMT\r\nConnection: close\r\nETag: \"632d94ec-9c\"\r\nAccept-Ranges: "
        "bytes\r\n\r\n<!doctype html>\n<html>\n\n<head>\n    <meta charset=utf-8>\n    "
        "<title>hello webserv</title>\n</head>\n\n<body>\n    <h1>hello webserv "
        "tim</h1>\n</body>\n\n</html>");

    // if path ends with / dann dir sonst file
    //      - check for index files
    //      if no index file
    //          - check for dir listing
    //              if no dir listing
    //                  error 404
    //              else
    //                  dir_listing
    // else if file
    //      - file exists
    //      if no file
    //          error 404
    //      else
    //          return file content
    //

    // if (cgi) -> go to cgi stuff
    // if (not cgi) only method GET allowed
    // open file with "root + request.path"
    // Construct response headers with chunked oder content-len based on file_size
}

config::Redirect* Response::_find_redir_folder(config::Location* location) {
    for (size_t i = 0; i < location->v_redirect.size(); i++) {
        if (location->v_redirect[i].origin == ".") {
            return &location->v_redirect[i];
        }
    }
    return NULL;
}

config::Redirect* Response::_find_redir_file(config::Location*  location,
                                             const std::string& file_path) {
    std::string origin(file_path.begin() + location->path.length(), file_path.end());
    for (size_t i = 0; i < location->v_redirect.size(); i++) {
        if (location->v_redirect[i].origin == origin) {
            return &location->v_redirect[i];
        }
    }
    return NULL;
}

void Response::_respond_redir(const config::Redirect& redir) {
    core::ByteBuffer body(512);
    core::ByteBuffer header(512);

    // BODY
    body.append("<html>\r\n<head><title>");
    body += g_status_codes.codes[redir.status_code];
    body.append("</title></head>\r\n<body>\r\n<center><h1>");
    body += g_status_codes.codes[redir.status_code];
    body.append("</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");

    // HEADER
    header.append("HTTP/1.1 ");
    header += g_status_codes.codes[redir.status_code];
    header.append("\r\nServer: webserv");
    header.append("\r\nContent-Type: text/html");
    header.append("\r\nContent-Length: ");
    header.append(std::to_string(body.size()).c_str());
    header.append("\r\nConnection: ");
    if (connection_state == http::CONNECTION_CLOSE)
        header.append("close");
    else
        header.append("keep-alive");
    header.append("\r\nLocation: ");
    header.append(redir.direction.c_str());
    header.append("\r\n\r\n");

    _buf = header + body;
}

}  // namespace http
