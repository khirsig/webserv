#include "Response.hpp"

#include "../core/utils.hpp"
#include "../file/Handler.hpp"
#include "MimeTypes.hpp"
#include "StatusCodes.hpp"
#include "httpStatusCodes.hpp"

namespace http {

Response::Response() : state(RESPONSE_HEADER), content(RESPONSE_CONTENT_NONE) {}

Response::~Response() {}

void Response::init(Request& request, config::Server& server, config::Location& location) {
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

    if (folder) {
        for (size_t i = 0; i < location.v_index.size(); i++) {
            try {
                file.init(location.root + request._uri_path_decoded + location.v_index[i]);
                // check for cgi
                // if (request.method != "GET")
                //    throw HTTP_FORBIDDEN;
                // goto file part
                if (file.max_size() == 0)
                    content = RESPONSE_CONTENT_NONE;
                else
                    content = RESPONSE_CONTENT_FILE;
                _construct_header();
                return;
            } catch (...) {
            }
        }
        if (location.directory_listing) {
            buf.append("cgi dir listing\n");
            // content = RESPONSE_CONTENT_CGI;
            return;
        }
        throw HTTP_FORBIDDEN;
    } else {
        size_t type_pos = request._uri_path_decoded.rfind('.');
        if (type_pos != std::string::npos) {
            for (size_t i = 0; i < location.v_cgi_pass.size(); i++) {
                if (request._uri_path_decoded.compare(type_pos + 1,
                                                      request._uri_path_decoded.length() - type_pos,
                                                      location.v_cgi_pass[i].type) == 0) {
                    _construct_header_cgi();
                    // cgi::Executor* exec = new cgi::Executor(request, *this);

                    // exec->execute(request, location.root, location.v_cgi_pass[i].path);
                    // int         read_fd = exec->get_fd();
                    // std::string temp_buf;
                    // char*       __buf = new char[20];
                    // int         rd = 0;
                    // int         buf_count = 0;
                    // while ((rd = read(read_fd, __buf, 19)) > 0) {
                    //     buf_count += rd;
                    //     __buf[rd] = '\0';
                    //     temp_buf += __buf;
                    // }
                    // delete[] __buf;
                    // delete exec;
                    // buf.append(temp_buf.c_str());
                    content = RESPONSE_CONTENT_CGI;
                    return;
                }
            }
        }
        // if (request.method != "GET")
        //     throw HTTP_FORBIDDEN;
        file.init(location.root + request._uri_path_decoded);
        if (file.max_size() == 0)
            content = RESPONSE_CONTENT_NONE;
        else
            content = RESPONSE_CONTENT_FILE;
        _construct_header();
    }
}

// // _buf.append("HTTP/1.1 200 OK\nContent-Length: 9\nContent-Type: text/plain\n\nRESPONSE\n");
// _buf.append(
//     "HTTP/1.1 200 OK\r\nServer: nginx\r\nDate: Thu, 20 Oct 2022 10:13:17 GMT\r\nContent-Type:
//     " "text/html\r\nContent-Length: 152\r\nLast-Modified: Fri, 23 Sep 2022 11:13:48 "
//     "GMT\r\nConnection: close\r\nETag: \"632d94ec-9c\"\r\nAccept-Ranges: "
//     "bytes\r\n\r\n<!doctype html>\n<html>\n\n<head>\n    <meta charset=utf-8>\n    "
//     "<title>hello webserv</title>\n</head>\n\n<body>\n    <h1>hello webserv "
//     "tim</h1>\n</body>\n\n</html>");

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
    core::ByteBuffer body(256);

    // BODY
    body.append("<html>\r\n<head><title>");
    body += g_status_codes.codes[redir.status_code];
    body.append("</title></head>\r\n<body>\r\n<center><h1>");
    body += g_status_codes.codes[redir.status_code];
    body.append("</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");

    // HEADER
    buf.append("HTTP/1.1 ");
    buf += g_status_codes.codes[redir.status_code];
    buf.append("\r\nServer: webserv");
    buf.append("\r\nContent-Type: text/html");
    buf.append("\r\nContent-Length: ");
    buf.append(SSTR(body.size()).c_str());
    buf.append("\r\nConnection: ");
    if (connection_state == http::CONNECTION_CLOSE)
        buf.append("close");
    else
        buf.append("keep-alive");
    buf.append("\r\nLocation: ");
    buf.append(redir.direction.c_str());
    buf.append("\r\n\r\n");

    buf += body;
}

void Response::_construct_header() {
    buf.append("HTTP/1.1 200 OK");
    buf.append("\r\nServer: webserv");
    buf.append("\r\nConnection: ");
    if (connection_state == CONNECTION_CLOSE)
        buf.append("close");
    else
        buf.append("keep-alive");
    buf.append("\r\nContent-Type: ");
    buf.append(mime_type(file.get_path()));
    buf.append("\r\nContent-Length: ");
    buf.append(SSTR(file.max_size()).c_str());
    buf.append("\r\n\r\n");
}

void Response::_construct_header_cgi() {
    buf.append("HTTP/1.1 200 OK");
    buf.append("\r\nServer: webserv");
    buf.append("\r\nContent-Type: text/html; charset=UTF-8");
    buf.append("\r\nTransfer-Encoding: chunked");
    buf.append("\r\nConnection: ");
    if (connection_state == CONNECTION_CLOSE)
        buf.append("close");
    else
        buf.append("keep-alive");
    buf.append("\r\n\r\n");
}

}  // namespace http
