#include "user2.hpp"

// clang-format off
/**
 * NOTE: Does not need to know anything about the visitables, so touching the visitable structure
 * does not cause recompilation.
 * */
int user2_value_by_virtual_calls(Base& base)
{
    return base.some_base_method();
}
// clang-format on
