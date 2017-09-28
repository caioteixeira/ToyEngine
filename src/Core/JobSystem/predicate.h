// jobxx - C++ lightweight task library.
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//
// Authors:
//   Sean Middleditch <sean.middleditch@gmail.com>

#if !defined(_guard_JOBXX_PREDICATE_H)
#define _guard_JOBXX_PREDICATE_H
#pragma once

#include <utility>

namespace jobxx
{

    class predicate
    {
    public:
        predicate() = default;

        template <typename FunctionT, typename = std::enable_if_t<!std::is_same_v<std::remove_const_t<std::remove_reference_t<FunctionT>>, predicate>>>
        /*implicit*/ predicate(FunctionT&& func) { _assign(std::forward<FunctionT>(func)); }

        explicit operator bool() const { return _thunk != nullptr; }

        bool operator()() { return _thunk(_view); }

    private:
        template <typename FunctionT> inline void _assign(FunctionT&& func);

        bool(*_thunk)(void*) = nullptr;
        void* _view = nullptr;
    };

    template <typename FunctionT>
    void predicate::_assign(FunctionT&& func)
    {
        using func_type = std::remove_reference_t<FunctionT>;
        _thunk = [](void* view){ return (*static_cast<func_type*>(view))(); };
        _view = &func;
    }

}

#endif // defined(_guard_JOBXX_PREDICATE_H)
