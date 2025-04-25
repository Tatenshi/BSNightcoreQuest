#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig) {
    CONFIG_VALUE(EnableInPractice, bool, "Enabled in practice mode", true);
    CONFIG_VALUE(EnableInSlowerMode, bool, "Enable with negative speed modifier", true);
    CONFIG_VALUE(EnableInMenu, bool, "Enable in song preview (menu)", true);
    CONFIG_VALUE(EnableUnlimitedPreviews, bool, "Make previews unlimited", true);
};