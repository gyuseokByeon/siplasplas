#include <siplasplas/reflection/parser/api/core/clang/index.hpp>
#include <siplasplas/utility/exception.hpp>
#include <siplasplas/utility/fusion.hpp>
#include <gmock/gmock.h>
#include <iostream>
#include <fstream>
#include <range/v3/all.hpp>
#include "clangtest.hpp"

using namespace ::testing;
using namespace ::cpp::reflection::parser::api::core::clang;

class IndexTest : public Test, public cpp::test::ClangTest
{
};

TEST_F(IndexTest, contruction_cxIndexNotNull)
{
    EXPECT_FALSE(index.isNull());
}

TEST_F(IndexTest, parse_returnsNullTranslationUnitOnError)
{
    TranslationUnit tu = index.parse("hello.cpp", CompileOptions({"-std=c++11"}));

    EXPECT_TRUE(tu.isNull());

    std::cout << "Translation unit: " << tu.spelling() << std::endl;
}

TEST_F(IndexTest, parse_returnsNonNullTranslationUnitOnSuccess)
{
    helloWorld("hello_world.cpp");
    TranslationUnit tu = index.parse("hello_world.cpp", CompileOptions({"-std=c++11"}));

    EXPECT_FALSE(tu.isNull());
    EXPECT_STREQ("hello_world.cpp", tu.spelling().c_str());
}

TEST_F(IndexTest, parse_stdargNotFoundIfNotClangIncludes)
{
    helloWorld("hello_world.cpp");
    TranslationUnit tu = index.parse("hello_world.cpp", CompileOptions({"-std=c++11"}));

    EXPECT_FALSE(tu.isNull());
    const auto& diagnostics = tu.diagnostics() | ranges::view::bounded;

    EXPECT_EQ(1, diagnostics.size());
    EXPECT_STREQ("fatal error: 'stdarg.h' file not found", diagnostics[0].display({}).c_str());
}

TEST_F(IndexTest, parse_noDiagnosticsIfClangIncludesSet)
{
    helloWorld("hello_world.cpp");

    TranslationUnit tu = index.parse("hello_world.cpp", CompileOptions()
        .std("c++11")
        .D("FOO", 42)
        .D("BAR", "bar")
        .I(SIPLASPLAS_LIBCLANG_INCLUDE_DIR)
        .I(SIPLASPLAS_LIBCLANG_SYSTEM_INCLUDE_DIR)
    );

    EXPECT_FALSE(tu.isNull());
    EXPECT_TRUE(tu.noDiagnostics());
}
