#include "Log.hpp"

#include <cerrno>
#include <string>

namespace log {

static const char* level_str[] = {"DEBUG", "INFO", "SYSTEM"};

// void error(LEVEL level, const std::string& msg1, const std::string& msg2, const std::string& msg3) {
//     fprintf(stderr, "[%s]: %s: %s %s\n", level_str[level], msg1.c_str(), msg2.c_str(),
//             msg3.c_str());
// }

// void access(LEVEL level, const std::string& msg) {
//     fprintf(stdout, "[%s]: %s\n", level_str[level], msg.c_str());
// }

// void debug(LEVEL level, const std::string& msg) {
//     fprintf(stdout, "[%s]: %s\n", level_str[level], msg.c_str());
// }

}  // namespace log

// LOGS

// ### ERRORS
// * system errors:         msg + str(errno) + [ msg ]

// ### INFO
// * accept new connection: DESCRIPTION + IP:PORT
// * timeout connection:    DESCRIPTION + IP:PORT
// * closed connection:     DESCRIPTION + IP:PORT

// ### DEBUG
// * parsed request:        COLORED INFOS
