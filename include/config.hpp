#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(EnableInPractice, bool, "Enabled in practice mode", true);
    CONFIG_VALUE(EnableInSlowerMode, bool, "Enable with negative speed modifier", true);
)