#include <catch.hpp>
#include "../clangtest.hpp"
#include <siplasplas/reflection/parser/api/core/clang/visitor.hpp>
#include <iostream>

using namespace ::cpp::reflection::parser::api::core::clang;

class CounterVisitor : public Visitor
{
public:
    bool visit(const Cursor& root) const
    {
        _count = 0;
        return Visitor::visit(root);
    }

    bool visit(const Cursor& root)
    {
        _count = 0;
        return Visitor::visit(root);
    }

    Visitor::Result onCursor(const Cursor& current, const Cursor& parent) const override
    {
        _count++;
        std::cout << current << std::endl;
        return Visitor::onCursor(current, parent);
    }

    Visitor::Result onCursor(const Cursor& current, const Cursor& parent) override
    {
        _count++;
        std::cout << current << std::endl;
        return Visitor::onCursor(current, parent);
    }

    std::size_t count() const
    {
        return _count;
    }

private:
    mutable std::size_t _count = 0;
};

SCENARIO("Visitors abort AST traversal by default")
{
    GIVEN("An index and a C++11 sourcefile to parse")
    {
        Index index;
        cpp::test::ClangTest::helloWorld("visitors_test.cpp");

        REQUIRE(!index.isNull());

        WHEN("The sourcefile is parsed with clang include dirs and C++11 option")
        {
            TranslationUnit tu = index.parse("visitors_test.cpp", CompileOptions()
                .I(SIPLASPLAS_LIBCLANG_INCLUDE_DIR)
                .I(SIPLASPLAS_LIBCLANG_SYSTEM_INCLUDE_DIR)
                .std("c++11")
            );

            REQUIRE(!tu.isNull());
            REQUIRE(tu.spelling().str().str() == "visitors_test.cpp");

            THEN("The visitation of the translation unit is aborted inmediately using default visitors")
            {
                CounterVisitor visitor;
                const CounterVisitor constVisitor;

                REQUIRE(visitor.visit(tu.cursor()));
                REQUIRE(visitor.count() == 1);
                REQUIRE(constVisitor.visit(tu.cursor()));
                REQUIRE(constVisitor.count() == 1);
            }
        }
    }
}
