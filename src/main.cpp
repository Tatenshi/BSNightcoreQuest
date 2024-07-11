#include "main.hpp"

MAKE_HOOK_MATCH(GameplayCoreInstallerInstallBindingsHook, &GlobalNamespace::GameplayCoreInstaller::InstallBindings, void, GlobalNamespace::GameplayCoreInstaller *self)
{
    // Base Call
    GameplayCoreInstallerInstallBindingsHook(self);

    bool positiveSongMultiplier = (self->_sceneSetupData->practiceSettings == NULL ? self->_sceneSetupData->gameplayModifiers->get_songSpeedMul() : self->_sceneSetupData->practiceSettings->get_songSpeedMul()) > 1;

    // First check if we have a positive multiplier or if it was allowed to run in slower mode
    // If this allows us, then we check if we are in practice mode and if yes if the change is allowed to run in practice mode
    if((positiveSongMultiplier || getModConfig().EnableInSlowerMode.GetValue()) 
        && (self->_sceneSetupData->practiceSettings == NULL || getModConfig().EnableInPractice.GetValue()))
    {
        self->_audioManager->set_musicPitch(1);
    }
}


void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation){
        // Make Touchable
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();

        // Create Container
        auto* container = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform());
        
        // Add Options
        AddConfigValueToggle(container->get_transform(), getModConfig().EnableInPractice);
        AddConfigValueToggle(container->get_transform(), getModConfig().EnableInSlowerMode);
    }
}

// Called at the early stages of game loading
extern "C" __attribute__((visibility("default"))) void setup(CModInfo* info)
{
    info->version = VERSION;
    info->id = MOD_ID;
    info->version_long = 0;
    modInfo.assign(*info);

    getModConfig().Init(modInfo);
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" __attribute__((visibility("default"))) void late_load()
{
    // Init things
    getModConfig().Init(modInfo);
    BSML::Init();

    // Register our Settings
    BSML::Register::RegisterSettingsMenu("BSNightcoreQuest", DidActivate, true);

    // Install Hooks
    auto logger = Paper::ConstLoggerContext("BSNightcoreQuest");
    getLogger().info("Installing hooks...");
    INSTALL_HOOK(logger, GameplayCoreInstallerInstallBindingsHook);
    getLogger().info("Installed all hooks!");
}