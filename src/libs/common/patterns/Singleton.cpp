#include <libs/common/patterns/Singleton.h>

#include <unordered_map>
#include <cassert>
#include <string_view>

namespace common::patterns::singleton
{
    using Registry = std::unordered_map<std::size_t, const char *>;
    const Registry registry;

    size_t number_of_instances()
    {
        return 0;
    }

    void dump_registry(std::ostream &ostream, std::string_view separator)
    {
        for (auto const &[key, name] : registry)
        {
            ostream << separator << name << '\n';
        }
    }

} // namespace common::patterns::singleton
