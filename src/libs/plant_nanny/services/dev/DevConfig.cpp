#include "libs/plant_nanny/services/dev/DevConfig.h"
#include "libs/plant_nanny/services/dev/DebugConfigStrategy.h"
#include "libs/plant_nanny/services/dev/ReleaseConfigStrategy.h"

namespace plant_nanny::services::dev
{

std::unique_ptr<IDevConfigStrategy> DevConfig::createStrategy()
{
#ifdef DEBUG
    return std::make_unique<DebugConfigStrategy>();
#else
    return std::make_unique<ReleaseConfigStrategy>();
#endif
}

void DevConfig::apply(config::IConfigManager& configManager)
{
    auto strategy = createStrategy();
    strategy->apply(configManager);
}

void DevConfig::logStatus()
{
    auto strategy = createStrategy();
    strategy->logStatus();
}

} // namespace plant_nanny::services::dev
