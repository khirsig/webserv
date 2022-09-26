#pragma once

namespace log {

enum LEVEL { DEBUG, INFO, SYSTEM };

const char COLOR_NO[] = "\033[m";
const char COLOR_RE[] = "\033[0;31m";
const char COLOR_RE_1[] = "\033[1;31m";
const char COLOR_YE[] = "\033[0;33m";
const char COLOR_YE_1[] = "\033[1;33m";
const char COLOR_GR[] = "\033[0;32m";
const char COLOR_GR_1[] = "\033[1;32m";
const char COLOR_BL[] = "\033[0;34m";
const char COLOR_BL_1[] = "\033[1;34m";
const char COLOR_PL[] = "\033[0;35m";
const char COLOR_PL_1[] = "\033[1;35m";
const char COLOR_CY[] = "\033[0;36m";
const char COLOR_CY_1[] = "\033[1;36m";

// void error(LEVEL level, const std::string& msg1, const std::string& msg2, const std::string&
// msg3);

}  // namespace log
