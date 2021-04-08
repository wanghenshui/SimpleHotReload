#include <doctest/doctest.h>
#include "hot_reload.h"
#include "foo.h"
#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>


const char* g_Test_v1 =
    "#include \"foo.h\"\n"
    "int bar = 42;\n"
    "int foo(int x) { return x + 5; }";

const char* g_Test_v2 =
    "#include \"foo.h\"\n"
    "int bar = -2;\n"
    "int foo(int x) { return x - 5; }";

/// @brief Fixture class
class FooModuleReloadTest {
public:
    FooModuleReloadTest () {
      REQUIRE(!std::system("cd ../build"));
      SetUp();
    }
    ~FooModuleReloadTest () {
      TearDown();
    }
    void SetUp()
    {
        WriteFile("../source/foo.cpp", g_Test_v1);
        Compile();
        FooModule::ReloadLibrary();
    }

    void ChangeAndReload()
    {
        WriteFile("../source/foo.cpp", g_Test_v2);
        Compile();
        FooModule::ReloadLibrary();
    }

    void TearDown()
    {
        FooModule::UnloadLibrary();
    }

private:

    void WriteFile(const char* path, const char* content)
    {
        // Output file stream and delete existing output
        std::ofstream out(path, std::ios_base::out | std::ios_base::trunc );
        out << content;
        out.close();
    }

    void Compile()
    {
        REQUIRE(!std::system("make"));
        sleep(1);
    }
};

/// @brief test if GetBar() is reloaded
TEST_CASE_FIXTURE(FooModuleReloadTest, "Reload")
{
    REQUIRE(FooModule::GetBar() == 42);
    REQUIRE(FooModule::Foo(4) == 9);
    ChangeAndReload();
    REQUIRE(FooModule::GetBar() == -2);
    REQUIRE(FooModule::Foo(4) == -1);
}
