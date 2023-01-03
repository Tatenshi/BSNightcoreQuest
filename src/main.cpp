#include "main.hpp"

MAKE_HOOK_MATCH(GameplayCoreInstallerInstallBindingsHook, &GlobalNamespace::GameplayCoreInstaller::InstallBindings, void, GlobalNamespace::GameplayCoreInstaller *self)
{
    // Base Call
    GameplayCoreInstallerInstallBindingsHook(self);

    bool positiveSongMultiplier = (self->sceneSetupData->practiceSettings == NULL ? self->sceneSetupData->gameplayModifiers->get_songSpeedMul() : self->sceneSetupData->practiceSettings->get_songSpeedMul()) > 1;

    // First check if we have a positive multiplier or if it was allowed to run in slower mode
    // If this allows us, then we check if we are in practice mode and if yes if the change is allowed to run in practice mode
    if((positiveSongMultiplier || getModConfig().EnableInSlowerMode.GetValue()) 
        && (self->sceneSetupData->practiceSettings == NULL || getModConfig().EnableInPractice.GetValue()))
    {
        self->audioManager->set_musicPitch(1);
    }
}


void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation){
        // Make Touchable
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();

        // Create Container
        auto* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());

        // Add Options
        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Enable in practice mode", getModConfig().EnableInPractice.GetValue(), [](auto value){
            getModConfig().EnableInPractice.SetValue(value);
        });

        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Enable with negative speed modifier", getModConfig().EnableInSlowerMode.GetValue(), [](auto value){
            getModConfig().EnableInSlowerMode.SetValue(value);
        });
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo &info)
{
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load()
{
    // Init things
    getModConfig().Init(modInfo);
    il2cpp_functions::Init();
    QuestUI::Init();

    // Register our Settings
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);

    // Install Hooks
    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), GameplayCoreInstallerInstallBindingsHook);
    getLogger().info("Installed all hooks!");
}