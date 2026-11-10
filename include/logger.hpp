#pragma once

#include "paper2_scotland2/shared/logger.hpp"
#include "config-utils/shared/config-utils.hpp"

// Static Variables
inline modloader::ModInfo modInfo = {MOD_ID, VERSION, 0}; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Define these functions here so that we can easily read configuration and log information from other files
Paper::ConstLoggerContext<17UL> getLogger();