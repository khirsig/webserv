
#include <string>

#include "config/Parser.hpp"
#include "core/Webserver.hpp"
#include "http/ErrorPages.hpp"
#include "settings.hpp"
#include "utils/timestamp.hpp"

http::ErrorPages  http::g_error_pages;
http::StatusCodes http::g_status_codes;

int main(int argc, char** argv) {
    try {
        std::string file_path;
        if (argc == 1)
            file_path = DEFAULT_CONF;
        else if (argc == 2)
            file_path = argv[1];
        else {
            std::cerr << "Error\nusage: ./build/webserv [config_file]\n";
            return EXIT_FAILURE;
        }

        http::g_status_codes.init();
        http::g_error_pages.init();

        config::Parser              parser;
        std::vector<config::Server> v_server;

        parser.parse(file_path, v_server);

        core::Webserver webserver(v_server, MAX_CONNECTIONS);
        webserver.run();
    } catch (const std::exception& e) {
        utils::print_timestamp(std::cerr);
        std::cerr << "" << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
