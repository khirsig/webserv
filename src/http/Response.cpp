#include "Response.hpp"

#include "../core/utils.hpp"
#include "../file/Handler.hpp"
#include "MimeTypes.hpp"
#include "StatusCodes.hpp"
#include "httpStatusCodes.hpp"

namespace http {

Response::Response()
    : connection_state(http::CONNECTION_KEEP_ALIVE),
      state(RESPONSE_HEADER),
      content(RESPONSE_CONTENT_NONE),
      cgi_done(false),
      executor(NULL) {}

Response::~Response() {
    if (executor) {
        cgi::g_executor.erase(executor->get_read_fd());
        cgi::g_executor.erase(executor->get_write_fd());
        delete executor;
    }
}

inline bool find_index(const std::string& file_path, config::Location& location,
                       file::Handler& file) {
    // std::string origin(request._uri_path_decoded.begin() + location.path.length(),
    //                    request._uri_path_decoded.end());
    for (size_t i = 0; i < location.v_index.size(); i++) {
        try {
            std::cerr << file_path + location.v_index[i] << std::endl;
            file.init(file_path + "/" + location.v_index[i]);
            return true;
        } catch (...) {
        }
    }
    return false;
}

config::CgiPass* find_cgi_pass(Request& request, config::Location& location) {
    size_t type_pos = request._uri_path_decoded.rfind('.');
    if (type_pos != std::string::npos) {
        for (size_t i = 0; i < location.v_cgi_pass.size(); i++) {
            if (request._uri_path_decoded.compare(type_pos + 1,
                                                  request._uri_path_decoded.length() - type_pos,
                                                  location.v_cgi_pass[i].type) == 0) {
                return &location.v_cgi_pass[i];
            }
        }
    }
    return NULL;
}

void Response::init(Request& request, config::Location& location,
                    core::EventNotificationInterface& eni) {
    connection_state = request.connection_state;

    bool        directory;
    std::string relative_file_path(request._uri_path_decoded.begin() + location.path.length(),
                                   request._uri_path_decoded.end());
    std::string file_path(location.root + relative_file_path);
    directory = !file.init(file_path);

    config::Redirect* redir;
    if (directory)
        redir = _find_redir_folder(&location);
    else
        redir = _find_redir_file(&location, relative_file_path);
    if (redir) {
        _respond_redir(*redir);
        return;
    }

    if (directory && !find_index(file_path, location, file)) {
        if (location.directory_listing) {
            buf.append("cgi dir listing\n");
            // _construct_header_cgi();
            // cgi::Executor* exec = new cgi::Executor(request, *this, eni);
            // exec->execute(location.root, "path_to_cgi_for_dir_listing");
            // cgi::g_executor.insert(exec->get_read_fd(), exec);
            // cgi::g_executor.insert(exec->get_write_fd(), exec);
            // content = RESPONSE_CONTENT_CGI;
            return;
        }
        throw HTTP_NOT_FOUND;
    } else {
        config::CgiPass* cgi_pass = find_cgi_pass(request, location);
        if (cgi_pass) {
            _construct_header_cgi();
            executor = new cgi::Executor(request, *this, eni);
            executor->execute(location.root, cgi_pass->path);
            cgi::g_executor.insert(std::make_pair(executor->get_read_fd(), executor));
            cgi::g_executor.insert(std::make_pair(executor->get_write_fd(), executor));
            content = RESPONSE_CONTENT_CGI;
            return;
        }
        if (request._method != "GET" && request._method != "HEAD")
            throw HTTP_METHOD_NOT_ALLOWED;
        // if (!file.is_open()) {
        //     std::string origin(request._uri_path_decoded.begin() + location.path.length(),
        //                        request._uri_path_decoded.end());
        //     // file.init(location.root + origin);
        // }
        if (file.max_size() == 0 || request._method == "HEAD")
            content = RESPONSE_CONTENT_NONE;
        else
            content = RESPONSE_CONTENT_FILE;
        _construct_header();
    }
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
                                             const std::string& relative_file_path) {
    // std::string origin(file_path.begin() + location->path.length() + 1, file_path.end());
    for (size_t i = 0; i < location->v_redirect.size(); i++) {
        if (location->v_redirect[i].origin == relative_file_path) {
            return &location->v_redirect[i];
        }
    }
    return NULL;
}

void Response::_respond_redir(const config::Redirect& redir) {
    core::ByteBuffer body(256);

    // BODY
    buf.append("<html>\r\n<head><title>");
    buf += g_status_codes.codes[redir.status_code];
    buf.append("</title></head>\r\n<body>\r\n<center><h1>");
    buf += g_status_codes.codes[redir.status_code];
    buf.append("</h1></center>\r\n<hr><center>webserv</center>\r\n</body>\r\n</html>\r\n");

    // HEADER
    header.append("HTTP/1.1 ");
    header += g_status_codes.codes[redir.status_code];
    header.append("\r\nServer: webserv");
    header.append("\r\nContent-Type: text/html");
    header.append("\r\nContent-Length: ");
    header.append(SSTR_DEC(body.size()).c_str());
    header.append("\r\nConnection: ");
    if (connection_state == http::CONNECTION_CLOSE)
        header.append("close");
    else
        header.append("keep-alive");
    header.append("\r\nLocation: ");
    header.append(redir.direction.c_str());
    header.append("\r\n\r\n");
}

void Response::_construct_header() {
    header.append("HTTP/1.1 200 OK");
    header.append("\r\nServer: webserv");
    header.append("\r\nConnection: ");
    if (connection_state == CONNECTION_CLOSE)
        header.append("close");
    else
        header.append("keep-alive");
    header.append("\r\nContent-Type: ");
    header.append(mime_type(file.get_path()));
    header.append("\r\nContent-Length: ");
    header.append(SSTR_DEC(file.max_size()).c_str());
    header.append("\r\n\r\n");
}

void Response::_construct_header_cgi() {
    header.append("HTTP/1.1 200 OK");
    header.append("\r\nServer: webserv");
    header.append("\r\nContent-Type: text/html; charset=UTF-8");
    header.append("\r\nTransfer-Encoding: chunked");
    header.append("\r\nConnection: ");
    if (connection_state == CONNECTION_CLOSE)
        header.append("close");
    else
        header.append("keep-alive");
    header.append("\r\n\r\n");
}

}  // namespace http
