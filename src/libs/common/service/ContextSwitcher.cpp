#include "libs/common/service/ContextSwitcher.h"

#include <libs/common/service/Registry.h>
#include <string_view>
namespace common::service::registry
{

    ContextSwitcher::ContextSwitcher(std::string_view context_name, Registry &registry) : registry_{registry}
    {
        registry_.push_context(context_name);
    }

    ContextSwitcher::~ContextSwitcher()
    {
        registry_.pop_context();
    }
}
