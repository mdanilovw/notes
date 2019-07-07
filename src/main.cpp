/**
 * Application entry point
 */

#include <cstring>
#include <memory>
#include "../include/cli.hpp"
#include "../include/core.hpp"

using std::shared_ptr;

int main(int argc, char *argv[])
{
    bool encryption;
    if(argc > 1 && strcmp(argv[1], Cli::NO_ENCRYPTION) == 0) 
        encryption = false;
    else 
        encryption = true;

    try { 
        shared_ptr<Core> core{ new Core }; 
        shared_ptr<CoreService> coreService{ new NetworkCoreService { core } };
        coreService->start();

        //Cli cli{ coreService, encryption };
        //cli.start();

        coreService->stop();
        return 0;
    }
    catch (const string& s) {
        std::cerr << s << std::endl;
    }
    catch (const char* c) {
        std::cerr << c << std::endl;
    }
    catch (...) {
        std::cerr << "\nThe application is terminated due to unexpected exception\n";
    }
    return 1;
}
