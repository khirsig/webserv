#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "../core/Address.hpp"
#include "../http/status_codes.hpp"
#include "Server.hpp"
#include "Token.hpp"

static const uint32_t allowed_redir[] = {HTTP_MOVED_PERMANENTLY, HTTP_FOUND,
                                         HTTP_TEMPORARY_REDIRECT, HTTP_PERMANENT_REDIRECT};

enum state_location_path { START, SLASH, FIRST_DOT, SECOND_DOT, SEGMENT };

namespace config {

class Interpreter {
   public:
    Interpreter(const std::string &file_path) : _path(file_path) {}

    void parse(const std::vector<Token> &v_token, std::vector<Server> &v_server);

   private:
    const std::string  _path;
    const std::string *_last_directive;

    bool _parse_server(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       Server &server);
    Location _parse_location(const std::vector<Token>           &v_token,
                             std::vector<Token>::const_iterator &it);
    void _parse_string(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       std::vector<std::string> &v_identifier);
    void _parse_string(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       std::string &identifier);
    void _parse_cgi_pass(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                         CgiPass &identifier);
    bool _parse_listen(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                       core::Address &identifier);
    void _parse_port(std::vector<Token>::const_iterator &it, const std::string &str,
                     in_port_t &port);
    void _parse_error_page(const std::vector<Token>           &v_token,
                           std::vector<Token>::const_iterator &it,
                           std::map<int, http::error_page_t>  &m_error_page);
    void _parse_redirect(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                         Redirect &identifier);
    void _parse_bytes(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                      std::uint64_t &identifier);
    void _parse_location_path(const std::vector<Token>           &v_token,
                              std::vector<Token>::const_iterator &it, std::string &location_path);
    void _parse_bool(const std::vector<Token> &v_token, std::vector<Token>::const_iterator &it,
                     bool &identifier);

    void _increment_token(const std::vector<Token>           &v_token,
                          std::vector<Token>::const_iterator &it);

    void _invalid_directive(std::vector<Token>::const_iterator &it) const;
    void _directive_already_set(std::vector<Token>::const_iterator &it) const;
    void _unexpected_file_ending(std::vector<Token>::const_iterator &it) const;
    void _unexpected_eof(std::vector<Token>::const_iterator &it) const;
    void _unexpected_operator(std::vector<Token>::const_iterator &it) const;
    void _none_terminated_directive(std::vector<Token>::const_iterator &it) const;
    void _invalid_bool_argument(std::vector<Token>::const_iterator &it) const;
    void _invalid_directive_argument_amount(std::vector<Token>::const_iterator &it) const;
    void _missing_opening(std::vector<Token>::const_iterator &it, const char &op) const;
    void _invalid_status_code(std::vector<Token>::const_iterator &it,
                              const uint32_t                     &status_code) const;
    void _invalid_status_code(std::vector<Token>::const_iterator &it,
                              const std::string                  &status_code) const;
    void _invalid_path(std::vector<Token>::const_iterator &it) const;
    void _invalid_error_code(std::vector<Token>::const_iterator &it, const int32_t &code) const;
    void _duplicate_error_code(std::vector<Token>::const_iterator &it, const int32_t &code) const;
    void _could_not_open_file(std::vector<Token>::const_iterator &it) const;
    void _invalid_port(std::vector<Token>::const_iterator &it, const std::string &port_str) const;
    void _numeric_char_expected(std::vector<Token>::const_iterator &it,
                                const std::string                  &num) const;
    void _invalid_size_identifier(std::vector<Token>::const_iterator &it,
                                  const std::string                  &num) const;
    void _numeric_overflow(std::vector<Token>::const_iterator &it, const std::string &num) const;
    void _invalid_parameter(std::vector<Token>::const_iterator &it) const;
    void _wrong_method(std::vector<Token>::const_iterator &it, const std::string &method) const;
    void _multiple_operator_used(std::vector<Token>::const_iterator &it, const char &op) const;
};

}  // namespace config