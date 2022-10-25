/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: khirsig <khirsig@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:53:20 by khirsig           #+#    #+#             */
/*   Updated: 2022/10/24 16:51:58 by khirsig          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Executor.hpp"

namespace cgi {

Executor::Executor(http::Request &request, http::Response &response,
                   core::EventNotificationInterface &eni)
    : _read_fd(-1),
      _write_fd(-1),
      _request(request),
      _response(response),
      _eni(eni),
      _request_index(_request._body_start) {}

Executor::~Executor() {
    close(_read_fd);  // delete events
}

void Executor::execute(std::string &root, std::string &path) {
    int read_fd[2];
    int write_fd[2];

    if (pipe(read_fd) == -1)
        throw 500;
    if (pipe(write_fd) == -1)
        throw 500;

    _id = fork();

    if (_id == -1) {
        close(read_fd[0]);
        close(read_fd[1]);
        close(write_fd[0]);
        close(write_fd[1]);
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

        _eni.add_event(_read_fd, EVFILT_READ, 0);
        if (_request._chunked_body) {
            if (_request._chunked_body_buf.size() > 0)
                _eni.add_event(_write_fd, EVFILT_WRITE, 0);
        } else if (_request._body_end != _request._body_start) {
            _eni.add_event(_write_fd, EVFILT_WRITE, 0);
        }
    }
}

void Executor::read(bool eof) {
    std::string temp_buf;
    char        buf[CGI_READ_BUFFER_SIZE];
    int         chars_read = 0;
    chars_read = ::read(_read_fd, buf, CGI_READ_BUFFER_SIZE - 1);
    buf[chars_read] = '\0';
    _response.buf.append(buf);
    if (eof && chars_read < CGI_READ_BUFFER_SIZE) {
        _eni.delete_event(_read_fd, EVFILT_READ);
        cgi::g_executor.erase(_read_fd);
        close(_read_fd);  // destructor?
        _read_fd = -1;
        _response.cgi_done = true;
    }
    _eni.add_event(_request.connection_fd, EVFILT_WRITE, 0);
}

void Executor::write(std::size_t max_size) {
    std::size_t left_bytes;
    std::size_t send_bytes;

    if (_request._chunked_body) {
        left_bytes = _request._chunked_body_buf.size() - _request._chunked_body_buf.pos;
        if (left_bytes > max_size)
            send_bytes = max_size;
        else
            send_bytes = left_bytes;
        ::write(_write_fd, &(_request._chunked_body_buf[_request._chunked_body_buf.pos]),
                send_bytes);
        _request._chunked_body_buf.pos += send_bytes;
        left_bytes = _request._chunked_body_buf.size() - _request._chunked_body_buf.pos;

    } else {
        left_bytes = _request._body_end - _request_index;
        if (left_bytes > max_size)
            send_bytes = max_size;
        else
            send_bytes = left_bytes;
        ::write(_write_fd, &(_request._buf[_request_index]), send_bytes);
        _request_index += send_bytes;
        left_bytes = _request._body_end - _request_index;
    }
    if (left_bytes == 0) {
        _eni.delete_event(_write_fd, EVFILT_WRITE);
        cgi::g_executor.erase(_write_fd);
        close(_write_fd);  // destructor?
        _write_fd = -1;
    }
}

int32_t Executor::get_read_fd() const { return _read_fd; }

int32_t Executor::get_write_fd() const { return _write_fd; }

void Executor::_run_program(std::string &root, std::string &path) {
    chdir(root.c_str());
    _request._uri_path_decoded.insert(0, ".");
    char **env_str = _get_env(_request._m_header);
    char **argv = _get_argv(path, _request._uri_path_decoded);
    if (execve(path.c_str(), argv, env_str) == -1)
        perror("execve");
    exit(EXIT_FAILURE);
}

char **Executor::_get_env(std::map<std::string, std::string> &env) {
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

void Executor::_update_env(std::map<std::string, std::string> &env) {
    std::map<std::string, std::string>::iterator it = env.find("QUERY_STRING");
    if (it != env.end())
        it->second = _request.to_string(_request._uri_query_start, _request._uri_query_end);
    it = env.find("REQUEST_METHOD");
    if (it != env.end())
        it->second = _request._method;
}

char **Executor::_get_argv(const std::string &path, const std::string &body) {
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

}  // namespace cgi