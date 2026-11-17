#pragma once
// Test executables only. Injection is confined to the calling thread and scope.
#include <cstdlib>
#include <new>
#include <cstddef>
namespace e2e_test_alloc {
thread_local long remaining = -1;
thread_local unsigned failures = 0;
struct Scope {
    explicit Scope(long count) { remaining = count; failures = 0; }
    ~Scope() { remaining = -1; }
};
}
void* operator new(std::size_t size) {
    if (e2e_test_alloc::remaining == 0) {
        ++e2e_test_alloc::failures;
        throw std::bad_alloc();
    }
    if (e2e_test_alloc::remaining > 0) --e2e_test_alloc::remaining;
    if (void* value = std::malloc(size ? size : 1)) return value;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* value) noexcept { std::free(value); }
void operator delete[](void* value) noexcept { std::free(value); }
void operator delete(void* value, std::size_t) noexcept { std::free(value); }
void operator delete[](void* value, std::size_t) noexcept { std::free(value); }
