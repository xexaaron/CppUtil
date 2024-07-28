#include <string_view>
#include <filesystem>
#include <iostream>

namespace Utility {
    class App {
    public:
        static void Init(int argc, char** argv);
        static std::string_view Name();
        static std::filesystem::path ExecutablePath();
        static std::filesystem::path ResourceDirectory(); 
    private:
        static int    s_Argc;
        static char** s_Argv;
    };
}

#ifdef _WIN32
    extern "C" {
        /** https://learn.microsoft.com/en-us/windows/console/setconsoleoutputcp */
        int SetConsoleOutputCP(unsigned int wCodePageID);
    }
#endif

namespace Utility { 
    int    App::s_Argc = 0;
    char** App::s_Argv = nullptr;

    void App::Init(int argc, char** argv) {
        s_Argc = argc;
        s_Argv = argv;
        std::cout.setf(std::ios::boolalpha); 
		std::cin.setf(std::ios::boolalpha);
        #ifdef _WIN32
            SetConsoleOutputCP(65001 /* CP_UTF8 */);
        #endif
	}

    std::string_view App::Name() {
        return std::filesystem::path(s_Argv[0]).filename().string();
    }

    std::filesystem::path App::ExecutablePath() {
        return std::filesystem::path(s_Argv[0]);
    }

    std::filesystem::path App::ResourceDirectory() {
        return ExecutablePath().parent_path() / "Resources";
    }

}