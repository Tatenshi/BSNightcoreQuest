#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(EnableInPractice, bool, "Enabled in Practice Mode", true);
    CONFIG_VALUE(EnableInSlowerMode, bool, "Enabled with negative Modifier", true);
)