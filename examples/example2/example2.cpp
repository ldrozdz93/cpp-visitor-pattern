#include "derived1.hpp"
#include "user1.hpp"
#include "user2.hpp"
#include "user3.hpp"

int main()
{
    Derived1 derived{};
    auto user1_result = user1_value_by_visitation(derived);
    auto user2_result = user2_value_by_virtual_calls(derived);
    auto user3_result = user3_value_by_visitation(derived);
    return 0;
}
