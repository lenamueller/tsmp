#include <string_view>
#include <tsmp/proxy.hpp>
#include <catch2/catch_all.hpp>

struct foo_t {
    int a = 0;

    constexpr static int add(int a, int b) {
        return a+b;
    }

    constexpr foo_t& inc() {
        ++a;
        return *this;
    }
};

struct interface_t {
    virtual ~interface_t() = default;

    virtual std::string print() const = 0;


private:
    void private_function() {}
};

struct foo_impl_t : public interface_t {
    virtual std::string print() const override {
        return "foo";
    }
};

struct bar_impl_t : public interface_t {
    virtual std::string print() const override {
        return "bar";
    }
};

TEST_CASE("value proxy test", "[unit]") {
    tsmp::value_proxy foo { foo_t{}, [](auto base, const std::string_view, auto&&... args){
        return base(std::forward<decltype(args)>(args)...);
    }};

    REQUIRE(foo.a == 0);
    REQUIRE(foo.add(5,2) == 7);
    REQUIRE(foo.inc().a == 1);

    SECTION("value can be move constructed") {
        auto foo2 { std::move(foo) };
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be move assigned") {
        auto foo2 = std::move(foo);
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be copy constructed") {
        auto foo2 { foo };
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be copy assigned") {
        auto foo2 = foo;
        REQUIRE(foo2.a == 1);
    }
}



TEST_CASE("polymorphic_value test", "[unit]") {
    tsmp::polymorphic_value<interface_t> foo { foo_impl_t{} };
    tsmp::polymorphic_value<interface_t> bar { bar_impl_t{} };

    REQUIRE(foo.print() == "foo");
    REQUIRE(bar.print() == "bar");
    
    SECTION("value can be move constructed") {
        auto foo2 { std::move(foo) };
        REQUIRE(foo2.print() == "foo");
    }

    SECTION("value can be move assigned") {
        auto foo2 = std::move(foo);
        REQUIRE(foo2.print() == "foo");
    }

    SECTION("value can be copy constructed") {
        auto foo2 { foo };
        REQUIRE(foo2.print() == "foo");
    }

    SECTION("value can be copy assigned") {
        auto foo2 = foo;
        REQUIRE(foo2.print() == "foo");
    }
}

TEST_CASE("unique proxy test", "[unit]") {
    tsmp::unique_proxy foo { foo_t{}, [](auto base, const std::string_view, auto&&... args){
        return base(std::forward<decltype(args)>(args)...);
    }};

    REQUIRE(foo.a == 0);
    REQUIRE(foo.add(5,2) == 7);
    REQUIRE(foo.inc().a == 1);

    SECTION("value can be move constructed") {
        auto foo2 { std::move(foo) };
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be move assigned") {
        auto foo2 = std::move(foo);
        REQUIRE(foo2.a == 1);
    }
}

TEST_CASE("shared proxy test", "[unit]") {
    tsmp::shared_proxy foo { foo_t{}, [](auto base, const std::string_view, auto&&... args){
        return base(std::forward<decltype(args)>(args)...);
    }};

    REQUIRE(foo.a == 0);
    REQUIRE(foo.add(5,2) == 7);
    REQUIRE(foo.inc().a == 1);

    SECTION("value can be move constructed") {
        auto foo2 { std::move(foo) };
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be move assigned") {
        auto foo2 = std::move(foo);
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be copy constructed") {
        auto foo2 { foo };
        REQUIRE(foo2.a == 1);
    }

    SECTION("value can be copy assigned") {
        auto foo2 = foo;
        REQUIRE(foo2.a == 1);
    }
}

TEST_CASE("proxy returning reference test", "[unit]") {
    auto proxy = tsmp::value_proxy{ std::array<int, 5>{} };
    proxy.front() = 4;
    REQUIRE(proxy.__tsmp_base.front() == 4);
}

struct overloaded_foo_t {
    int identity(int value) {
        return value;
    }
    
    double identity(double value) {
        return value;
    }

    std::string identity(std::string value) {
        return value;
    }
};
TEST_CASE("proxy returning from overloaded function test", "[unit]") {
    auto proxy = tsmp::value_proxy{ overloaded_foo_t{} };
    REQUIRE(proxy.identity(4) == 4);
    REQUIRE(proxy.identity("4") == "4");
    REQUIRE(proxy.identity(4.0) == 4.0);
}