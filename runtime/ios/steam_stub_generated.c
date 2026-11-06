/* Généré par tools/gen_steam_stub.py depuis runtime/steam_generated.c. Ne pas modifier. */
/* Stub Steam du build iOS seulement : voir runtime/ios/steam_stub.c. */
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#define EXPORT __attribute__((visibility("default"), used))
void *steam_stub_interface(const char *version);
int32_t steam_stub_ticket(void *buffer, int32_t capacity);
EXPORT int32_t SteamAPI_ISteamAppList_GetAppBuildId(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamAppList_GetAppInstallDir(void *a0, uint32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamAppList_GetAppName(void *a0, uint32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT uint32_t SteamAPI_ISteamAppList_GetInstalledApps(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint32_t SteamAPI_ISteamAppList_GetNumInstalledApps(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamApps_BGetDLCDataByIndex(void *a0, int32_t a1, void *a2, void *a3, void *a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsAppInstalled(void *a0, uint32_t a1) { (void)a0; (void)a1; return a1 == 620u; }
EXPORT bool SteamAPI_ISteamApps_BIsCybercafe(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsDlcInstalled(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsLowViolence(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsSubscribed(void *a0) { (void)a0; return true; }
EXPORT bool SteamAPI_ISteamApps_BIsSubscribedApp(void *a0, uint32_t a1) { (void)a0; (void)a1; return a1 == 620u; }
EXPORT bool SteamAPI_ISteamApps_BIsSubscribedFromFamilySharing(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsSubscribedFromFreeWeekend(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsTimedTrial(void *a0, void *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsVACBanned(void *a0) { (void)a0; return false; }
EXPORT int32_t SteamAPI_ISteamApps_GetAppBuildId(void *a0) { (void)a0; return 0; }
EXPORT uint32_t SteamAPI_ISteamApps_GetAppInstallDir(void *a0, uint32_t a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT uint64_t SteamAPI_ISteamApps_GetAppOwner(void *a0) { (void)a0; return 76561197960265729ull; }
EXPORT const char *SteamAPI_ISteamApps_GetAvailableGameLanguages(void *a0) { (void)a0; return "english"; }
EXPORT bool SteamAPI_ISteamApps_GetCurrentBetaName(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT const char *SteamAPI_ISteamApps_GetCurrentGameLanguage(void *a0) { (void)a0; return "english"; }
EXPORT int32_t SteamAPI_ISteamApps_GetDLCCount(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamApps_GetDlcDownloadProgress(void *a0, uint32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint32_t SteamAPI_ISteamApps_GetEarliestPurchaseUnixTime(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamApps_GetFileDetails(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT uint32_t SteamAPI_ISteamApps_GetInstalledDepots(void *a0, uint32_t a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamApps_GetLaunchCommandLine(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT const char *SteamAPI_ISteamApps_GetLaunchQueryParam(void *a0, const char *a1) { (void)a0; (void)a1; return ""; }
EXPORT void SteamAPI_ISteamApps_InstallDLC(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamApps_MarkContentCorrupt(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT void SteamAPI_ISteamApps_RequestAllProofOfPurchaseKeys(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamApps_RequestAppProofOfPurchaseKey(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamApps_UninstallDLC(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamClient_BReleaseSteamPipe(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamClient_BShutdownIfAllPipesClosed(void *a0) { (void)a0; return false; }
EXPORT int32_t SteamAPI_ISteamClient_ConnectToGlobalUser(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamClient_CreateLocalUser(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamClient_CreateSteamPipe(void *a0) { (void)a0; return 0; }
EXPORT uint32_t SteamAPI_ISteamClient_GetIPCCallCount(void *a0) { (void)a0; return 0; }
EXPORT void *SteamAPI_ISteamClient_GetISteamAppList(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamApps(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamController(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamFriends(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamGameSearch(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamGameServer(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamGameServerStats(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamGenericInterface(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamHTMLSurface(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamHTTP(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamInput(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamInventory(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamMatchmaking(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamMatchmakingServers(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamMusic(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamMusicRemote(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamNetworking(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamParentalSettings(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamParties(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamRemotePlay(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamRemoteStorage(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamScreenshots(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamUGC(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamUser(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamUserStats(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void *SteamAPI_ISteamClient_GetISteamUtils(void *a0, int32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return steam_stub_interface(a2); }
EXPORT void *SteamAPI_ISteamClient_GetISteamVideo(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
EXPORT void SteamAPI_ISteamClient_ReleaseUser(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamController_ActivateActionSet(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamController_ActivateActionSetLayer(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamController_DeactivateActionSetLayer(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamController_DeactivateAllActionSetLayers(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT int32_t SteamAPI_ISteamController_GetActionOriginFromXboxOrigin(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamController_GetActionSetHandle(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamController_GetActiveActionSetLayers(void *a0, uint64_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamController_GetAnalogActionHandle(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamController_GetAnalogActionOrigins(void *a0, uint64_t a1, uint64_t a2, uint64_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT int32_t SteamAPI_ISteamController_GetConnectedControllers(void *a0, void *a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamController_GetControllerBindingRevision(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamController_GetControllerForGamepadIndex(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamController_GetCurrentActionSet(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamController_GetDigitalActionHandle(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamController_GetDigitalActionOrigins(void *a0, uint64_t a1, uint64_t a2, uint64_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT int32_t SteamAPI_ISteamController_GetGamepadIndexForController(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamController_GetGlyphForActionOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamController_GetGlyphForXboxOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT int32_t SteamAPI_ISteamController_GetInputTypeForHandle(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamController_GetStringForActionOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamController_GetStringForXboxOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT bool SteamAPI_ISteamController_Init(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamController_RunFrame(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamController_SetLEDColor(void *a0, uint64_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;  }
EXPORT bool SteamAPI_ISteamController_ShowBindingPanel(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamController_Shutdown(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamController_StopAnalogActionMomentum(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT int32_t SteamAPI_ISteamController_TranslateActionOrigin(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT void SteamAPI_ISteamController_TriggerHapticPulse(void *a0, uint64_t a1, int32_t a2, uint16_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamController_TriggerRepeatedHapticPulse(void *a0, uint64_t a1, int32_t a2, uint16_t a3, uint16_t a4, uint16_t a5, uint32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;  }
EXPORT void SteamAPI_ISteamController_TriggerVibration(void *a0, uint64_t a1, uint16_t a2, uint16_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlay(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialog(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialogConnectString(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlayRemotePlayTogetherInviteDialog(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlayToStore(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlayToUser(void *a0, const char *a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage(void *a0, const char *a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamFriends_ClearRichPresence(void *a0) { (void)a0;  }
EXPORT bool SteamAPI_ISteamFriends_CloseClanChatWindowInSteam(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamFriends_DownloadClanActivityCounts(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamFriends_EnumerateFollowingList(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetChatMemberByIndex(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamFriends_GetClanActivityCounts(void *a0, uint64_t a1, void *a2, void *a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetClanByIndex(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetClanChatMemberCount(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetClanChatMessage(void *a0, uint64_t a1, int32_t a2, void *a3, int32_t a4, void *a5, void *a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetClanCount(void *a0) { (void)a0; return 0; }
EXPORT const char *SteamAPI_ISteamFriends_GetClanName(void *a0, uint64_t a1) { (void)a0; (void)a1; return ""; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetClanOfficerByIndex(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetClanOfficerCount(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetClanOwner(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamFriends_GetClanTag(void *a0, uint64_t a1) { (void)a0; (void)a1; return ""; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetCoplayFriend(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetCoplayFriendCount(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetFollowerCount(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetFriendByIndex(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint32_t SteamAPI_ISteamFriends_GetFriendCoplayGame(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendCoplayTime(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendCount(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendCountFromSource(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamFriends_GetFriendFromSourceByIndex(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamFriends_GetFriendGamePlayed(void *a0, uint64_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendMessage(void *a0, uint64_t a1, int32_t a2, void *a3, int32_t a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
EXPORT const char *SteamAPI_ISteamFriends_GetFriendPersonaName(void *a0, uint64_t a1) { (void)a0; (void)a1; return "Player"; }
EXPORT const char *SteamAPI_ISteamFriends_GetFriendPersonaNameHistory(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendPersonaState(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendRelationship(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamFriends_GetFriendRichPresence(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT const char *SteamAPI_ISteamFriends_GetFriendRichPresenceKeyByIndex(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendRichPresenceKeyCount(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendSteamLevel(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendsGroupCount(void *a0) { (void)a0; return 0; }
EXPORT int16_t SteamAPI_ISteamFriends_GetFriendsGroupIDByIndex(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetFriendsGroupMembersCount(void *a0, int16_t a1) { (void)a0; (void)a1; return 0; }
EXPORT void SteamAPI_ISteamFriends_GetFriendsGroupMembersList(void *a0, int16_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT const char *SteamAPI_ISteamFriends_GetFriendsGroupName(void *a0, int16_t a1) { (void)a0; (void)a1; return ""; }
EXPORT int32_t SteamAPI_ISteamFriends_GetLargeFriendAvatar(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetMediumFriendAvatar(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamFriends_GetNumChatsWithUnreadPriorityMessages(void *a0) { (void)a0; return 0; }
EXPORT const char *SteamAPI_ISteamFriends_GetPersonaName(void *a0) { (void)a0; return "Player"; }
EXPORT int32_t SteamAPI_ISteamFriends_GetPersonaState(void *a0) { (void)a0; return 1; }
EXPORT const char *SteamAPI_ISteamFriends_GetPlayerNickname(void *a0, uint64_t a1) { (void)a0; (void)a1; return ""; }
EXPORT int32_t SteamAPI_ISteamFriends_GetSmallFriendAvatar(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint32_t SteamAPI_ISteamFriends_GetUserRestrictions(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamFriends_HasFriend(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamFriends_InviteUserToGame(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamFriends_IsClanChatAdmin(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamFriends_IsClanChatWindowOpenInSteam(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamFriends_IsClanOfficialGameGroup(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamFriends_IsClanPublic(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamFriends_IsFollowing(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamFriends_IsUserInSource(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamFriends_JoinClanChatRoom(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamFriends_LeaveClanChatRoom(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamFriends_OpenClanChatWindowInSteam(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamFriends_RegisterProtocolInOverlayBrowser(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamFriends_ReplyToFriendMessage(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamFriends_RequestClanOfficerList(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT void SteamAPI_ISteamFriends_RequestFriendRichPresence(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamFriends_RequestUserInformation(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamFriends_SendClanChatMessage(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT void SteamAPI_ISteamFriends_SetInGameVoiceSpeaking(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT bool SteamAPI_ISteamFriends_SetListenForFriendsMessages(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamFriends_SetPersonaName(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT void SteamAPI_ISteamFriends_SetPlayedWith(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamFriends_SetRichPresence(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT int32_t SteamAPI_ISteamGameSearch_AcceptGame(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_AddGameSearchParams(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_CancelRequestPlayersForGame(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_DeclineGame(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_EndGame(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_EndGameSearch(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_HostConfirmGameStart(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_RequestPlayersForGame(void *a0, int32_t a1, int32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_RetrieveConnectionDetails(void *a0, uint64_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_SearchForGameSolo(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_SearchForGameWithLobby(void *a0, uint64_t a1, int32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_SetConnectionDetails(void *a0, const char *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_SetGameHostParams(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamGameSearch_SubmitPlayerResult(void *a0, uint64_t a1, uint64_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT bool SteamAPI_ISteamGameServerStats_ClearUserAchievement(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamGameServerStats_GetUserAchievement(void *a0, uint64_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamGameServerStats_GetUserStatFloat(void *a0, uint64_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamGameServerStats_GetUserStatInt32(void *a0, uint64_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamGameServerStats_RequestUserStats(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamGameServerStats_SetUserAchievement(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamGameServerStats_SetUserStatFloat(void *a0, uint64_t a1, const char *a2, float a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamGameServerStats_SetUserStatInt32(void *a0, uint64_t a1, const char *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamGameServerStats_StoreUserStats(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamGameServerStats_UpdateUserAvgRateStat(void *a0, uint64_t a1, const char *a2, float a3, double a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT uint64_t SteamAPI_ISteamGameServer_AssociateWithClan(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamGameServer_BLoggedOn(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamGameServer_BSecure(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamGameServer_BUpdateUserData(void *a0, uint64_t a1, const char *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT int32_t SteamAPI_ISteamGameServer_BeginAuthSession(void *a0, void *a1, int32_t a2, uint64_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT void SteamAPI_ISteamGameServer_CancelAuthTicket(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_ClearAllKeyValues(void *a0) { (void)a0;  }
EXPORT uint64_t SteamAPI_ISteamGameServer_ComputeNewPlayerCompatibility(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamGameServer_CreateUnauthenticatedUserConnection(void *a0) { (void)a0; return 0; }
EXPORT void SteamAPI_ISteamGameServer_EndAuthSession(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT uint32_t SteamAPI_ISteamGameServer_GetAuthSessionTicket(void *a0, void *a1, int32_t a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; int32_t n = steam_stub_ticket(a1, a2); if (a3) *(uint32_t *)a3 = (uint32_t)n; return n > 0 ? 1u : 0u; }
EXPORT void SteamAPI_ISteamGameServer_GetGameplayStats(void *a0) { (void)a0;  }
EXPORT int32_t SteamAPI_ISteamGameServer_GetNextOutgoingPacket(void *a0, void *a1, int32_t a2, void *a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT uint64_t SteamAPI_ISteamGameServer_GetServerReputation(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamGameServer_GetSteamID(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamGameServer_HandleIncomingPacket(void *a0, void *a1, int32_t a2, uint32_t a3, uint16_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT void SteamAPI_ISteamGameServer_LogOff(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamGameServer_LogOn(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_LogOnAnonymous(void *a0) { (void)a0;  }
EXPORT bool SteamAPI_ISteamGameServer_RequestUserGroupStatus(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate_DEPRECATED(void *a0, uint32_t a1, void *a2, uint32_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; if (a4) *(uint64_t *)a4 = 76561197960265729ull; return true; }
EXPORT void SteamAPI_ISteamGameServer_SendUserDisconnect_DEPRECATED(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetAdvertiseServerActive(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetBotPlayerCount(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetDedicatedServer(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetGameData(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetGameDescription(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetGameTags(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetKeyValue(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamGameServer_SetMapName(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetMaxPlayerCount(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetModDir(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetPasswordProtected(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetProduct(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetRegion(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetServerName(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetSpectatorPort(void *a0, uint16_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamGameServer_SetSpectatorServerName(void *a0, const char *a1) { (void)a0; (void)a1;  }
EXPORT int32_t SteamAPI_ISteamGameServer_UserHasLicenseForApp(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamGameServer_WasRestartRequested(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamHTMLSurface_AddHeader(void *a0, uint32_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_AllowStartRequest(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_CopyToClipboard(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT uint64_t SteamAPI_ISteamHTMLSurface_CreateBrowser(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT void SteamAPI_ISteamHTMLSurface_ExecuteJavascript(void *a0, uint32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_Find(void *a0, uint32_t a1, const char *a2, bool a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4;  }
EXPORT void SteamAPI_ISteamHTMLSurface_GetLinkAtPosition(void *a0, uint32_t a1, int32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_GoBack(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_GoForward(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamHTMLSurface_Init(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamHTMLSurface_JSDialogResponse(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_KeyChar(void *a0, uint32_t a1, uint32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_KeyDown(void *a0, uint32_t a1, uint32_t a2, int32_t a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4;  }
EXPORT void SteamAPI_ISteamHTMLSurface_KeyUp(void *a0, uint32_t a1, uint32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_LoadURL(void *a0, uint32_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_MouseDoubleClick(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_MouseDown(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_MouseMove(void *a0, uint32_t a1, int32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_MouseUp(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_MouseWheel(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_OpenDeveloperTools(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_PasteFromClipboard(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_Reload(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_RemoveBrowser(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetBackgroundMode(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetCookie(void *a0, const char *a1, const char *a2, const char *a3, const char *a4, uint32_t a5, bool a6, bool a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetDPIScalingFactor(void *a0, uint32_t a1, float a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetHorizontalScroll(void *a0, uint32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetKeyFocus(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetPageScaleFactor(void *a0, uint32_t a1, float a2, int32_t a3, int32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetSize(void *a0, uint32_t a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamHTMLSurface_SetVerticalScroll(void *a0, uint32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT bool SteamAPI_ISteamHTMLSurface_Shutdown(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamHTMLSurface_StopFind(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_StopLoad(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamHTMLSurface_ViewSource(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT uint32_t SteamAPI_ISteamHTTP_CreateCookieContainer(void *a0, bool a1) { (void)a0; (void)a1; return 0; }
EXPORT uint32_t SteamAPI_ISteamHTTP_CreateHTTPRequest(void *a0, int32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamHTTP_DeferHTTPRequest(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPDownloadProgressPct(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPRequestWasTimedOut(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPResponseBodyData(void *a0, uint32_t a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPResponseBodySize(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPResponseHeaderSize(void *a0, uint32_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPResponseHeaderValue(void *a0, uint32_t a1, const char *a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamHTTP_GetHTTPStreamingResponseBodyData(void *a0, uint32_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamHTTP_PrioritizeHTTPRequest(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamHTTP_ReleaseCookieContainer(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamHTTP_ReleaseHTTPRequest(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SendHTTPRequest(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SendHTTPRequestAndStreamResponse(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetCookie(void *a0, uint32_t a1, const char *a2, const char *a3, const char *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestAbsoluteTimeoutMS(void *a0, uint32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestContextValue(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestCookieContainer(void *a0, uint32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestGetOrPostParameter(void *a0, uint32_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestHeaderValue(void *a0, uint32_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestNetworkActivityTimeout(void *a0, uint32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestRequiresVerifiedCertificate(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamHTTP_SetHTTPRequestUserAgentInfo(void *a0, uint32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT void SteamAPI_ISteamInput_ActivateActionSet(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamInput_ActivateActionSetLayer(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT bool SteamAPI_ISteamInput_BNewDataAvailable(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamInput_BWaitForData(void *a0, bool a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT void SteamAPI_ISteamInput_DeactivateActionSetLayer(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamInput_DeactivateAllActionSetLayers(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamInput_EnableDeviceCallbacks(void *a0) { (void)a0;  }
EXPORT int32_t SteamAPI_ISteamInput_GetActionOriginFromXboxOrigin(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamInput_GetActionSetHandle(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamInput_GetActiveActionSetLayers(void *a0, uint64_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamInput_GetAnalogActionHandle(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamInput_GetAnalogActionOrigins(void *a0, uint64_t a1, uint64_t a2, uint64_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT int32_t SteamAPI_ISteamInput_GetConnectedControllers(void *a0, void *a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamInput_GetControllerForGamepadIndex(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamInput_GetCurrentActionSet(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamInput_GetDeviceBindingRevision(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamInput_GetDigitalActionHandle(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamInput_GetDigitalActionOrigins(void *a0, uint64_t a1, uint64_t a2, uint64_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT int32_t SteamAPI_ISteamInput_GetGamepadIndexForController(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamInput_GetGlyphForActionOrigin_Legacy(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamInput_GetGlyphForXboxOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamInput_GetGlyphPNGForActionOrigin(void *a0, int32_t a1, int32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return ""; }
EXPORT const char *SteamAPI_ISteamInput_GetGlyphSVGForActionOrigin(void *a0, int32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT int32_t SteamAPI_ISteamInput_GetInputTypeForHandle(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint32_t SteamAPI_ISteamInput_GetRemotePlaySessionID(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint16_t SteamAPI_ISteamInput_GetSessionInputConfigurationSettings(void *a0) { (void)a0; return 0; }
EXPORT const char *SteamAPI_ISteamInput_GetStringForActionOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamInput_GetStringForAnalogActionName(void *a0, uint64_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamInput_GetStringForDigitalActionName(void *a0, uint64_t a1) { (void)a0; (void)a1; return ""; }
EXPORT const char *SteamAPI_ISteamInput_GetStringForXboxOrigin(void *a0, int32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT bool SteamAPI_ISteamInput_Init(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT void SteamAPI_ISteamInput_Legacy_TriggerHapticPulse(void *a0, uint64_t a1, int32_t a2, uint16_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamInput_Legacy_TriggerRepeatedHapticPulse(void *a0, uint64_t a1, int32_t a2, uint16_t a3, uint16_t a4, uint16_t a5, uint32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;  }
EXPORT void SteamAPI_ISteamInput_RunFrame(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamInput_SetInputActionManifestFilePath(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT void SteamAPI_ISteamInput_SetLEDColor(void *a0, uint64_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;  }
EXPORT bool SteamAPI_ISteamInput_ShowBindingPanel(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamInput_Shutdown(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamInput_StopAnalogActionMomentum(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT int32_t SteamAPI_ISteamInput_TranslateActionOrigin(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT void SteamAPI_ISteamInput_TriggerVibration(void *a0, uint64_t a1, uint16_t a2, uint16_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamInput_TriggerVibrationExtended(void *a0, uint64_t a1, uint16_t a2, uint16_t a3, uint16_t a4, uint16_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5;  }
EXPORT bool SteamAPI_ISteamInventory_AddPromoItem(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamInventory_CheckResultSteamID(void *a0, int32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamInventory_ConsumeItem(void *a0, void *a1, uint64_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamInventory_DeserializeResult(void *a0, void *a1, void *a2, uint32_t a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT void SteamAPI_ISteamInventory_DestroyResult(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamInventory_GetAllItems(void *a0, void *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamInventory_GetItemPrice(void *a0, int32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamInventory_GetItemsByID(void *a0, void *a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint32_t SteamAPI_ISteamInventory_GetNumItemsWithPrices(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamInventory_GetResultStatus(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint32_t SteamAPI_ISteamInventory_GetResultTimestamp(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamInventory_GrantPromoItems(void *a0, void *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamInventory_InspectItem(void *a0, void *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamInventory_LoadItemDefinitions(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamInventory_RemoveProperty(void *a0, uint64_t a1, uint64_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamInventory_RequestEligiblePromoItemDefinitionsIDs(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamInventory_RequestPrices(void *a0) { (void)a0; return 0; }
EXPORT void SteamAPI_ISteamInventory_SendItemDropHeartbeat(void *a0) { (void)a0;  }
EXPORT bool SteamAPI_ISteamInventory_SetPropertyBool(void *a0, uint64_t a1, uint64_t a2, const char *a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamInventory_SetPropertyFloat(void *a0, uint64_t a1, uint64_t a2, const char *a3, float a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamInventory_SetPropertyInt64(void *a0, uint64_t a1, uint64_t a2, const char *a3, int64_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamInventory_SetPropertyString(void *a0, uint64_t a1, uint64_t a2, const char *a3, const char *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT uint64_t SteamAPI_ISteamInventory_StartUpdateProperties(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamInventory_SubmitUpdateProperties(void *a0, uint64_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamInventory_TransferItemQuantity(void *a0, void *a1, uint64_t a2, uint32_t a3, uint64_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamInventory_TriggerItemDrop(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT void SteamAPI_ISteamMatchmakingServers_CancelServerQuery(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT int32_t SteamAPI_ISteamMatchmaking_AddFavoriteGame(void *a0, uint32_t a1, uint32_t a2, uint16_t a3, uint16_t a4, uint32_t a5, uint32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListCompatibleMembersFilter(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListDistanceFilter(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListFilterSlotsAvailable(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListNearValueFilter(void *a0, const char *a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListNumericalFilter(void *a0, const char *a1, int32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListResultCountFilter(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamMatchmaking_AddRequestLobbyListStringFilter(void *a0, const char *a1, const char *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT uint64_t SteamAPI_ISteamMatchmaking_CreateLobby(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamMatchmaking_DeleteLobbyData(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMatchmaking_GetFavoriteGame(void *a0, int32_t a1, void *a2, void *a3, void *a4, void *a5, void *a6, void *a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; return false; }
EXPORT int32_t SteamAPI_ISteamMatchmaking_GetFavoriteGameCount(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamMatchmaking_GetLobbyByIndex(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamMatchmaking_GetLobbyChatEntry(void *a0, uint64_t a1, int32_t a2, void *a3, void *a4, int32_t a5, void *a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
EXPORT const char *SteamAPI_ISteamMatchmaking_GetLobbyData(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT bool SteamAPI_ISteamMatchmaking_GetLobbyDataByIndex(void *a0, uint64_t a1, int32_t a2, void *a3, int32_t a4, void *a5, int32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return false; }
EXPORT int32_t SteamAPI_ISteamMatchmaking_GetLobbyDataCount(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamMatchmaking_GetLobbyGameServer(void *a0, uint64_t a1, void *a2, void *a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT uint64_t SteamAPI_ISteamMatchmaking_GetLobbyMemberByIndex(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT const char *SteamAPI_ISteamMatchmaking_GetLobbyMemberData(void *a0, uint64_t a1, uint64_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return ""; }
EXPORT int32_t SteamAPI_ISteamMatchmaking_GetLobbyMemberLimit(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamMatchmaking_GetLobbyOwner(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamMatchmaking_GetNumLobbyMembers(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamMatchmaking_InviteUserToLobby(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamMatchmaking_JoinLobby(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT void SteamAPI_ISteamMatchmaking_LeaveLobby(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamMatchmaking_RemoveFavoriteGame(void *a0, uint32_t a1, uint32_t a2, uint16_t a3, uint16_t a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamMatchmaking_RequestLobbyData(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamMatchmaking_RequestLobbyList(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamMatchmaking_SendLobbyChatMsg(void *a0, uint64_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamMatchmaking_SetLinkedLobby(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMatchmaking_SetLobbyData(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT void SteamAPI_ISteamMatchmaking_SetLobbyGameServer(void *a0, uint64_t a1, uint32_t a2, uint16_t a3, uint64_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4;  }
EXPORT bool SteamAPI_ISteamMatchmaking_SetLobbyJoinable(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT void SteamAPI_ISteamMatchmaking_SetLobbyMemberData(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT bool SteamAPI_ISteamMatchmaking_SetLobbyMemberLimit(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMatchmaking_SetLobbyOwner(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMatchmaking_SetLobbyType(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_BActivationSuccess(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_BIsCurrentMusicRemote(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_CurrentEntryDidChange(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_CurrentEntryIsAvailable(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_CurrentEntryWillChange(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_DeregisterSteamMusicRemote(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_EnableLooped(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_EnablePlayNext(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_EnablePlayPrevious(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_EnablePlaylists(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_EnableQueue(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_EnableShuffled(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_PlaylistDidChange(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_PlaylistWillChange(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_QueueDidChange(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_QueueWillChange(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_RegisterSteamMusicRemote(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_ResetPlaylistEntries(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_ResetQueueEntries(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_SetCurrentPlaylistEntry(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_SetCurrentQueueEntry(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_SetDisplayName(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_SetPNGIcon_64x64(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_SetPlaylistEntry(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_SetQueueEntry(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdateCurrentEntryCoverArt(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdateCurrentEntryElapsedSeconds(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdateCurrentEntryText(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdateLooped(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdatePlaybackStatus(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdateShuffled(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusicRemote_UpdateVolume(void *a0, float a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamMusic_BIsEnabled(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamMusic_BIsPlaying(void *a0) { (void)a0; return false; }
EXPORT int32_t SteamAPI_ISteamMusic_GetPlaybackStatus(void *a0) { (void)a0; return 0; }
EXPORT float SteamAPI_ISteamMusic_GetVolume(void *a0) { (void)a0; return 0; }
EXPORT void SteamAPI_ISteamMusic_Pause(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamMusic_Play(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamMusic_PlayNext(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamMusic_PlayPrevious(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamMusic_SetVolume(void *a0, float a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamNetworking_AllowP2PPacketRelay(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamNetworking_CloseP2PChannelWithUser(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamNetworking_CloseP2PSessionWithUser(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT uint32_t SteamAPI_ISteamNetworking_CreateP2PConnectionSocket(void *a0, uint64_t a1, int32_t a2, int32_t a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT bool SteamAPI_ISteamNetworking_DestroyListenSocket(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamNetworking_DestroySocket(void *a0, uint32_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT int32_t SteamAPI_ISteamNetworking_GetMaxPacketSize(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamNetworking_GetSocketConnectionType(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamNetworking_IsDataAvailable(void *a0, uint32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamNetworking_IsDataAvailableOnSocket(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamNetworking_IsP2PPacketAvailable(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamNetworking_ReadP2PPacket(void *a0, void *a1, uint32_t a2, void *a3, void *a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamNetworking_RetrieveData(void *a0, uint32_t a1, void *a2, uint32_t a3, void *a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamNetworking_RetrieveDataFromSocket(void *a0, uint32_t a1, void *a2, uint32_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamNetworking_SendDataOnSocket(void *a0, uint32_t a1, void *a2, uint32_t a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamNetworking_SendP2PPacket(void *a0, uint64_t a1, void *a2, uint32_t a3, int32_t a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamParentalSettings_BIsAppBlocked(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamParentalSettings_BIsAppInBlockList(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamParentalSettings_BIsFeatureBlocked(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamParentalSettings_BIsFeatureInBlockList(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamParentalSettings_BIsParentalLockEnabled(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamParentalSettings_BIsParentalLockLocked(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_BeginFileWriteBatch(void *a0) { (void)a0; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_CommitPublishedFileUpdate(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_CreatePublishedFileUpdateRequest(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_DeletePublishedFile(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_EndFileWriteBatch(void *a0) { (void)a0; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_EnumeratePublishedFilesByUserAction(void *a0, int32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_EnumerateUserPublishedFiles(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_EnumerateUserSubscribedFiles(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileDelete(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileExists(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileForget(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FilePersisted(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_FileRead(void *a0, const char *a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_FileReadAsync(void *a0, const char *a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileReadAsyncComplete(void *a0, uint64_t a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_FileShare(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileWrite(void *a0, const char *a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_FileWriteAsync(void *a0, const char *a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileWriteStreamCancel(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileWriteStreamClose(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_FileWriteStreamOpen(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_FileWriteStreamWriteChunk(void *a0, uint64_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_GetCachedUGCCount(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_GetCachedUGCHandle(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_GetFileCount(void *a0) { (void)a0; return 0; }
EXPORT const char *SteamAPI_ISteamRemoteStorage_GetFileNameAndSize(void *a0, int32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_GetFileSize(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT int64_t SteamAPI_ISteamRemoteStorage_GetFileTimestamp(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamRemoteStorage_GetLocalFileChange(void *a0, int32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return ""; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_GetLocalFileChangeCount(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_GetPublishedFileDetails(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_GetPublishedItemVoteDetails(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_GetQuota(void *a0, void *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_GetSyncPlatforms(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_GetUGCDetails(void *a0, uint64_t a1, void *a2, const char **a3, void *a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_GetUGCDownloadProgress(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_GetUserPublishedItemVoteDetails(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_IsCloudEnabledForAccount(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_IsCloudEnabledForApp(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamRemoteStorage_SetCloudEnabledForApp(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamRemoteStorage_SetSyncPlatforms(void *a0, const char *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_SetUserPublishedFileAction(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_SubscribePublishedFile(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_UGCDownload(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_UGCDownloadToLocation(void *a0, uint64_t a1, const char *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamRemoteStorage_UGCRead(void *a0, uint64_t a1, void *a2, int32_t a3, uint32_t a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_UnsubscribePublishedFile(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamRemoteStorage_UpdatePublishedFileDescription(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_UpdatePublishedFileFile(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_UpdatePublishedFilePreviewFile(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_UpdatePublishedFileSetChangeDescription(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_UpdatePublishedFileTitle(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamRemoteStorage_UpdatePublishedFileVisibility(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamRemoteStorage_UpdateUserPublishedItemVote(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint32_t SteamAPI_ISteamScreenshots_AddScreenshotToLibrary(void *a0, const char *a1, const char *a2, int32_t a3, int32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT uint32_t SteamAPI_ISteamScreenshots_AddVRScreenshotToLibrary(void *a0, int32_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT void SteamAPI_ISteamScreenshots_HookScreenshots(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamScreenshots_IsScreenshotsHooked(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamScreenshots_SetLocation(void *a0, uint32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamScreenshots_TagPublishedFile(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamScreenshots_TagUser(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT void SteamAPI_ISteamScreenshots_TriggerScreenshot(void *a0) { (void)a0;  }
EXPORT uint32_t SteamAPI_ISteamScreenshots_WriteScreenshot(void *a0, void *a1, uint32_t a2, int32_t a3, int32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_AddAppDependency(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_AddDependency(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUGC_AddExcludedTag(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_AddItemKeyValueTag(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUGC_AddItemPreviewFile(void *a0, uint64_t a1, const char *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUGC_AddItemPreviewVideo(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_AddItemToFavorites(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUGC_AddRequiredKeyValueTag(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUGC_AddRequiredTag(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_BInitWorkshopForGameServer(void *a0, uint32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_CreateItem(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_CreateQueryAllUGCRequestCursor(void *a0, int32_t a1, int32_t a2, uint32_t a3, uint32_t a4, const char *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_CreateQueryAllUGCRequestPage(void *a0, int32_t a1, int32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_CreateQueryUGCDetailsRequest(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_CreateQueryUserUGCRequest(void *a0, uint32_t a1, int32_t a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6, uint32_t a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_DeleteItem(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUGC_DownloadItem(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_GetAppDependencies(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUGC_GetItemDownloadInfo(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetItemInstallInfo(void *a0, uint64_t a1, void *a2, void *a3, uint32_t a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT uint32_t SteamAPI_ISteamUGC_GetItemState(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamUGC_GetItemUpdateProgress(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT uint32_t SteamAPI_ISteamUGC_GetNumSubscribedItems(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryFirstUGCKeyValueTag(void *a0, uint64_t a1, uint32_t a2, const char *a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCChildren(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCKeyValueTag(void *a0, uint64_t a1, uint32_t a2, uint32_t a3, void *a4, uint32_t a5, void *a6, uint32_t a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCMetadata(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT uint32_t SteamAPI_ISteamUGC_GetQueryUGCNumAdditionalPreviews(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint32_t SteamAPI_ISteamUGC_GetQueryUGCNumKeyValueTags(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint32_t SteamAPI_ISteamUGC_GetQueryUGCNumTags(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCPreviewURL(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCStatistic(void *a0, uint64_t a1, uint32_t a2, int32_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCTag(void *a0, uint64_t a1, uint32_t a2, uint32_t a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamUGC_GetQueryUGCTagDisplayName(void *a0, uint64_t a1, uint32_t a2, uint32_t a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT uint32_t SteamAPI_ISteamUGC_GetSubscribedItems(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_GetUserItemVote(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_GetWorkshopEULAStatus(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamUGC_ReleaseQueryUGCRequest(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamUGC_RemoveAllItemKeyValueTags(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_RemoveAppDependency(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_RemoveDependency(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_RemoveItemFromFavorites(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUGC_RemoveItemKeyValueTags(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_RemoveItemPreview(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_RequestUGCDetails(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_SendQueryUGCRequest(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUGC_SetAllowCachedResponse(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetAllowLegacyUpload(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetCloudFileNameFilter(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemContent(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemDescription(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemMetadata(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemPreview(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemTitle(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemUpdateLanguage(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetItemVisibility(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetLanguage(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetMatchAnyTag(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetRankedByTrendDays(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnAdditionalPreviews(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnChildren(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnKeyValueTags(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnLongDescription(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnMetadata(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnOnlyIDs(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnPlaytimeStats(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetReturnTotalOnly(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetSearchText(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetTimeCreatedDateRange(void *a0, uint64_t a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUGC_SetTimeUpdatedDateRange(void *a0, uint64_t a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_SetUserItemVote(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUGC_ShowWorkshopEULA(void *a0) { (void)a0; return false; }
EXPORT uint64_t SteamAPI_ISteamUGC_StartItemUpdate(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_StartPlaytimeTracking(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_StopPlaytimeTracking(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_StopPlaytimeTrackingForAllItems(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_SubmitItemUpdate(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUGC_SubscribeItem(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT void SteamAPI_ISteamUGC_SuspendDownloads(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT uint64_t SteamAPI_ISteamUGC_UnsubscribeItem(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUGC_UpdateItemPreviewFile(void *a0, uint64_t a1, uint32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUGC_UpdateItemPreviewVideo(void *a0, uint64_t a1, uint32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamUserStats_AttachLeaderboardUGC(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUserStats_ClearAchievement(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT uint64_t SteamAPI_ISteamUserStats_DownloadLeaderboardEntries(void *a0, uint64_t a1, int32_t a2, int32_t a3, int32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
EXPORT uint64_t SteamAPI_ISteamUserStats_DownloadLeaderboardEntriesForUsers(void *a0, uint64_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT uint64_t SteamAPI_ISteamUserStats_FindLeaderboard(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamUserStats_FindOrCreateLeaderboard(void *a0, const char *a1, int32_t a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT bool SteamAPI_ISteamUserStats_GetAchievement(void *a0, const char *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetAchievementAchievedPercent(void *a0, const char *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetAchievementAndUnlockTime(void *a0, const char *a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT const char *SteamAPI_ISteamUserStats_GetAchievementDisplayAttribute(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return ""; }
EXPORT int32_t SteamAPI_ISteamUserStats_GetAchievementIcon(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamUserStats_GetAchievementName(void *a0, uint32_t a1) { (void)a0; (void)a1; return ""; }
EXPORT bool SteamAPI_ISteamUserStats_GetAchievementProgressLimitsFloat(void *a0, const char *a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetAchievementProgressLimitsInt32(void *a0, const char *a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetDownloadedLeaderboardEntry(void *a0, uint64_t a1, int32_t a2, void *a3, void *a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetGlobalStatDouble(void *a0, const char *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT int32_t SteamAPI_ISteamUserStats_GetGlobalStatHistoryDouble(void *a0, const char *a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamUserStats_GetGlobalStatHistoryInt64(void *a0, const char *a1, void *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT bool SteamAPI_ISteamUserStats_GetGlobalStatInt64(void *a0, const char *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT int32_t SteamAPI_ISteamUserStats_GetLeaderboardDisplayType(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamUserStats_GetLeaderboardEntryCount(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT const char *SteamAPI_ISteamUserStats_GetLeaderboardName(void *a0, uint64_t a1) { (void)a0; (void)a1; return ""; }
EXPORT int32_t SteamAPI_ISteamUserStats_GetLeaderboardSortMethod(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint32_t SteamAPI_ISteamUserStats_GetNumAchievements(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamUserStats_GetNumberOfCurrentPlayers(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamUserStats_GetStatFloat(void *a0, const char *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetStatInt32(void *a0, const char *a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetUserAchievement(void *a0, uint64_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetUserAchievementAndUnlockTime(void *a0, uint64_t a1, const char *a2, void *a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetUserStatFloat(void *a0, uint64_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUserStats_GetUserStatInt32(void *a0, uint64_t a1, const char *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUserStats_IndicateAchievementProgress(void *a0, const char *a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUserStats_RequestCurrentStats(void *a0) { (void)a0; return false; }
EXPORT uint64_t SteamAPI_ISteamUserStats_RequestGlobalAchievementPercentages(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamUserStats_RequestGlobalStats(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT uint64_t SteamAPI_ISteamUserStats_RequestUserStats(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUserStats_ResetAllStats(void *a0, bool a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamUserStats_SetAchievement(void *a0, const char *a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamUserStats_SetStatFloat(void *a0, const char *a1, float a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUserStats_SetStatInt32(void *a0, const char *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUserStats_StoreStats(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUserStats_UpdateAvgRateStat(void *a0, const char *a1, float a2, double a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint64_t SteamAPI_ISteamUserStats_UploadLeaderboardScore(void *a0, uint64_t a1, int32_t a2, int32_t a3, void *a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
EXPORT void SteamAPI_ISteamUser_AdvertiseGame(void *a0, uint64_t a1, uint32_t a2, uint16_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
EXPORT bool SteamAPI_ISteamUser_BIsBehindNAT(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUser_BIsPhoneIdentifying(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUser_BIsPhoneRequiringVerification(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUser_BIsPhoneVerified(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUser_BIsTwoFactorEnabled(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUser_BLoggedOn(void *a0) { (void)a0; return true; }
EXPORT bool SteamAPI_ISteamUser_BSetDurationControlOnlineState(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
EXPORT int32_t SteamAPI_ISteamUser_BeginAuthSession(void *a0, void *a1, int32_t a2, uint64_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT void SteamAPI_ISteamUser_CancelAuthTicket(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT int32_t SteamAPI_ISteamUser_DecompressVoice(void *a0, void *a1, uint32_t a2, void *a3, uint32_t a4, void *a5, uint32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
EXPORT void SteamAPI_ISteamUser_EndAuthSession(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
EXPORT uint32_t SteamAPI_ISteamUser_GetAuthSessionTicket(void *a0, void *a1, int32_t a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; int32_t n = steam_stub_ticket(a1, a2); if (a3) *(uint32_t *)a3 = (uint32_t)n; return n > 0 ? 1u : 0u; }
EXPORT uint64_t SteamAPI_ISteamUser_GetDurationControl(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamUser_GetEncryptedAppTicket(void *a0, void *a1, int32_t a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT int32_t SteamAPI_ISteamUser_GetGameBadgeLevel(void *a0, int32_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT int32_t SteamAPI_ISteamUser_GetHSteamUser(void *a0) { (void)a0; return 1; }
EXPORT uint64_t SteamAPI_ISteamUser_GetMarketEligibility(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamUser_GetPlayerSteamLevel(void *a0) { (void)a0; return 0; }
EXPORT uint64_t SteamAPI_ISteamUser_GetSteamID(void *a0) { (void)a0; return 76561197960265729ull; }
EXPORT bool SteamAPI_ISteamUser_GetUserDataFolder(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint32_t SteamAPI_ISteamUser_GetVoiceOptimalSampleRate(void *a0) { (void)a0; return 0; }
EXPORT int32_t SteamAPI_ISteamUser_InitiateGameConnection_DEPRECATED(void *a0, void *a1, int32_t a2, uint64_t a3, uint32_t a4, uint16_t a5, bool a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return steam_stub_ticket(a1, a2); }
EXPORT uint64_t SteamAPI_ISteamUser_RequestEncryptedAppTicket(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint64_t SteamAPI_ISteamUser_RequestStoreAuthURL(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT void SteamAPI_ISteamUser_StartVoiceRecording(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamUser_StopVoiceRecording(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamUser_TerminateGameConnection_DEPRECATED(void *a0, uint32_t a1, uint16_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT int32_t SteamAPI_ISteamUser_UserHasLicenseForApp(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT bool SteamAPI_ISteamUtils_BOverlayNeedsPresent(void *a0) { (void)a0; return false; }
EXPORT uint64_t SteamAPI_ISteamUtils_CheckFileSignature(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUtils_DismissFloatingGamepadTextInput(void *a0) { (void)a0; return false; }
EXPORT int32_t SteamAPI_ISteamUtils_FilterText(void *a0, int32_t a1, uint64_t a2, const char *a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
EXPORT int32_t SteamAPI_ISteamUtils_GetAPICallFailureReason(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUtils_GetAPICallResult(void *a0, uint64_t a1, void *a2, int32_t a3, int32_t a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT uint32_t SteamAPI_ISteamUtils_GetAppID(void *a0) { (void)a0; return 620u; }
EXPORT int32_t SteamAPI_ISteamUtils_GetConnectedUniverse(void *a0) { (void)a0; return 1; }
EXPORT uint8_t SteamAPI_ISteamUtils_GetCurrentBatteryPower(void *a0) { (void)a0; return 255; }
EXPORT bool SteamAPI_ISteamUtils_GetEnteredGamepadTextInput(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT uint32_t SteamAPI_ISteamUtils_GetEnteredGamepadTextLength(void *a0) { (void)a0; return 0; }
EXPORT uint32_t SteamAPI_ISteamUtils_GetIPCCallCount(void *a0) { (void)a0; return 0; }
EXPORT const char *SteamAPI_ISteamUtils_GetIPCountry(void *a0) { (void)a0; return "FR"; }
EXPORT int32_t SteamAPI_ISteamUtils_GetIPv6ConnectivityState(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamUtils_GetImageRGBA(void *a0, int32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT bool SteamAPI_ISteamUtils_GetImageSize(void *a0, int32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
EXPORT uint32_t SteamAPI_ISteamUtils_GetSecondsSinceAppActive(void *a0) { (void)a0; return 0; }
EXPORT uint32_t SteamAPI_ISteamUtils_GetSecondsSinceComputerActive(void *a0) { (void)a0; return 0; }
EXPORT uint32_t SteamAPI_ISteamUtils_GetServerRealTime(void *a0) { (void)a0; return (uint32_t)time(NULL); }
EXPORT const char *SteamAPI_ISteamUtils_GetSteamUILanguage(void *a0) { (void)a0; return "english"; }
EXPORT bool SteamAPI_ISteamUtils_InitFilterText(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsAPICallCompleted(void *a0, uint64_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsOverlayEnabled(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsSteamChinaLauncher(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsSteamInBigPictureMode(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsSteamRunningInVR(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsSteamRunningOnSteamDeck(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamUtils_IsVRHeadsetStreamingEnabled(void *a0) { (void)a0; return false; }
EXPORT void SteamAPI_ISteamUtils_SetGameLauncherMode(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamUtils_SetOverlayNotificationInset(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamUtils_SetOverlayNotificationPosition(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamUtils_SetVRHeadsetStreamingEnabled(void *a0, bool a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamUtils_ShowFloatingGamepadTextInput(void *a0, int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamUtils_ShowGamepadTextInput(void *a0, int32_t a1, int32_t a2, const char *a3, uint32_t a4, const char *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT void SteamAPI_ISteamUtils_StartVRDashboard(void *a0) { (void)a0;  }
EXPORT void SteamAPI_ISteamVideo_GetOPFSettings(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamVideo_GetVideoURL(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
EXPORT bool SteamAPI_ISteamVideo_IsBroadcasting(void *a0, void *a1) { (void)a0; (void)a1; return false; }
#endif
