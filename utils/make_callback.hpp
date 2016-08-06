#pragma once
#include "force_cast.hpp"

template<typename Class, typename Ret, typename... Args>
Ret(*make_callback(Ret(Class::*source)(Args...)))(void *, Args...) {
    return force_cast<Ret(*)(void *, Args...), Ret(Class::*)(Args...)>(source);
}
