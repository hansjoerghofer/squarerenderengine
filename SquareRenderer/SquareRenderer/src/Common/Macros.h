#pragma once

#include <memory>

#define DECLARE_PTRS(type) \
class type; \
typedef std::unique_ptr<type> type##UPtr; \
typedef std::shared_ptr<type> type##SPtr; \
typedef std::weak_ptr<type> type##WPtr; \

#define ASSERT(x) \
assert(x);
