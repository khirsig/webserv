#include "CgiHandler.hpp"

#include "../http/status_codes.hpp"

namespace core {

CgiHandler::CgiHandler(const http::Request &request, http::Response &response)
    : _request(request), _response(response), _read_fd(-1), _write_fd(-1), _is_done(true) {
    _buf = new char[CGI_BUF_SIZE];
}

CgiHandler::~CgiHandler() {
    if (_read_fd != -1)
        close(_read_fd);
    if (_write_fd != -1)
        close(_write_fd);
    delete[] _buf;
}

void CgiHandler::init(int connection_fd) {
    if (_read_fd != -1)
        close(_read_fd);
    if (_write_fd != -1)
        close(_write_fd);
    _connection_fd = connection_fd;
}

void CgiHandler::execute(EventNotificationInterface &eni, const std::string &cgi_path,
                         const std::string &script_path) {
    reset(eni);

    int read_fd[2];
    int write_fd[2];

    if (pipe(read_fd) == -1) {
        reset(eni);
        throw HTTP_INTERNAL_SERVER_ERROR;
    }
    if (pipe(write_fd) == -1) {
        close(read_fd[0]);
        close(read_fd[1]);
        reset(eni);
        throw HTTP_INTERNAL_SERVER_ERROR;
    }

    _is_done = false;
    _pid = fork();

    if (_pid == -1) {
        close(read_fd[0]);
        close(read_fd[1]);
        close(write_fd[0]);
        close(write_fd[1]);
        reset(eni);
        throw HTTP_INTERNAL_SERVER_ERROR;
    }

    if (_pid == 0) {
        close(write_fd[1]);
        close(read_fd[0]);
        dup2(write_fd[0], STDIN_FILENO);
        close(write_fd[0]);
        dup2(read_fd[1], STDOUT_FILENO);
        close(read_fd[1]);
        _run_program(cgi_path, script_path);
    } else {
        _read_fd = read_fd[0];
        _write_fd = write_fd[1];
        close(read_fd[1]);
        close(write_fd[0]);

        eni.add_event(_read_fd, EVFILT_READ);
        eni.add_cgi_fd(_read_fd, this);
        if (!_request.body().empty()) {
            eni.add_event(_write_fd, EVFILT_WRITE);
            eni.add_cgi_fd(_write_fd, this);
        } else {
            close(_write_fd);
            _write_fd = -1;
        }
    }
}

void CgiHandler::reset(EventNotificationInterface &eni) {
    _is_done = true;
    _pid = -1;
    _body_pos = 0;
    if (_read_fd != -1) {
        eni.delete_event(_read_fd, EVFILT_READ);
        eni.remove_cgi_fd(_read_fd);
        close(_read_fd);
        _read_fd = -1;
    }
    if (_write_fd != -1) {
        eni.delete_event(_write_fd, EVFILT_WRITE);
        eni.remove_cgi_fd(_write_fd);
        close(_write_fd);
        _write_fd = -1;
    }
}

void CgiHandler::eof_read_write(EventNotificationInterface &eni) {
    // std::cerr << "CGI EOF" << std::endl;
    reset(eni);
    eni.enable_event(_connection_fd, EVFILT_WRITE);
    eni.add_timer(_connection_fd, TIMEOUT_TIME);
}

void CgiHandler::read(EventNotificationInterface &eni, size_t data_len) {
    size_t to_read_len = data_len < CGI_BUF_SIZE ? data_len : CGI_BUF_SIZE;
    int    read_len = ::read(_read_fd, _buf, to_read_len);
    if (read_len == -1) {
        reset(eni);
        throw std::runtime_error("Error reading from CGI");
    }
    _response.body().append(_buf, read_len);
    eni.enable_event(_connection_fd, EVFILT_WRITE);
    eni.add_timer(_connection_fd, TIMEOUT_TIME);
    // std::cerr << "CGI read " << read_len << " bytes" << std::endl;
}

void CgiHandler::write(EventNotificationInterface &eni, std::size_t max_size) {
    std::size_t left_bytes;
    std::size_t send_bytes;

    std::cerr << "CGI write" << std::endl;

    left_bytes = _request.body().size() - _body_pos;
    if (left_bytes > max_size)
        send_bytes = max_size;
    else
        send_bytes = left_bytes;
    ::write(_write_fd, &(_request.body()[_body_pos]), send_bytes);
    _body_pos += send_bytes;
    left_bytes = _request.body().size() - _body_pos;
    if (left_bytes == 0) {
        eni.delete_event(_write_fd, EVFILT_WRITE);
        eni.remove_cgi_fd(_write_fd);
        close(_write_fd);  // destructor?
        _write_fd = -1;
    }
}

bool CgiHandler::is_done() const { return _is_done; }

int32_t CgiHandler::get_read_fd() const { return _read_fd; }

int32_t CgiHandler::get_write_fd() const { return _write_fd; }

void CgiHandler::_run_program(const std::string &cgi_path, const std::string &script_path) {
    std::map<std::string, std::string> m_header(_request.m_header());

    char **env = _get_env(m_header);
    char **argv = _get_argv(cgi_path, script_path);

    chdir(_request.location()->root.c_str());
    execve(cgi_path.c_str(), argv, env);
    perror("execve");
    // free env && argv
    exit(EXIT_FAILURE);
}

char **CgiHandler::_get_env(std::map<std::string, std::string> &env) {
    for (uint32_t i = 0; i < sizeof(env_string) / sizeof(env_string[0]); ++i) {
        if (env.find(env_string[i]) == env.end()) {
            env.insert(std::make_pair(env_string[i], ""));
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
        it->second = _request.method_str();
    }
    it = env.find("PATH_INFO");
    if (it != env.end()) {
        it->second = _request.relative_path();
    }
    it = env.find("DOCUMENT_ROOT");
    if (it != env.end()) {
        it->second = _request.location()->root;
    }
    it = env.find("SCRIPT_FILENAME");
    if (it != env.end()) {
        it->second = _request.relative_path();
    }
    it = env.find("CONTENT_TYPE");
    if (it != env.end()) {
        std::map<std::string, std::string>::iterator it_tmp = env.find("CONTENT-TYPE");
        if (it_tmp != env.end()) {
            it->second = it_tmp->second;
        }
    }
    // TODO: Add all the other env variables
}

char **CgiHandler::_get_argv(const std::string &cgi_path, const std::string &script_path) {
    char **argv = new char *[3];
    argv[0] = new char[cgi_path.size() + 1];
    for (std::size_t i = 0; i < cgi_path.size(); ++i) {
        argv[0][i] = cgi_path[i];
    }
    argv[0][cgi_path.size()] = '\0';
    argv[1] = new char[script_path.size() + 1];
    for (std::size_t i = 0; i < script_path.size(); ++i) {
        argv[1][i] = script_path[i];
    }
    argv[1][script_path.size()] = '\0';
    argv[2] = NULL;
    return argv;
}

}  // namespace core
