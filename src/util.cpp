#include "util.hpp"
#include <cstdio>
#include <unistd.h>
namespace Util {
    void SetTerminalNormalInputMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &SAVED_TERM_ATTR);
    }

    void SetTerminalPasswordInputMode() {
        tcgetattr(STDIN_FILENO, &SAVED_TERM_ATTR);
        atexit(SetTerminalNormalInputMode);
        
        struct termios current_attr;
        tcgetattr(STDIN_FILENO, &current_attr);
        
        current_attr.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &current_attr);
    }
}

