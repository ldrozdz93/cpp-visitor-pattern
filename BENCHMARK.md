
##### example.cpp
```
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

```
# Benchmark
- Visitor pattern by Fedor Pikus: https://youtu.be/MdtYi0vvct0?t=880
    - uses double virtual dispatch (```vstor``` uses virtual dispatch + ```std::variant``` dispatch)
    - uses a fixed return type (```vstor``` allows arbitrary return types)
    - puts restrictions on the visitor overload set, i.e. correct order of types and no ```auto&``` types alowed (```vstor``` accepts any overload set)
    - TODO: add comparison benchmark 
- Visitor pattern by  Arthur O’Dwyer: https://quuxplusone.github.io/blog/2020/09/29/oop-visit/
    - uses sequential typeid comparison (```vstor``` uses virtual dispatch + ```std::variant``` dispatch)
    - specifies the concrete visitable types in the call site, which requires runtime error handling (```vstor``` knows the types are correct by compilation)
    - the above point makes it less intrusive (```vstor``` knows forces the usage of inheritance and CRTP)
    - TODO: add comparison benchmark
Please write to me if you know any other valuable alternatives to compare with. 