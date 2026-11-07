/* Généré par tools/gen_steam_stub.py depuis runtime/steam_generated.c. Ne pas modifier. */
/* Stub Steam du build iOS seulement : voir runtime/ios/steam_stub.c. */
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#define EXPORT __attribute__((visibility("default"), used))
void *steam_stub_interface(const char *version);
int32_t steam_stub_ticket(void *buffer, int32_t capacity);
uint32_t steam_stub_app_id(void);
EXPORT int32_t SteamAPI_ISteamAppList_GetAppBuildId(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
EXPORT int32_t SteamAPI_ISteamAppList_GetAppInstallDir(void *a0, uint32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT int32_t SteamAPI_ISteamAppList_GetAppName(void *a0, uint32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
EXPORT uint32_t SteamAPI_ISteamAppList_GetInstalledApps(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
EXPORT uint32_t SteamAPI_ISteamAppList_GetNumInstalledApps(void *a0) { (void)a0; return 0; }
EXPORT bool SteamAPI_ISteamApps_BGetDLCDataByIndex(void *a0, int32_t a1, void *a2, void *a3, void *a4, int32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsAppInstalled(void *a0, uint32_t a1) { (void)a0; (void)a1; return a1 == steam_stub_app_id(); }
EXPORT bool SteamAPI_ISteamApps_BIsCybercafe(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsDlcInstalled(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsLowViolence(void *a0) { (void)a0; return false; }
EXPORT bool SteamAPI_ISteamApps_BIsSubscribed(void *a0) { (void)a0; return true; }
EXPORT bool SteamAPI_ISteamApps_BIsSubscribedApp(void *a0, uint32_t a1) { (void)a0; (void)a1; return a1 == steam_stub_app_id(); }
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
EXPORT bool SteamAPI_ISteamInput_Init(void *a0, bool a1) { (void)a0; (void)a1; return true; }
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
EXPORT void SteamAPI_ISteamMatchmakingServers_CancelQuery(void *a0, void *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamMatchmakingServers_CancelServerQuery(void *a0, int32_t a1) { (void)a0; (void)a1;  }
EXPORT int32_t SteamAPI_ISteamMatchmakingServers_GetServerCount(void *a0, void *a1) { (void)a0; (void)a1; return 0; }
EXPORT bool SteamAPI_ISteamMatchmakingServers_IsRefreshing(void *a0, void *a1) { (void)a0; (void)a1; return false; }
EXPORT void SteamAPI_ISteamMatchmakingServers_RefreshQuery(void *a0, void *a1) { (void)a0; (void)a1;  }
EXPORT void SteamAPI_ISteamMatchmakingServers_RefreshServer(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
EXPORT void SteamAPI_ISteamMatchmakingServers_ReleaseRequest(void *a0, void *a1) { (void)a0; (void)a1;  }
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
EXPORT uint32_t SteamAPI_ISteamUtils_GetAppID(void *a0) { (void)a0; return steam_stub_app_id(); }
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

/* Interfaces appelées par emplacement de vtable (SDK récents). */
static unsigned long long zero_slot(void) { return 0; }
static uint32_t v_STEAMREMOTEPLAY_INTERFACE_VERSION002_0(void *a0) { (void)a0; return 0; }
static uint32_t v_STEAMREMOTEPLAY_INTERFACE_VERSION002_1(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
static uint64_t v_STEAMREMOTEPLAY_INTERFACE_VERSION002_2(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static const char *v_STEAMREMOTEPLAY_INTERFACE_VERSION002_3(void *a0, uint32_t a1) { (void)a0; (void)a1; return ""; }
static int32_t v_STEAMREMOTEPLAY_INTERFACE_VERSION002_4(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static bool v_STEAMREMOTEPLAY_INTERFACE_VERSION002_5(void *a0, uint32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMREMOTEPLAY_INTERFACE_VERSION002_6(void *a0, bool a1) { (void)a0; (void)a1; return false; }
static bool v_STEAMREMOTEPLAY_INTERFACE_VERSION002_7(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
static void *vt_STEAMREMOTEPLAY_INTERFACE_VERSION002[8] = {(void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_0, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_1, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_2, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_3, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_4, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_5, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_6, (void *)v_STEAMREMOTEPLAY_INTERFACE_VERSION002_7};
static void v_STEAMTIMELINE_INTERFACE_V004_0(void *a0, const char *a1, float a2) { (void)a0; (void)a1; (void)a2;  }
static void v_STEAMTIMELINE_INTERFACE_V004_1(void *a0, float a1) { (void)a0; (void)a1;  }
static void v_STEAMTIMELINE_INTERFACE_V004_2(void *a0, int32_t a1) { (void)a0; (void)a1;  }
static uint64_t v_STEAMTIMELINE_INTERFACE_V004_3(void *a0, const char *a1, const char *a2, const char *a3, uint32_t a4, float a5, int32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
static uint64_t v_STEAMTIMELINE_INTERFACE_V004_4(void *a0, const char *a1, const char *a2, const char *a3, uint32_t a4, float a5, float a6, int32_t a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; return 0; }
static uint64_t v_STEAMTIMELINE_INTERFACE_V004_5(void *a0, const char *a1, const char *a2, const char *a3, uint32_t a4, float a5, int32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
static void v_STEAMTIMELINE_INTERFACE_V004_6(void *a0, uint64_t a1, const char *a2, const char *a3, const char *a4, uint32_t a5, int32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6;  }
static void v_STEAMTIMELINE_INTERFACE_V004_7(void *a0, uint64_t a1, float a2) { (void)a0; (void)a1; (void)a2;  }
static void v_STEAMTIMELINE_INTERFACE_V004_8(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
static uint64_t v_STEAMTIMELINE_INTERFACE_V004_9(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static void v_STEAMTIMELINE_INTERFACE_V004_10(void *a0) { (void)a0;  }
static void v_STEAMTIMELINE_INTERFACE_V004_11(void *a0) { (void)a0;  }
static void v_STEAMTIMELINE_INTERFACE_V004_12(void *a0, const char *a1) { (void)a0; (void)a1;  }
static uint64_t v_STEAMTIMELINE_INTERFACE_V004_13(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
static void v_STEAMTIMELINE_INTERFACE_V004_14(void *a0, const char *a1, const char *a2, const char *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4;  }
static void v_STEAMTIMELINE_INTERFACE_V004_15(void *a0, const char *a1, const char *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
static void v_STEAMTIMELINE_INTERFACE_V004_16(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_STEAMTIMELINE_INTERFACE_V004_17(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
static void *vt_STEAMTIMELINE_INTERFACE_V004[18] = {(void *)v_STEAMTIMELINE_INTERFACE_V004_0, (void *)v_STEAMTIMELINE_INTERFACE_V004_1, (void *)v_STEAMTIMELINE_INTERFACE_V004_2, (void *)v_STEAMTIMELINE_INTERFACE_V004_3, (void *)v_STEAMTIMELINE_INTERFACE_V004_4, (void *)v_STEAMTIMELINE_INTERFACE_V004_5, (void *)v_STEAMTIMELINE_INTERFACE_V004_6, (void *)v_STEAMTIMELINE_INTERFACE_V004_7, (void *)v_STEAMTIMELINE_INTERFACE_V004_8, (void *)v_STEAMTIMELINE_INTERFACE_V004_9, (void *)v_STEAMTIMELINE_INTERFACE_V004_10, (void *)v_STEAMTIMELINE_INTERFACE_V004_11, (void *)v_STEAMTIMELINE_INTERFACE_V004_12, (void *)v_STEAMTIMELINE_INTERFACE_V004_13, (void *)v_STEAMTIMELINE_INTERFACE_V004_14, (void *)v_STEAMTIMELINE_INTERFACE_V004_15, (void *)v_STEAMTIMELINE_INTERFACE_V004_16, (void *)v_STEAMTIMELINE_INTERFACE_V004_17};
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_0(void *a0, uint32_t a1, int32_t a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6, uint32_t a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_1(void *a0, int32_t a1, int32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_2(void *a0, int32_t a1, int32_t a2, uint32_t a3, uint32_t a4, const char *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_3(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_4(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_6(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_STEAMUGC_INTERFACE_VERSION020_7(void *a0, uint64_t a1, uint32_t a2, uint32_t a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_8(void *a0, uint64_t a1, uint32_t a2, uint32_t a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_9(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_10(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_11(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_12(void *a0, uint64_t a1, uint32_t a2, int32_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_13(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_15(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_STEAMUGC_INTERFACE_VERSION020_16(void *a0, uint64_t a1, uint32_t a2, uint32_t a3, void *a4, uint32_t a5, void *a6, uint32_t a7) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_17(void *a0, uint64_t a1, uint32_t a2, const char *a3, void *a4, uint32_t a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_18(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_20(void *a0, uint64_t a1, uint32_t a2, void *a3, uint32_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
static bool v_STEAMUGC_INTERFACE_VERSION020_21(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_22(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_24(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_25(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_26(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_27(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_28(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_29(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_30(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_31(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_32(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_33(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_34(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_35(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_36(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_37(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_38(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_39(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_40(void *a0, uint64_t a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_41(void *a0, uint64_t a1, uint32_t a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_42(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_43(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_44(void *a0, uint32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_45(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_STEAMUGC_INTERFACE_VERSION020_46(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_47(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_48(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_49(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_50(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_52(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_53(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_54(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_55(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_56(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_57(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_58(void *a0, uint64_t a1, const char *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_59(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_60(void *a0, uint64_t a1, uint32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_61(void *a0, uint64_t a1, uint32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_62(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_63(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_64(void *a0, uint64_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_65(void *a0, uint64_t a1, const char *a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_66(void *a0, uint64_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return 0; }
static int32_t v_STEAMUGC_INTERFACE_VERSION020_67(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_68(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_69(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_70(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_71(void *a0, uint32_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_72(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_73(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_74(void *a0) { (void)a0; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_75(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_76(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static bool v_STEAMUGC_INTERFACE_VERSION020_77(void *a0, uint64_t a1, void *a2, void *a3, uint32_t a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_78(void *a0, uint64_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_79(void *a0, uint64_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return false; }
static bool v_STEAMUGC_INTERFACE_VERSION020_80(void *a0, uint32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static void v_STEAMUGC_INTERFACE_VERSION020_81(void *a0, bool a1) { (void)a0; (void)a1;  }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_82(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_83(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_84(void *a0) { (void)a0; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_85(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_86(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_87(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_88(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_89(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_90(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static bool v_STEAMUGC_INTERFACE_VERSION020_91(void *a0) { (void)a0; return false; }
static uint64_t v_STEAMUGC_INTERFACE_VERSION020_92(void *a0) { (void)a0; return 0; }
static uint32_t v_STEAMUGC_INTERFACE_VERSION020_93(void *a0, void *a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static void *vt_STEAMUGC_INTERFACE_VERSION020[94] = {(void *)v_STEAMUGC_INTERFACE_VERSION020_0, (void *)v_STEAMUGC_INTERFACE_VERSION020_1, (void *)v_STEAMUGC_INTERFACE_VERSION020_2, (void *)v_STEAMUGC_INTERFACE_VERSION020_3, (void *)v_STEAMUGC_INTERFACE_VERSION020_4, (void *)zero_slot, (void *)v_STEAMUGC_INTERFACE_VERSION020_6, (void *)v_STEAMUGC_INTERFACE_VERSION020_7, (void *)v_STEAMUGC_INTERFACE_VERSION020_8, (void *)v_STEAMUGC_INTERFACE_VERSION020_9, (void *)v_STEAMUGC_INTERFACE_VERSION020_10, (void *)v_STEAMUGC_INTERFACE_VERSION020_11, (void *)v_STEAMUGC_INTERFACE_VERSION020_12, (void *)v_STEAMUGC_INTERFACE_VERSION020_13, (void *)zero_slot, (void *)v_STEAMUGC_INTERFACE_VERSION020_15, (void *)v_STEAMUGC_INTERFACE_VERSION020_16, (void *)v_STEAMUGC_INTERFACE_VERSION020_17, (void *)v_STEAMUGC_INTERFACE_VERSION020_18, (void *)zero_slot, (void *)v_STEAMUGC_INTERFACE_VERSION020_20, (void *)v_STEAMUGC_INTERFACE_VERSION020_21, (void *)v_STEAMUGC_INTERFACE_VERSION020_22, (void *)zero_slot, (void *)v_STEAMUGC_INTERFACE_VERSION020_24, (void *)v_STEAMUGC_INTERFACE_VERSION020_25, (void *)v_STEAMUGC_INTERFACE_VERSION020_26, (void *)v_STEAMUGC_INTERFACE_VERSION020_27, (void *)v_STEAMUGC_INTERFACE_VERSION020_28, (void *)v_STEAMUGC_INTERFACE_VERSION020_29, (void *)v_STEAMUGC_INTERFACE_VERSION020_30, (void *)v_STEAMUGC_INTERFACE_VERSION020_31, (void *)v_STEAMUGC_INTERFACE_VERSION020_32, (void *)v_STEAMUGC_INTERFACE_VERSION020_33, (void *)v_STEAMUGC_INTERFACE_VERSION020_34, (void *)v_STEAMUGC_INTERFACE_VERSION020_35, (void *)v_STEAMUGC_INTERFACE_VERSION020_36, (void *)v_STEAMUGC_INTERFACE_VERSION020_37, (void *)v_STEAMUGC_INTERFACE_VERSION020_38, (void *)v_STEAMUGC_INTERFACE_VERSION020_39, (void *)v_STEAMUGC_INTERFACE_VERSION020_40, (void *)v_STEAMUGC_INTERFACE_VERSION020_41, (void *)v_STEAMUGC_INTERFACE_VERSION020_42, (void *)v_STEAMUGC_INTERFACE_VERSION020_43, (void *)v_STEAMUGC_INTERFACE_VERSION020_44, (void *)v_STEAMUGC_INTERFACE_VERSION020_45, (void *)v_STEAMUGC_INTERFACE_VERSION020_46, (void *)v_STEAMUGC_INTERFACE_VERSION020_47, (void *)v_STEAMUGC_INTERFACE_VERSION020_48, (void *)v_STEAMUGC_INTERFACE_VERSION020_49, (void *)v_STEAMUGC_INTERFACE_VERSION020_50, (void *)zero_slot, (void *)v_STEAMUGC_INTERFACE_VERSION020_52, (void *)v_STEAMUGC_INTERFACE_VERSION020_53, (void *)v_STEAMUGC_INTERFACE_VERSION020_54, (void *)v_STEAMUGC_INTERFACE_VERSION020_55, (void *)v_STEAMUGC_INTERFACE_VERSION020_56, (void *)v_STEAMUGC_INTERFACE_VERSION020_57, (void *)v_STEAMUGC_INTERFACE_VERSION020_58, (void *)v_STEAMUGC_INTERFACE_VERSION020_59, (void *)v_STEAMUGC_INTERFACE_VERSION020_60, (void *)v_STEAMUGC_INTERFACE_VERSION020_61, (void *)v_STEAMUGC_INTERFACE_VERSION020_62, (void *)v_STEAMUGC_INTERFACE_VERSION020_63, (void *)v_STEAMUGC_INTERFACE_VERSION020_64, (void *)v_STEAMUGC_INTERFACE_VERSION020_65, (void *)v_STEAMUGC_INTERFACE_VERSION020_66, (void *)v_STEAMUGC_INTERFACE_VERSION020_67, (void *)v_STEAMUGC_INTERFACE_VERSION020_68, (void *)v_STEAMUGC_INTERFACE_VERSION020_69, (void *)v_STEAMUGC_INTERFACE_VERSION020_70, (void *)v_STEAMUGC_INTERFACE_VERSION020_71, (void *)v_STEAMUGC_INTERFACE_VERSION020_72, (void *)v_STEAMUGC_INTERFACE_VERSION020_73, (void *)v_STEAMUGC_INTERFACE_VERSION020_74, (void *)v_STEAMUGC_INTERFACE_VERSION020_75, (void *)v_STEAMUGC_INTERFACE_VERSION020_76, (void *)v_STEAMUGC_INTERFACE_VERSION020_77, (void *)v_STEAMUGC_INTERFACE_VERSION020_78, (void *)v_STEAMUGC_INTERFACE_VERSION020_79, (void *)v_STEAMUGC_INTERFACE_VERSION020_80, (void *)v_STEAMUGC_INTERFACE_VERSION020_81, (void *)v_STEAMUGC_INTERFACE_VERSION020_82, (void *)v_STEAMUGC_INTERFACE_VERSION020_83, (void *)v_STEAMUGC_INTERFACE_VERSION020_84, (void *)v_STEAMUGC_INTERFACE_VERSION020_85, (void *)v_STEAMUGC_INTERFACE_VERSION020_86, (void *)v_STEAMUGC_INTERFACE_VERSION020_87, (void *)v_STEAMUGC_INTERFACE_VERSION020_88, (void *)v_STEAMUGC_INTERFACE_VERSION020_89, (void *)v_STEAMUGC_INTERFACE_VERSION020_90, (void *)v_STEAMUGC_INTERFACE_VERSION020_91, (void *)v_STEAMUGC_INTERFACE_VERSION020_92, (void *)v_STEAMUGC_INTERFACE_VERSION020_93};
static void v_STEAMVIDEO_INTERFACE_V007_0(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
static bool v_STEAMVIDEO_INTERFACE_V007_1(void *a0, void *a1) { (void)a0; (void)a1; return false; }
static void v_STEAMVIDEO_INTERFACE_V007_2(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
static void *vt_STEAMVIDEO_INTERFACE_V007[3] = {(void *)v_STEAMVIDEO_INTERFACE_V007_0, (void *)v_STEAMVIDEO_INTERFACE_V007_1, (void *)v_STEAMVIDEO_INTERFACE_V007_2};
static int32_t v_SteamClient021_0(void *a0) { (void)a0; return 0; }
static bool v_SteamClient021_1(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
static int32_t v_SteamClient021_2(void *a0, int32_t a1) { (void)a0; (void)a1; return 0; }
static int32_t v_SteamClient021_3(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static void v_SteamClient021_4(void *a0, int32_t a1, int32_t a2) { (void)a0; (void)a1; (void)a2;  }
static void *v_SteamClient021_5(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_6(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_8(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_9(void *a0, int32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2; return steam_stub_interface(a2); }
static void *v_SteamClient021_10(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_11(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_12(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_13(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_14(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_15(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_16(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_17(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_18(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_19(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static uint32_t v_SteamClient021_21(void *a0) { (void)a0; return 0; }
static bool v_SteamClient021_23(void *a0) { (void)a0; return false; }
static void *v_SteamClient021_24(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_25(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_26(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_27(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_28(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_29(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_33(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_34(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_35(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_36(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_37(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *v_SteamClient021_38(void *a0, int32_t a1, int32_t a2, const char *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return steam_stub_interface(a3); }
static void *vt_SteamClient021[39] = {(void *)v_SteamClient021_0, (void *)v_SteamClient021_1, (void *)v_SteamClient021_2, (void *)v_SteamClient021_3, (void *)v_SteamClient021_4, (void *)v_SteamClient021_5, (void *)v_SteamClient021_6, (void *)zero_slot, (void *)v_SteamClient021_8, (void *)v_SteamClient021_9, (void *)v_SteamClient021_10, (void *)v_SteamClient021_11, (void *)v_SteamClient021_12, (void *)v_SteamClient021_13, (void *)v_SteamClient021_14, (void *)v_SteamClient021_15, (void *)v_SteamClient021_16, (void *)v_SteamClient021_17, (void *)v_SteamClient021_18, (void *)v_SteamClient021_19, (void *)zero_slot, (void *)v_SteamClient021_21, (void *)zero_slot, (void *)v_SteamClient021_23, (void *)v_SteamClient021_24, (void *)v_SteamClient021_25, (void *)v_SteamClient021_26, (void *)v_SteamClient021_27, (void *)v_SteamClient021_28, (void *)v_SteamClient021_29, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamClient021_33, (void *)v_SteamClient021_34, (void *)v_SteamClient021_35, (void *)v_SteamClient021_36, (void *)v_SteamClient021_37, (void *)v_SteamClient021_38};
static void v_SteamGameServer015_1(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_2(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_3(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_4(void *a0, bool a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_5(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_6(void *a0) { (void)a0;  }
static void v_SteamGameServer015_7(void *a0) { (void)a0;  }
static bool v_SteamGameServer015_8(void *a0) { (void)a0; return false; }
static bool v_SteamGameServer015_9(void *a0) { (void)a0; return false; }
static uint64_t v_SteamGameServer015_10(void *a0) { (void)a0; return 0; }
static bool v_SteamGameServer015_11(void *a0) { (void)a0; return false; }
static void v_SteamGameServer015_12(void *a0, int32_t a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_13(void *a0, int32_t a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_14(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_15(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_16(void *a0, bool a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_17(void *a0, uint16_t a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_18(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_19(void *a0) { (void)a0;  }
static void v_SteamGameServer015_20(void *a0, const char *a1, const char *a2) { (void)a0; (void)a1; (void)a2;  }
static void v_SteamGameServer015_21(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_22(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_23(void *a0, const char *a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_24(void *a0, bool a1) { (void)a0; (void)a1;  }
static int32_t v_SteamGameServer015_26(void *a0, void *a1, int32_t a2, uint64_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
static void v_SteamGameServer015_27(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
static void v_SteamGameServer015_28(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
static int32_t v_SteamGameServer015_29(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_SteamGameServer015_30(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static void v_SteamGameServer015_31(void *a0) { (void)a0;  }
static uint64_t v_SteamGameServer015_32(void *a0) { (void)a0; return 0; }
static bool v_SteamGameServer015_34(void *a0, void *a1, int32_t a2, uint32_t a3, uint16_t a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
static int32_t v_SteamGameServer015_35(void *a0, void *a1, int32_t a2, void *a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return 0; }
static uint64_t v_SteamGameServer015_36(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static uint64_t v_SteamGameServer015_37(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static bool v_SteamGameServer015_38(void *a0, uint32_t a1, void *a2, uint32_t a3, void *a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; if (a4) *(uint64_t *)a4 = 76561197960265729ull; return true; }
static uint64_t v_SteamGameServer015_39(void *a0) { (void)a0; return 0; }
static void v_SteamGameServer015_40(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
static bool v_SteamGameServer015_41(void *a0, uint64_t a1, const char *a2, uint32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static void *vt_SteamGameServer015[42] = {(void *)zero_slot, (void *)v_SteamGameServer015_1, (void *)v_SteamGameServer015_2, (void *)v_SteamGameServer015_3, (void *)v_SteamGameServer015_4, (void *)v_SteamGameServer015_5, (void *)v_SteamGameServer015_6, (void *)v_SteamGameServer015_7, (void *)v_SteamGameServer015_8, (void *)v_SteamGameServer015_9, (void *)v_SteamGameServer015_10, (void *)v_SteamGameServer015_11, (void *)v_SteamGameServer015_12, (void *)v_SteamGameServer015_13, (void *)v_SteamGameServer015_14, (void *)v_SteamGameServer015_15, (void *)v_SteamGameServer015_16, (void *)v_SteamGameServer015_17, (void *)v_SteamGameServer015_18, (void *)v_SteamGameServer015_19, (void *)v_SteamGameServer015_20, (void *)v_SteamGameServer015_21, (void *)v_SteamGameServer015_22, (void *)v_SteamGameServer015_23, (void *)v_SteamGameServer015_24, (void *)zero_slot, (void *)v_SteamGameServer015_26, (void *)v_SteamGameServer015_27, (void *)v_SteamGameServer015_28, (void *)v_SteamGameServer015_29, (void *)v_SteamGameServer015_30, (void *)v_SteamGameServer015_31, (void *)v_SteamGameServer015_32, (void *)zero_slot, (void *)v_SteamGameServer015_34, (void *)v_SteamGameServer015_35, (void *)v_SteamGameServer015_36, (void *)v_SteamGameServer015_37, (void *)v_SteamGameServer015_38, (void *)v_SteamGameServer015_39, (void *)v_SteamGameServer015_40, (void *)v_SteamGameServer015_41};
static int32_t v_SteamNetworkingSockets012_4(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static bool v_SteamNetworkingSockets012_5(void *a0, uint32_t a1, int32_t a2, const char *a3, bool a4) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; return false; }
static bool v_SteamNetworkingSockets012_6(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
static bool v_SteamNetworkingSockets012_7(void *a0, uint32_t a1, int64_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static int64_t v_SteamNetworkingSockets012_8(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static void v_SteamNetworkingSockets012_9(void *a0, uint32_t a1, const char *a2) { (void)a0; (void)a1; (void)a2;  }
static bool v_SteamNetworkingSockets012_10(void *a0, uint32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static int32_t v_SteamNetworkingSockets012_11(void *a0, uint32_t a1, void *a2, uint32_t a3, int32_t a4, void *a5) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; return 0; }
static int32_t v_SteamNetworkingSockets012_13(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static int32_t v_SteamNetworkingSockets012_17(void *a0, uint32_t a1, void *a2, int32_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
static bool v_SteamNetworkingSockets012_18(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return false; }
static int32_t v_SteamNetworkingSockets012_22(void *a0) { (void)a0; return 0; }
static uint32_t v_SteamNetworkingSockets012_24(void *a0) { (void)a0; return 0; }
static bool v_SteamNetworkingSockets012_25(void *a0, uint32_t a1) { (void)a0; (void)a1; return false; }
static bool v_SteamNetworkingSockets012_26(void *a0, uint32_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static uint16_t v_SteamNetworkingSockets012_31(void *a0) { (void)a0; return 0; }
static uint32_t v_SteamNetworkingSockets012_32(void *a0) { (void)a0; return 0; }
static int32_t v_SteamNetworkingSockets012_33(void *a0, void *a1) { (void)a0; (void)a1; return 0; }
static void v_SteamNetworkingSockets012_41(void *a0) { (void)a0;  }
static bool v_SteamNetworkingSockets012_42(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
static int32_t v_SteamNetworkingSockets012_45(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return 0; }
static void *vt_SteamNetworkingSockets012[46] = {(void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_4, (void *)v_SteamNetworkingSockets012_5, (void *)v_SteamNetworkingSockets012_6, (void *)v_SteamNetworkingSockets012_7, (void *)v_SteamNetworkingSockets012_8, (void *)v_SteamNetworkingSockets012_9, (void *)v_SteamNetworkingSockets012_10, (void *)v_SteamNetworkingSockets012_11, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_13, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_17, (void *)v_SteamNetworkingSockets012_18, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_22, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_24, (void *)v_SteamNetworkingSockets012_25, (void *)v_SteamNetworkingSockets012_26, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_31, (void *)v_SteamNetworkingSockets012_32, (void *)v_SteamNetworkingSockets012_33, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_41, (void *)v_SteamNetworkingSockets012_42, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingSockets012_45};
static bool v_SteamNetworkingUtils004_7(void *a0, float a1) { (void)a0; (void)a1; return false; }
static int32_t v_SteamNetworkingUtils004_8(void *a0, uint32_t a1, void *a2) { (void)a0; (void)a1; (void)a2; return 0; }
static int32_t v_SteamNetworkingUtils004_9(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static int32_t v_SteamNetworkingUtils004_10(void *a0) { (void)a0; return 0; }
static int32_t v_SteamNetworkingUtils004_11(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static int64_t v_SteamNetworkingUtils004_12(void *a0) { (void)a0; return 0; }
static int32_t v_SteamNetworkingUtils004_14(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static const char *v_SteamNetworkingUtils004_18(void *a0, int32_t a1, void *a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return ""; }
static int32_t v_SteamNetworkingUtils004_19(void *a0, int32_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_SteamNetworkingUtils004_21(void *a0, void *a1, const char *a2) { (void)a0; (void)a1; (void)a2; return false; }
static int32_t v_SteamNetworkingUtils004_22(void *a0, void *a1) { (void)a0; (void)a1; return 0; }
static void *vt_SteamNetworkingUtils004[23] = {(void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingUtils004_7, (void *)v_SteamNetworkingUtils004_8, (void *)v_SteamNetworkingUtils004_9, (void *)v_SteamNetworkingUtils004_10, (void *)v_SteamNetworkingUtils004_11, (void *)v_SteamNetworkingUtils004_12, (void *)zero_slot, (void *)v_SteamNetworkingUtils004_14, (void *)zero_slot, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamNetworkingUtils004_18, (void *)v_SteamNetworkingUtils004_19, (void *)zero_slot, (void *)v_SteamNetworkingUtils004_21, (void *)v_SteamNetworkingUtils004_22};
static uint32_t v_SteamParties002_0(void *a0) { (void)a0; return 0; }
static uint64_t v_SteamParties002_1(void *a0, uint32_t a1) { (void)a0; (void)a1; return 0; }
static uint64_t v_SteamParties002_3(void *a0, uint64_t a1) { (void)a0; (void)a1; return 0; }
static bool v_SteamParties002_4(void *a0, void *a1) { (void)a0; (void)a1; return false; }
static void v_SteamParties002_7(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
static void v_SteamParties002_8(void *a0, uint64_t a1, uint64_t a2) { (void)a0; (void)a1; (void)a2;  }
static uint64_t v_SteamParties002_9(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_SteamParties002_10(void *a0, uint64_t a1) { (void)a0; (void)a1; return false; }
static void *vt_SteamParties002[11] = {(void *)v_SteamParties002_0, (void *)v_SteamParties002_1, (void *)zero_slot, (void *)v_SteamParties002_3, (void *)v_SteamParties002_4, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamParties002_7, (void *)v_SteamParties002_8, (void *)v_SteamParties002_9, (void *)v_SteamParties002_10};
static int32_t v_SteamUser023_0(void *a0) { (void)a0; return 1; }
static bool v_SteamUser023_1(void *a0) { (void)a0; return true; }
static uint64_t v_SteamUser023_2(void *a0) { (void)a0; return 76561197960265729ull; }
static int32_t v_SteamUser023_3(void *a0, void *a1, int32_t a2, uint64_t a3, uint32_t a4, uint16_t a5, bool a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return steam_stub_ticket(a1, a2); }
static void v_SteamUser023_4(void *a0, uint32_t a1, uint16_t a2) { (void)a0; (void)a1; (void)a2;  }
static bool v_SteamUser023_6(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return false; }
static void v_SteamUser023_7(void *a0) { (void)a0;  }
static void v_SteamUser023_8(void *a0) { (void)a0;  }
static int32_t v_SteamUser023_11(void *a0, void *a1, uint32_t a2, void *a3, uint32_t a4, void *a5, uint32_t a6) { (void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; return 0; }
static uint32_t v_SteamUser023_12(void *a0) { (void)a0; return 0; }
static uint32_t v_SteamUser023_14(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
static int32_t v_SteamUser023_15(void *a0, void *a1, int32_t a2, uint64_t a3) { (void)a0; (void)a1; (void)a2; (void)a3; return 0; }
static void v_SteamUser023_16(void *a0, uint64_t a1) { (void)a0; (void)a1;  }
static void v_SteamUser023_17(void *a0, uint32_t a1) { (void)a0; (void)a1;  }
static int32_t v_SteamUser023_18(void *a0, uint64_t a1, uint32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_SteamUser023_19(void *a0) { (void)a0; return false; }
static void v_SteamUser023_20(void *a0, uint64_t a1, uint32_t a2, uint16_t a3) { (void)a0; (void)a1; (void)a2; (void)a3;  }
static uint64_t v_SteamUser023_21(void *a0, void *a1, int32_t a2) { (void)a0; (void)a1; (void)a2; return 0; }
static bool v_SteamUser023_22(void *a0, void *a1, int32_t a2, void *a3) { (void)a0; (void)a1; (void)a2; (void)a3; return false; }
static int32_t v_SteamUser023_23(void *a0, int32_t a1, bool a2) { (void)a0; (void)a1; (void)a2; return 0; }
static int32_t v_SteamUser023_24(void *a0) { (void)a0; return 0; }
static uint64_t v_SteamUser023_25(void *a0, const char *a1) { (void)a0; (void)a1; return 0; }
static bool v_SteamUser023_26(void *a0) { (void)a0; return false; }
static bool v_SteamUser023_27(void *a0) { (void)a0; return false; }
static bool v_SteamUser023_28(void *a0) { (void)a0; return false; }
static bool v_SteamUser023_29(void *a0) { (void)a0; return false; }
static uint64_t v_SteamUser023_30(void *a0) { (void)a0; return 0; }
static uint64_t v_SteamUser023_31(void *a0) { (void)a0; return 0; }
static bool v_SteamUser023_32(void *a0, int32_t a1) { (void)a0; (void)a1; return false; }
static void *vt_SteamUser023[33] = {(void *)v_SteamUser023_0, (void *)v_SteamUser023_1, (void *)v_SteamUser023_2, (void *)v_SteamUser023_3, (void *)v_SteamUser023_4, (void *)zero_slot, (void *)v_SteamUser023_6, (void *)v_SteamUser023_7, (void *)v_SteamUser023_8, (void *)zero_slot, (void *)zero_slot, (void *)v_SteamUser023_11, (void *)v_SteamUser023_12, (void *)zero_slot, (void *)v_SteamUser023_14, (void *)v_SteamUser023_15, (void *)v_SteamUser023_16, (void *)v_SteamUser023_17, (void *)v_SteamUser023_18, (void *)v_SteamUser023_19, (void *)v_SteamUser023_20, (void *)v_SteamUser023_21, (void *)v_SteamUser023_22, (void *)v_SteamUser023_23, (void *)v_SteamUser023_24, (void *)v_SteamUser023_25, (void *)v_SteamUser023_26, (void *)v_SteamUser023_27, (void *)v_SteamUser023_28, (void *)v_SteamUser023_29, (void *)v_SteamUser023_30, (void *)v_SteamUser023_31, (void *)v_SteamUser023_32};
void **steam_stub_vtable(const char *version) {
    if (!strcmp(version, "STEAMREMOTEPLAY_INTERFACE_VERSION002")) return vt_STEAMREMOTEPLAY_INTERFACE_VERSION002;
    if (!strcmp(version, "STEAMTIMELINE_INTERFACE_V004")) return vt_STEAMTIMELINE_INTERFACE_V004;
    if (!strcmp(version, "STEAMUGC_INTERFACE_VERSION020")) return vt_STEAMUGC_INTERFACE_VERSION020;
    if (!strcmp(version, "STEAMVIDEO_INTERFACE_V007")) return vt_STEAMVIDEO_INTERFACE_V007;
    if (!strcmp(version, "SteamClient021")) return vt_SteamClient021;
    if (!strcmp(version, "SteamGameServer015")) return vt_SteamGameServer015;
    if (!strcmp(version, "SteamNetworkingSockets012")) return vt_SteamNetworkingSockets012;
    if (!strcmp(version, "SteamNetworkingUtils004")) return vt_SteamNetworkingUtils004;
    if (!strcmp(version, "SteamParties002")) return vt_SteamParties002;
    if (!strcmp(version, "SteamUser023")) return vt_SteamUser023;
    return NULL;
}
#endif
