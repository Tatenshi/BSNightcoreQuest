#include "main.hpp"

#include "GlobalNamespace/ResetPitchOnGameplayFinished.hpp"
#include "UnityEngine/Audio/AudioMixer.hpp"

SafePtrUnity<GlobalNamespace::AudioManagerSO> audioManager;

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

MAKE_HOOK_MATCH(MainInit, &GlobalNamespace::SettingsApplicatorSO::ApplyGameSettings, void, GlobalNamespace::SettingsApplicatorSO* self, ByRef<BeatSaber::Settings::Settings> settings) {
    MainInit(self, settings);

    audioManager = self->_audioManager;
}

MAKE_HOOK_MATCH(RefreshMultipliers, &GlobalNamespace::GameplayModifiersPanelController::RefreshTotalMultiplierAndRankUI, void, GlobalNamespace::GameplayModifiersPanelController* self) {
    RefreshMultipliers(self);

    SetPitchSpeed(self);
}

MAKE_HOOK_MATCH(GameplayModifiersPanelAwake, &GlobalNamespace::GameplaySetupViewController::DidActivate, void, GlobalNamespace::GameplaySetupViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    GameplayModifiersPanelAwake(self, firstActivation, addedToHierarchy, screenSystemEnabling);

    SetPitchSpeed(self->_gameplayModifiersPanelController);
}

void SetPitchSpeed(GlobalNamespace::GameplayModifiersPanelController* self){
    if(getModConfig().EnableInMenu.GetValue())
    {
        float multiplier = self->_gameplayModifiers->get_songSpeedMul();
        if (multiplier < 1 && !getModConfig().EnableInSlowerMode.GetValue()) 
        {
            multiplier = 1;
        }
        audioManager->set_musicSpeed(multiplier);
        audioManager->set_musicPitch(1);
    }
}

MAKE_HOOK_MATCH(DisableGameplaySetupViewController, &GlobalNamespace::GameplaySetupViewController::OnDisable, void, GlobalNamespace::GameplaySetupViewController* self)
{
    DisableGameplaySetupViewController(self);
    audioManager->set_musicSpeed(1);
}

static std::map<std::string,float> oldPreviewDurations {};

MAKE_HOOK_MATCH(SongPlayerCrossFade, &GlobalNamespace::LevelCollectionViewController::SongPlayerCrossfadeToLevel, void, GlobalNamespace::LevelCollectionViewController* self, GlobalNamespace::BeatmapLevel* level) {
    float oldDuration = level->previewDuration;

    if (getModConfig().EnableUnlimitedPreviews.GetValue()) 
    {
        // -1 means making the audioclip unlimited
        oldPreviewDurations.insert_or_assign(level->levelID, oldDuration);
        level->previewDuration=-1;
    }

    SongPlayerCrossFade(self, level);
}

MAKE_HOOK_MATCH(Unload, &GlobalNamespace::LevelCollectionViewController::UnloadPreviewAudioClip, void, GlobalNamespace::LevelCollectionViewController* self, GlobalNamespace::BeatmapLevel* level)
{
    if(oldPreviewDurations.contains(level->levelID)) {
        level->previewDuration=oldPreviewDurations[level->levelID];
    }
    Unload(self, level);
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
        AddConfigValueToggle(container->get_transform(), getModConfig().EnableInMenu);
        AddConfigValueToggle(container->get_transform(), getModConfig().EnableUnlimitedPreviews);
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
    INSTALL_HOOK(logger, RefreshMultipliers);
    INSTALL_HOOK(logger, MainInit);
    INSTALL_HOOK(logger, SongPlayerCrossFade);
    INSTALL_HOOK(logger, Unload);
    INSTALL_HOOK(logger, DisableGameplaySetupViewController);
    INSTALL_HOOK(logger, GameplayModifiersPanelAwake);
    getLogger().info("Installed all hooks!");
}