#include "Context.h"

#include <string_view>
#include <memory>

namespace common::service
{
    Context::Context(std::string_view name) noexcept : name_{std::move(name)} {}
}