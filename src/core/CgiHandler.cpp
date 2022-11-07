#include "CgiHandler.hpp"

namespace core {

CgiHandler::CgiHandler(http::Request &request, http::Response &response,
                       core::EventNotificationInterface &eni)
    : _read_fd(-1),
      _write_fd(-1),
      _request(request),
      _response(response),
      _eni(eni),
      _is_done(true) {}

CgiHandler::~CgiHandler() {
    close(_read_fd);  // delete events
}

void CgiHandler::init(int connection_fd) {
    reset();
    _connection_fd = connection_fd;
}

void CgiHandler::execute(std::string &root, std::string &path) {
    reset();

    int read_fd[2];
    int write_fd[2];

    if (pipe(read_fd) == -1) {
        reset();
        throw 500;
    }
    if (pipe(write_fd) == -1) {
        reset();
        throw 500;
    }

    _is_done = false;
    _id = fork();

    if (_id == -1) {
        close(read_fd[0]);
        close(read_fd[1]);
        close(write_fd[0]);
        close(write_fd[1]);
        reset();
        throw 500;
    }

    if (_id == 0) {
        close(write_fd[1]);
        close(read_fd[0]);
        dup2(write_fd[0], STDIN_FILENO);
        close(write_fd[0]);
        dup2(read_fd[1], STDOUT_FILENO);
        close(read_fd[1]);
        _run_program(root, path);
    } else {
        _read_fd = read_fd[0];
        _write_fd = write_fd[1];
        close(read_fd[1]);
        close(write_fd[0]);

        _eni.enable_event(_read_fd, EVFILT_READ);
        _eni.add_cgi_fd(this, _read_fd);
        if (_request.body().begin() != _request.body().end()) {
            _eni.add_event(_write_fd, EVFILT_WRITE);
            _eni.add_cgi_fd(this, _write_fd);
        }
    }
}

void CgiHandler::reset() {
    _is_done = true;
    _id = -1;
    _body_pos = 0;
    if (_read_fd != -1) {
        close(_read_fd);
        _read_fd = -1;
    }
    if (_write_fd != -1) {
        close(_write_fd);
        _write_fd = -1;
    }
}

void CgiHandler::read(bool eof) {
    std::string temp_buf;
    char        buf[CGI_READ_BUFFER_SIZE];
    int         chars_read = 0;
    chars_read = ::read(_read_fd, buf, CGI_READ_BUFFER_SIZE - 1);  // write in global buff
    buf[chars_read] = '\0';
    _response.body().append(buf);  // write in response body
    if (eof && chars_read < CGI_READ_BUFFER_SIZE) {
        _eni.disable_event(_read_fd, EVFILT_READ);
        _eni.remove_cgi_fd(_read_fd);
        close(_read_fd);  // destructor?
        _read_fd = -1;
        _is_done = true;
    }
    _eni.enable_event(_connection_fd, EVFILT_WRITE);
}

void CgiHandler::write(std::size_t max_size) {
    std::size_t left_bytes;
    std::size_t send_bytes;

    left_bytes = _request.body().size() - _body_pos;
    if (left_bytes > max_size)
        send_bytes = max_size;
    else
        send_bytes = left_bytes;
    ::write(_write_fd, &(_request.body()[_body_pos]), send_bytes);
    _body_pos += send_bytes;
    left_bytes = _request.body().size() - _body_pos;
    if (left_bytes == 0) {
        _eni.disable_event(_write_fd, EVFILT_WRITE);
        _eni.remove_cgi_fd(_write_fd);
        close(_write_fd);  // destructor?
        _write_fd = -1;
    }
}

bool CgiHandler::is_done() const { return _is_done; }

int32_t CgiHandler::get_read_fd() const { return _read_fd; }

int32_t CgiHandler::get_write_fd() const { return _write_fd; }

void CgiHandler::_run_program(std::string &root, std::string &path) {
    chdir(root.c_str());
    std::string path_decoded(_request.path_decoded());
    path_decoded.insert(0, ".");
    std::map<std::string, std::string> m_header(_request.m_header());
    char                             **env_str = _get_env(m_header);
    char                             **argv = _get_argv(path, path_decoded);
    if (execve(path.c_str(), argv, env_str) == -1)
        perror("execve");
    exit(EXIT_FAILURE);
}

char **CgiHandler::_get_env(std::map<std::string, std::string> &env) {
    for (uint32_t i = 0; i < env_arr_length; ++i) {
        if (env.find(env_string[i]) == env.end()) {
            std::string key = env_string[i];
            std::string val;
            env.insert(std::make_pair<std::string, std::string>(key, val));
        }
    }

    _update_env(env);

    char   **ret = new char *[env.size() + 1];
    uint32_t i = 0;
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
        ret[i] = new char[it->first.length() + it->second.length() + 2];
        uint32_t j = 0;
        for (uint32_t k = 0; k < it->first.length(); ++k, ++j) {
            ret[i][j] = it->first[k];
        }
        ret[i][j] = '=';
        ++j;
        for (uint32_t k = 0; k < it->second.length(); ++k, ++j) {
            ret[i][j] = it->second[k];
        }
        ret[i][j] = '\0';
        ++i;
    }
    ret[env.size()] = NULL;

    return ret;
}

void CgiHandler::_update_env(std::map<std::string, std::string> &env) {
    std::map<std::string, std::string>::iterator it = env.find("QUERY_STRING");
    if (it != env.end())
        it->second = _request.query_string();
    it = env.find("REQUEST_METHOD");
    if (it != env.end()) {
        switch (_request.method()) {
            case http::Request::Method::GET:
                it->second = "GET";
                break;
            case http::Request::Method::POST:
                it->second = "POST";
                break;
            case http::Request::Method::DELETE:
                it->second = "DELETE";
                break;
            case http::Request::Method::HEAD:
                it->second = "HEAD";
                break;
        }
    }
}

char **CgiHandler::_get_argv(const std::string &path, const std::string &body) {
    char **argv = new char *[3];
    argv[0] = new char[path.size() + 1];
    for (std::size_t i = 0; i < path.size(); ++i) {
        argv[0][i] = path[i];
    }
    argv[0][path.size()] = '\0';
    argv[1] = new char[body.size() + 1];
    for (std::size_t i = 0; i < body.size(); ++i) {
        argv[1][i] = body[i];
    }
    argv[1][body.size()] = '\0';
    argv[2] = NULL;
    return argv;
}

}  // namespace core