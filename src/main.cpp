
#include <string>

#include "config/Parser.hpp"
#include "core/Webserver.hpp"
#include "http/status_codes.hpp"
#include "settings.hpp"
#include "utils/timestamp.hpp"

const std::map<int, std::string> http::g_m_status_codes = http::new_m_status_codes();

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "Error\nusage: " << argv[0] << " [config_file]\n";
        return EXIT_FAILURE;
    }
    try {
        std::vector<config::Server> v_server;

        config::Parser parser;
        if (argc == 2)
            parser.parse(argv[1], v_server);
        else
            parser.parse(DEFAULT_CONFIG_FILE, v_server);

        core::Webserver webserver(v_server);
        webserver.run();
    } catch (const std::exception& e) {
        utils::print_timestamp(std::cerr);
        std::cerr << ": " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
