
#include <string>

#include "config/Parser.hpp"
#include "core/Webserver.hpp"
#include "http/status_codes.hpp"
#include "settings.hpp"
#include "utils/color.hpp"
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
        const char*    config_file = argc == 2 ? argv[1] : DEFAULT_CONFIG_FILE;
        parser.parse(config_file, v_server);

#if PRINT_LEVEL > 0
        std::cout << utils::COLOR_CY_1 << "Parsed config: " << utils::COLOR_NO << config_file
                  << std::endl;
#endif

        core::Webserver webserver(v_server);
        webserver.run();
    } catch (const std::exception& e) {
        std::cerr << "[";
        utils::print_timestamp(std::cerr);
        std::cerr << "]: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
