#include <string>
#include <type_traits>
#include <vector>
#include <set>
#include <unordered_set>

#include "sltbench/Bench.h"
#include "visitor_odwyer.hpp"
#include "vstor/vstor.hpp"

namespace {

using vstor::Overloaded;

static const std::vector<size_t> insert_to_set_args{ 1000, 10000, 100000, 1000000 };

static void InsertToSetSorted(const size_t& count)
{
    std::set< size_t > s;
    for (size_t i = 0; i < count; ++i)
        s.insert(i);
}
SLTBENCH_FUNCTION_WITH_ARGS(InsertToSetSorted, insert_to_set_args);

static void InsertToSetUnordered(const size_t& count)
{
    std::unordered_set<size_t> s;
    s.reserve(count);
    for (size_t i = 0; i < count; ++i)
        s.insert(i);
}
SLTBENCH_FUNCTION_WITH_ARGS(InsertToSetUnordered, insert_to_set_args);


}  // namespace