#include "tsmp/introspect.hpp"
#include "tsmp/reflect.hpp"
#include <string_view>
#include <tsmp/proxy.hpp>
#include <fmt/format.h>
#include <utility>
#include <vector>
#include <map>

#include <chrono>

struct entry_t {
    std::chrono::duration<double> duration;
    size_t calls;
};

struct trace_data_t {
    struct collector_t {
        ~collector_t() {
            const auto end = std::chrono::high_resolution_clock::now();
            data.duration += end-begin;
            ++data.calls;
        }
        entry_t& data;
        const std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    };
    
    trace_data_t(const char* class_name) : 
        class_name(class_name)
    {}

    collector_t collect(const char* name) {
        return { data[name] };
    }

    ~trace_data_t() {
        for(const auto& [name, entry] : data) {
            fmt::print("{}::{} took {}s in {} calls\n", class_name, name, entry.duration.count(), entry.calls);
        }
    }



    const char* class_name;
    std::map<const char*, entry_t> data;
};

struct tracing_functor_t {

    tracing_functor_t(const char* class_name) :
        data(class_name)
    {}

    trace_data_t data;

    template <class... Args>
    decltype(auto) operator()(auto fn, const char* name, Args&&... args) {
        const auto collector = data.collect(name);
        return (fn(std::forward<Args>(args)...));
    }
};

template <class T>
using tracing_proxy = tsmp::value_proxy<T, tracing_functor_t>;

struct Foo {
    void print() {};
};

int main(int, char*[]) {
    // tracing_proxy<std::vector<int>> test_vector;
    auto test_vector = tsmp::value_proxy{ std::vector<int>{}, tracing_functor_t{"std::vector<int>"} };
    
    std::vector<int> test;
    Foo foo;
    tsmp::introspect vector_introspecter { foo };
    vector_introspecter.invoke("print");

    for(auto i = 0u; i < 1000000u; ++i) {
        test_vector.push_back(5);
    }

    for(auto i = 0u; i < 1000000u; ++i) {
        test_vector.front() = 5;
    }

    for(auto i = 0u; i < 1000000u; ++i) {
        test_vector.at(0) = 5;
    }
    
    
        return 0;
}