// Test runner. Tests live in the *Tests.cpp files.
//
//   AcademiaEngine.Tests.exe                      unit tests only (no window)
//   AcademiaEngine.Tests.exe --visual             visual tests only (opens the engine window)
//   AcademiaEngine.Tests.exe --visual-hold=2      visual tests, each step stays 2 seconds on screen
//
// Any doctest option also works, e.g. -tc="Camera*" or --help.
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include <cstdlib>
#include <cstring>

#include "VisualTest.h"

int main(int argc, char** argv)
{
    bool visual = false;

    for (int i = 1; i < argc; ++i)
    {
        constexpr const char* holdOption = "--visual-hold=";
        if (std::strcmp(argv[i], "--visual") == 0)
        {
            visual = true;
        }
        else if (std::strncmp(argv[i], holdOption, std::strlen(holdOption)) == 0)
        {
            visual = true;
            VisualTestEngine::HoldSeconds = static_cast<float>(std::atof(argv[i] + std::strlen(holdOption)));
        }
    }

    doctest::Context context;
    context.addFilter(visual ? "test-suite" : "test-suite-exclude", "Visual");
    context.applyCommandLine(argc, argv);

    return context.run();
}
