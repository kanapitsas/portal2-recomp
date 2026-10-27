/* Généré par tools/gen_steam.py depuis la libsteam_api.dylib officielle (SDK 1.53a) et
 * Steamworks.NET 20.1.0 (MIT). Ne pas modifier : relancer le générateur. */
#include "steam.h"

void steam_manual_client_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret);
void steam_manual_utils_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret);
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppBuildId */
static void m0(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetAppBuildId"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppInstallDir */
static void m1(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetAppInstallDir"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppName */
static void m2(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetAppName"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetInstalledApps */
static void m3(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetInstalledApps"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 4u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetNumInstalledApps */
static void m4(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamAppList_GetNumInstalledApps"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::BGetDLCDataByIndex */
static void m5(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BGetDLCDataByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 1u);
    void *a3 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsAppInstalled */
static void m6(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsAppInstalled"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsCybercafe */
static void m7(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsCybercafe"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsDlcInstalled */
static void m8(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsDlcInstalled"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsLowViolence */
static void m9(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsLowViolence"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribed */
static void m10(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribed"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedApp */
static void m11(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribedApp"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFamilySharing */
static void m12(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribedFromFamilySharing"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFreeWeekend */
static void m13(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsSubscribedFromFreeWeekend"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsTimedTrial */
static void m14(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsTimedTrial"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsVACBanned */
static void m15(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_BIsVACBanned"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppBuildId */
static void m16(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAppBuildId"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppInstallDir */
static void m17(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAppInstallDir"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppOwner */
static void m18(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAppOwner"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAvailableGameLanguages */
static void m19(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetAvailableGameLanguages"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetCurrentBetaName */
static void m20(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetCurrentBetaName"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetCurrentGameLanguage */
static void m21(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetCurrentGameLanguage"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetDLCCount */
static void m22(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetDLCCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetDlcDownloadProgress */
static void m23(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetDlcDownloadProgress"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    void *a2 = steam_out(c, R32(args + 8u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetEarliestPurchaseUnixTime */
static void m24(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetEarliestPurchaseUnixTime"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetFileDetails */
static void m25(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetFileDetails"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetInstalledDepots */
static void m26(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetInstalledDepots"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetLaunchCommandLine */
static void m27(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetLaunchCommandLine"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetLaunchQueryParam */
static void m28(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_GetLaunchQueryParam"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::InstallDLC */
static void m29(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_InstallDLC"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMAPPS_INTERFACE_VERSION008::MarkContentCorrupt */
static void m30(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_MarkContentCorrupt"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::RequestAllProofOfPurchaseKeys */
static void m31(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_RequestAllProofOfPurchaseKeys"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMAPPS_INTERFACE_VERSION008::RequestAppProofOfPurchaseKey */
static void m32(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_RequestAppProofOfPurchaseKey"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMAPPS_INTERFACE_VERSION008::UninstallDLC */
static void m33(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamApps_UninstallDLC"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::AddHeader */
static void m34(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_AddHeader"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::AllowStartRequest */
static void m35(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_AllowStartRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::CopyToClipboard */
static void m36(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_CopyToClipboard"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::CreateBrowser */
static void m37(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_CreateBrowser"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::ExecuteJavascript */
static void m38(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_ExecuteJavascript"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Find */
static void m40(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, bool, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Find"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool a2 = (R8(args + 8u) != 0);
    bool a3 = (R8(args + 12u) != 0);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GetLinkAtPosition */
static void m41(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_GetLinkAtPosition"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoBack */
static void m42(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_GoBack"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoForward */
static void m43(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_GoForward"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Init */
static void m44(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Init"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::JSDialogResponse */
static void m45(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_JSDialogResponse"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyChar */
static void m46(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_KeyChar"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyDown */
static void m47(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_KeyDown"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyUp */
static void m48(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_KeyUp"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::LoadURL */
static void m49(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_LoadURL"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDoubleClick */
static void m50(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseDoubleClick"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDown */
static void m51(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseDown"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseMove */
static void m52(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseMove"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseUp */
static void m53(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseUp"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseWheel */
static void m54(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_MouseWheel"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::OpenDeveloperTools */
static void m55(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_OpenDeveloperTools"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::PasteFromClipboard */
static void m56(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_PasteFromClipboard"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Reload */
static void m57(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Reload"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::RemoveBrowser */
static void m58(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_RemoveBrowser"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetBackgroundMode */
static void m59(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetBackgroundMode"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetCookie */
static void m60(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, const char *, const char *, uint32_t, bool, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetCookie"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    const char *a3 = steam_in_string(c, R32(args + 12u));
    uint32_t a4 = (uint32_t)R32(args + 16u);
    bool a5 = (R8(args + 20u) != 0);
    bool a6 = (R8(args + 24u) != 0);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetDPIScalingFactor */
static void m61(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetDPIScalingFactor"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    float a1 = RF(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetHorizontalScroll */
static void m62(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetHorizontalScroll"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetKeyFocus */
static void m63(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetKeyFocus"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetPageScaleFactor */
static void m64(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, float, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetPageScaleFactor"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    float a1 = RF(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetSize */
static void m65(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetSize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetVerticalScroll */
static void m66(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_SetVerticalScroll"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Shutdown */
static void m67(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_Shutdown"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopFind */
static void m68(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_StopFind"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopLoad */
static void m69(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_StopLoad"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::ViewSource */
static void m70(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTMLSurface_ViewSource"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTTP_INTERFACE_VERSION003::CreateCookieContainer */
static void m71(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_CreateCookieContainer"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION003::CreateHTTPRequest */
static void m72(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_CreateHTTPRequest"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION003::DeferHTTPRequest */
static void m73(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_DeferHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPDownloadProgressPct */
static void m74(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPDownloadProgressPct"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPRequestWasTimedOut */
static void m75(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPRequestWasTimedOut"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodyData */
static void m76(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseBodyData"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodySize */
static void m77(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseBodySize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderSize */
static void m78(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseHeaderSize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderValue */
static void m79(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPResponseHeaderValue"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPStreamingResponseBodyData */
static void m80(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_GetHTTPStreamingResponseBodyData"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::PrioritizeHTTPRequest */
static void m81(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_PrioritizeHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::ReleaseCookieContainer */
static void m82(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_ReleaseCookieContainer"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::ReleaseHTTPRequest */
static void m83(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_ReleaseHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequest */
static void m84(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SendHTTPRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequestAndStreamResponse */
static void m85(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SendHTTPRequestAndStreamResponse"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetCookie */
static void m86(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetCookie"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    const char *a3 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestAbsoluteTimeoutMS */
static void m87(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestAbsoluteTimeoutMS"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestContextValue */
static void m88(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestContextValue"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestCookieContainer */
static void m89(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestCookieContainer"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestGetOrPostParameter */
static void m90(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestGetOrPostParameter"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestHeaderValue */
static void m91(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestHeaderValue"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestNetworkActivityTimeout */
static void m92(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestNetworkActivityTimeout"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRequiresVerifiedCertificate */
static void m94(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestRequiresVerifiedCertificate"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestUserAgentInfo */
static void m95(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamHTTP_SetHTTPRequestUserAgentInfo"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::AddPromoItem */
static void m96(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_AddPromoItem"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::CheckResultSteamID */
static void m98(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_CheckResultSteamID"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::ConsumeItem */
static void m99(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_ConsumeItem"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::DeserializeResult */
static void m100(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_DeserializeResult"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::DestroyResult */
static void m101(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_DestroyResult"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMINVENTORY_INTERFACE_V003::GetAllItems */
static void m104(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetAllItems"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetItemPrice */
static void m108(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetItemPrice"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    void *a2 = steam_out(c, R32(args + 8u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetItemsByID */
static void m109(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetItemsByID"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetNumItemsWithPrices */
static void m111(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetNumItemsWithPrices"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GetResultStatus */
static void m114(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetResultStatus"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GetResultTimestamp */
static void m115(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GetResultTimestamp"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GrantPromoItems */
static void m116(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_GrantPromoItems"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::InspectItem */
static void m117(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_InspectItem"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::LoadItemDefinitions */
static void m118(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_LoadItemDefinitions"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::RemoveProperty */
static void m119(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_RemoveProperty"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::RequestEligiblePromoItemDefinitionsIDs */
static void m120(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_RequestEligiblePromoItemDefinitionsIDs"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::RequestPrices */
static void m121(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_RequestPrices"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::SendItemDropHeartbeat */
static void m122(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SendItemDropHeartbeat"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyBool */
static void m124(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyBool"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    bool a3 = (R8(args + 20u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyFloat */
static void m125(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    float a3 = RF(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyInt64 */
static void m126(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, int64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyInt64"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    int64_t a3 = (int64_t)R64(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyString */
static void m127(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SetPropertyString"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    const char *a3 = steam_in_string(c, R32(args + 20u));
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::StartUpdateProperties */
static void m129(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_StartUpdateProperties"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::SubmitUpdateProperties */
static void m130(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_SubmitUpdateProperties"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::TransferItemQuantity */
static void m131(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint64_t, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_TransferItemQuantity"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    uint64_t a3 = (uint64_t)R64(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::TriggerItemDrop */
static void m132(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInventory_TriggerItemDrop"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::BActivationSuccess */
static void m133(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_BActivationSuccess"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::BIsCurrentMusicRemote */
static void m134(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_BIsCurrentMusicRemote"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryDidChange */
static void m135(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_CurrentEntryDidChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryIsAvailable */
static void m136(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_CurrentEntryIsAvailable"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryWillChange */
static void m137(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_CurrentEntryWillChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::DeregisterSteamMusicRemote */
static void m138(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_DeregisterSteamMusicRemote"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableLooped */
static void m139(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnableLooped"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayNext */
static void m140(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnablePlayNext"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayPrevious */
static void m141(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnablePlayPrevious"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlaylists */
static void m142(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnablePlaylists"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableQueue */
static void m143(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnableQueue"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableShuffled */
static void m144(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_EnableShuffled"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistDidChange */
static void m145(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_PlaylistDidChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistWillChange */
static void m146(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_PlaylistWillChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueDidChange */
static void m147(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_QueueDidChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueWillChange */
static void m148(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_QueueWillChange"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::RegisterSteamMusicRemote */
static void m149(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_RegisterSteamMusicRemote"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetPlaylistEntries */
static void m150(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_ResetPlaylistEntries"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetQueueEntries */
static void m151(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_ResetQueueEntries"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentPlaylistEntry */
static void m152(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetCurrentPlaylistEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentQueueEntry */
static void m153(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetCurrentQueueEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetDisplayName */
static void m154(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetDisplayName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPNGIcon_64x64 */
static void m155(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetPNGIcon_64x64"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPlaylistEntry */
static void m156(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetPlaylistEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetQueueEntry */
static void m157(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_SetQueueEntry"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryCoverArt */
static void m158(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateCurrentEntryCoverArt"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryElapsedSeconds */
static void m159(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateCurrentEntryElapsedSeconds"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryText */
static void m160(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateCurrentEntryText"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateLooped */
static void m161(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateLooped"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdatePlaybackStatus */
static void m162(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdatePlaybackStatus"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateShuffled */
static void m163(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateShuffled"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateVolume */
static void m164(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusicRemote_UpdateVolume"), __ATOMIC_RELAXED);
    float a0 = RF(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::BIsEnabled */
static void m165(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_BIsEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::BIsPlaying */
static void m166(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_BIsPlaying"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::GetPlaybackStatus */
static void m167(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_GetPlaybackStatus"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMMUSIC_INTERFACE_VERSION001::GetVolume */
static void m168(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef float (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_GetVolume"), __ATOMIC_RELAXED);
    float r = ((F)fn)(self);
    rt_return_st0(c, r);
}
/* STEAMMUSIC_INTERFACE_VERSION001::Pause */
static void m169(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_Pause"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::Play */
static void m170(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_Play"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::PlayNext */
static void m171(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_PlayNext"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::PlayPrevious */
static void m172(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_PlayPrevious"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::SetVolume */
static void m173(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMusic_SetVolume"), __ATOMIC_RELAXED);
    float a0 = RF(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppBlocked */
static void m174(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsAppBlocked"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppInBlockList */
static void m175(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsAppInBlockList"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureBlocked */
static void m176(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsFeatureBlocked"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureInBlockList */
static void m177(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsFeatureInBlockList"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockEnabled */
static void m178(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsParentalLockEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockLocked */
static void m179(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamParentalSettings_BIsParentalLockLocked"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::BeginFileWriteBatch */
static void m180(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_BeginFileWriteBatch"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::CommitPublishedFileUpdate */
static void m181(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_CommitPublishedFileUpdate"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::CreatePublishedFileUpdateRequest */
static void m182(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_CreatePublishedFileUpdateRequest"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::DeletePublishedFile */
static void m183(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_DeletePublishedFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EndFileWriteBatch */
static void m184(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EndFileWriteBatch"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedFilesByUserAction */
static void m185(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EnumeratePublishedFilesByUserAction"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserPublishedFiles */
static void m187(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EnumerateUserPublishedFiles"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSubscribedFiles */
static void m189(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_EnumerateUserSubscribedFiles"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileDelete */
static void m190(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileDelete"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileExists */
static void m191(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileExists"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileForget */
static void m192(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileForget"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FilePersisted */
static void m193(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FilePersisted"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileRead */
static void m194(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileRead"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsync */
static void m195(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileReadAsync"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsyncComplete */
static void m196(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileReadAsyncComplete"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileShare */
static void m197(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileShare"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWrite */
static void m198(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWrite"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteAsync */
static void m199(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteAsync"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamCancel */
static void m200(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamCancel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamClose */
static void m201(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamClose"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamOpen */
static void m202(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamOpen"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamWriteChunk */
static void m203(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_FileWriteStreamWriteChunk"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCCount */
static void m204(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetCachedUGCCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCHandle */
static void m205(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetCachedUGCHandle"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileCount */
static void m206(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileNameAndSize */
static void m207(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileNameAndSize"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileSize */
static void m208(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileSize"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileTimestamp */
static void m209(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetFileTimestamp"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChange */
static void m210(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetLocalFileChange"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChangeCount */
static void m211(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetLocalFileChangeCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedFileDetails */
static void m212(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetPublishedFileDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedItemVoteDetails */
static void m213(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetPublishedItemVoteDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetQuota */
static void m214(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetQuota"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 8u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetSyncPlatforms */
static void m215(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetSyncPlatforms"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDetails */
static void m216(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, const char **, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetUGCDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2_slot = steam_out(c, R32(args + 12u), 4u); const char *a2_text = NULL;
    const char **a2 = a2_slot ? &a2_text : NULL;
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    void *a4 = steam_out(c, R32(args + 20u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    if (a2_slot) { uint32_t a2_guest = steam_guest_string(c, a2_text); memcpy(a2_slot, &a2_guest, 4); }
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDownloadProgress */
static void m217(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetUGCDownloadProgress"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUserPublishedItemVoteDetails */
static void m218(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_GetUserPublishedItemVoteDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForAccount */
static void m219(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_IsCloudEnabledForAccount"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForApp */
static void m220(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_IsCloudEnabledForApp"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetCloudEnabledForApp */
static void m221(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SetCloudEnabledForApp"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetSyncPlatforms */
static void m222(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SetSyncPlatforms"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetUserPublishedFileAction */
static void m223(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SetUserPublishedFileAction"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SubscribePublishedFile */
static void m224(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_SubscribePublishedFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownload */
static void m225(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UGCDownload"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownloadToLocation */
static void m226(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UGCDownloadToLocation"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCRead */
static void m227(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UGCRead"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UnsubscribePublishedFile */
static void m228(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UnsubscribePublishedFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileDescription */
static void m229(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileFile */
static void m230(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFilePreviewFile */
static void m231(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFilePreviewFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileSetChangeDescription */
static void m232(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileSetChangeDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTitle */
static void m234(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileTitle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileVisibility */
static void m235(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdatePublishedFileVisibility"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdateUserPublishedItemVote */
static void m236(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamRemoteStorage_UpdateUserPublishedItemVote"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::AddScreenshotToLibrary */
static void m237(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, const char *, const char *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_AddScreenshotToLibrary"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::AddVRScreenshotToLibrary */
static void m238(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_AddVRScreenshotToLibrary"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::HookScreenshots */
static void m239(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_HookScreenshots"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::IsScreenshotsHooked */
static void m240(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_IsScreenshotsHooked"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::SetLocation */
static void m241(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_SetLocation"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TagPublishedFile */
static void m242(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_TagPublishedFile"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TagUser */
static void m243(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_TagUser"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TriggerScreenshot */
static void m244(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_TriggerScreenshot"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::WriteScreenshot */
static void m245(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamScreenshots_WriteScreenshot"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::AddAppDependency */
static void m246(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddAppDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddDependency */
static void m247(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddExcludedTag */
static void m248(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddExcludedTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemKeyValueTag */
static void m249(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemPreviewFile */
static void m250(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemPreviewFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemPreviewVideo */
static void m251(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemPreviewVideo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemToFavorites */
static void m252(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddItemToFavorites"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddRequiredKeyValueTag */
static void m253(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddRequiredKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddRequiredTag */
static void m254(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_AddRequiredTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::BInitWorkshopForGameServer */
static void m256(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_BInitWorkshopForGameServer"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::CreateItem */
static void m257(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateItem"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestCursor */
static void m258(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryAllUGCRequestCursor"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestPage */
static void m259(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryAllUGCRequestPage"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryUserUGCRequest */
static void m261(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_CreateQueryUserUGCRequest"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    uint32_t a6 = (uint32_t)R32(args + 24u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::DeleteItem */
static void m262(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_DeleteItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::DownloadItem */
static void m263(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_DownloadItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetAppDependencies */
static void m264(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetAppDependencies"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemDownloadInfo */
static void m265(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemDownloadInfo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemInstallInfo */
static void m266(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemInstallInfo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemState */
static void m267(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemState"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemUpdateProgress */
static void m268(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetItemUpdateProgress"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetNumSubscribedItems */
static void m269(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetNumSubscribedItems"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryFirstUGCKeyValueTag */
static void m270(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryFirstUGCKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCChildren */
static void m271(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCChildren"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCKeyValueTag */
static void m272(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCKeyValueTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    void *a5 = steam_out_buffer(c, R32(args + 24u), (int64_t)(int32_t)R32(args + 28u), 1u);
    uint32_t a6 = (uint32_t)R32(args + 28u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCMetadata */
static void m273(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCMetadata"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumAdditionalPreviews */
static void m274(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCNumAdditionalPreviews"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumKeyValueTags */
static void m275(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCNumKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumTags */
static void m276(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCNumTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCPreviewURL */
static void m277(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCPreviewURL"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCStatistic */
static void m279(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCStatistic"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTag */
static void m280(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTagDisplayName */
static void m281(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetQueryUGCTagDisplayName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetSubscribedItems */
static void m282(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetSubscribedItems"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetUserItemVote */
static void m283(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetUserItemVote"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::GetWorkshopEULAStatus */
static void m284(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_GetWorkshopEULAStatus"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::ReleaseQueryUGCRequest */
static void m285(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_ReleaseQueryUGCRequest"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveAllItemKeyValueTags */
static void m286(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveAllItemKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveAppDependency */
static void m287(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveAppDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveDependency */
static void m288(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveDependency"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemFromFavorites */
static void m289(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveItemFromFavorites"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemKeyValueTags */
static void m290(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveItemKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemPreview */
static void m291(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RemoveItemPreview"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RequestUGCDetails */
static void m292(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_RequestUGCDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SendQueryUGCRequest */
static void m293(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SendQueryUGCRequest"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SetAllowCachedResponse */
static void m294(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetAllowCachedResponse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetAllowLegacyUpload */
static void m295(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetAllowLegacyUpload"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetCloudFileNameFilter */
static void m296(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetCloudFileNameFilter"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemContent */
static void m297(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemContent"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemDescription */
static void m298(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemMetadata */
static void m299(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemMetadata"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemPreview */
static void m300(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemPreview"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemTitle */
static void m302(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemTitle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemUpdateLanguage */
static void m303(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemUpdateLanguage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemVisibility */
static void m304(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetItemVisibility"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetLanguage */
static void m305(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetLanguage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetMatchAnyTag */
static void m306(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetMatchAnyTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetRankedByTrendDays */
static void m307(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetRankedByTrendDays"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnAdditionalPreviews */
static void m308(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnAdditionalPreviews"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnChildren */
static void m309(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnChildren"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnKeyValueTags */
static void m310(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnKeyValueTags"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnLongDescription */
static void m311(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnLongDescription"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnMetadata */
static void m312(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnMetadata"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnOnlyIDs */
static void m313(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnOnlyIDs"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnPlaytimeStats */
static void m314(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnPlaytimeStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnTotalOnly */
static void m315(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetReturnTotalOnly"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetSearchText */
static void m316(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetSearchText"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetTimeCreatedDateRange */
static void m317(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetTimeCreatedDateRange"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetTimeUpdatedDateRange */
static void m318(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetTimeUpdatedDateRange"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetUserItemVote */
static void m319(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SetUserItemVote"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::ShowWorkshopEULA */
static void m320(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_ShowWorkshopEULA"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::StartItemUpdate */
static void m321(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_StartItemUpdate"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTrackingForAllItems */
static void m324(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_StopPlaytimeTrackingForAllItems"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SubmitItemUpdate */
static void m325(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SubmitItemUpdate"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SubscribeItem */
static void m326(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SubscribeItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SuspendDownloads */
static void m327(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_SuspendDownloads"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMUGC_INTERFACE_VERSION016::UnsubscribeItem */
static void m328(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_UnsubscribeItem"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewFile */
static void m329(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_UpdateItemPreviewFile"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewVideo */
static void m330(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUGC_UpdateItemPreviewVideo"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::AttachLeaderboardUGC */
static void m331(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_AttachLeaderboardUGC"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::ClearAchievement */
static void m332(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_ClearAchievement"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntries */
static void m333(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_DownloadLeaderboardEntries"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntriesForUsers */
static void m334(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_DownloadLeaderboardEntriesForUsers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::FindLeaderboard */
static void m335(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_FindLeaderboard"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::FindOrCreateLeaderboard */
static void m336(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_FindOrCreateLeaderboard"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievement */
static void m337(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievement"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAchievedPercent */
static void m338(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementAchievedPercent"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAndUnlockTime */
static void m339(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementAndUnlockTime"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementDisplayAttribute */
static void m340(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementDisplayAttribute"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementIcon */
static void m341(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementIcon"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementName */
static void m342(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementName"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsFloat */
static void m343(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementProgressLimitsFloat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsInt32 */
static void m344(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetAchievementProgressLimitsInt32"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetDownloadedLeaderboardEntry */
static void m345(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetDownloadedLeaderboardEntry"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 28u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatDouble */
static void m346(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatDouble"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryDouble */
static void m347(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatHistoryDouble"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryInt64 */
static void m348(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatHistoryInt64"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatInt64 */
static void m349(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetGlobalStatInt64"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardDisplayType */
static void m350(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardDisplayType"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardEntryCount */
static void m351(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardEntryCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardName */
static void m352(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardSortMethod */
static void m353(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetLeaderboardSortMethod"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetNumAchievements */
static void m356(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetNumAchievements"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetNumberOfCurrentPlayers */
static void m357(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetNumberOfCurrentPlayers"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetStatFloat */
static void m358(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetStatFloat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetStatInt32 */
static void m359(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetStatInt32"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievement */
static void m360(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievementAndUnlockTime */
static void m361(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserAchievementAndUnlockTime"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatFloat */
static void m362(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserStatFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatInt32 */
static void m363(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_GetUserStatInt32"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::IndicateAchievementProgress */
static void m364(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_IndicateAchievementProgress"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestCurrentStats */
static void m365(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestCurrentStats"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalAchievementPercentages */
static void m366(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestGlobalAchievementPercentages"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalStats */
static void m367(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestGlobalStats"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestUserStats */
static void m368(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_RequestUserStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::ResetAllStats */
static void m369(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_ResetAllStats"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetAchievement */
static void m370(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_SetAchievement"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetStatFloat */
static void m371(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_SetStatFloat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetStatInt32 */
static void m372(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_SetStatInt32"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::StoreStats */
static void m373(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_StoreStats"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::UpdateAvgRateStat */
static void m374(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float, double);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_UpdateAvgRateStat"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    double a2 = RD(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::UploadLeaderboardScore */
static void m375(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUserStats_UploadLeaderboardScore"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_in_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMVIDEO_INTERFACE_V002::GetOPFSettings */
static void m376(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamVideo_GetOPFSettings"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V002::GetVideoURL */
static void m378(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamVideo_GetVideoURL"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V002::IsBroadcasting */
static void m379(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamVideo_IsBroadcasting"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::BReleaseSteamPipe */
static void m380(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_BReleaseSteamPipe"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::BShutdownIfAllPipesClosed */
static void m381(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_BShutdownIfAllPipesClosed"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::ConnectToGlobalUser */
static void m382(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_ConnectToGlobalUser"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamClient020::CreateLocalUser */
static void m383(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_CreateLocalUser"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamClient020::CreateSteamPipe */
static void m384(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_CreateSteamPipe"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient020::GetIPCCallCount */
static void m385(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetIPCCallCount"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient020::GetISteamAppList */
static void m386(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamAppList"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamApps */
static void m387(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamApps"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamController */
static void m388(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamController"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamFriends */
static void m389(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamFriends"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameSearch */
static void m390(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGameSearch"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameServer */
static void m391(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGameServer"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameServerStats */
static void m392(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGameServerStats"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGenericInterface */
static void m393(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamGenericInterface"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamHTMLSurface */
static void m394(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamHTMLSurface"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamHTTP */
static void m395(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamHTTP"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamInput */
static void m396(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamInput"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamInventory */
static void m397(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamInventory"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMatchmaking */
static void m398(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMatchmaking"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMatchmakingServers */
static void m399(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMatchmakingServers"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMusic */
static void m400(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMusic"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMusicRemote */
static void m401(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamMusicRemote"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamNetworking */
static void m402(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamNetworking"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamParentalSettings */
static void m403(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamParentalSettings"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamParties */
static void m404(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamParties"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamRemotePlay */
static void m405(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamRemotePlay"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamRemoteStorage */
static void m406(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamRemoteStorage"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamScreenshots */
static void m407(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamScreenshots"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUGC */
static void m408(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUGC"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUser */
static void m409(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUser"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUserStats */
static void m410(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUserStats"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUtils */
static void m411(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamUtils"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void * r = ((F)fn)(self, a0, a1);
    c->eax = steam_proxy(c, r, a1);
}
/* SteamClient020::GetISteamVideo */
static void m412(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_GetISteamVideo"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::ReleaseUser */
static void m413(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamClient_ReleaseUser"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::ActivateActionSet */
static void m416(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_ActivateActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::ActivateActionSetLayer */
static void m417(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_ActivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::DeactivateActionSetLayer */
static void m418(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_DeactivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::DeactivateAllActionSetLayers */
static void m419(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_DeactivateAllActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamController008::GetActionOriginFromXboxOrigin */
static void m420(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetActionOriginFromXboxOrigin"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetActionSetHandle */
static void m421(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetActionSetHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetActiveActionSetLayers */
static void m422(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetActiveActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), 16, 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetAnalogActionHandle */
static void m423(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetAnalogActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetAnalogActionOrigins */
static void m424(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetAnalogActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetConnectedControllers */
static void m425(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetConnectedControllers"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetControllerBindingRevision */
static void m426(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetControllerBindingRevision"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::GetControllerForGamepadIndex */
static void m427(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetControllerForGamepadIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetCurrentActionSet */
static void m428(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetCurrentActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetDigitalActionHandle */
static void m429(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetDigitalActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetDigitalActionOrigins */
static void m430(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetDigitalActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetGamepadIndexForController */
static void m431(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetGamepadIndexForController"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetGlyphForActionOrigin */
static void m432(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetGlyphForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetGlyphForXboxOrigin */
static void m433(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetGlyphForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetInputTypeForHandle */
static void m434(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetInputTypeForHandle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetStringForActionOrigin */
static void m435(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetStringForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetStringForXboxOrigin */
static void m436(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_GetStringForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::Init */
static void m437(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_Init"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::RunFrame */
static void m438(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_RunFrame"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamController008::SetLEDColor */
static void m439(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_SetLEDColor"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamController008::ShowBindingPanel */
static void m440(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_ShowBindingPanel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::Shutdown */
static void m441(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_Shutdown"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::StopAnalogActionMomentum */
static void m442(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_StopAnalogActionMomentum"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::TranslateActionOrigin */
static void m443(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TranslateActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::TriggerHapticPulse */
static void m444(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TriggerHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamController008::TriggerRepeatedHapticPulse */
static void m445(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TriggerRepeatedHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamController008::TriggerVibration */
static void m446(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamController_TriggerVibration"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends017::ActivateGameOverlay */
static void m447(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlay"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayInviteDialog */
static void m448(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialog"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayInviteDialogConnectString */
static void m449(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayInviteDialogConnectString"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayRemotePlayTogetherInviteDialog */
static void m450(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayRemotePlayTogetherInviteDialog"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayToStore */
static void m451(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayToStore"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ActivateGameOverlayToUser */
static void m452(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayToUser"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t a1 = (uint64_t)R64(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ActivateGameOverlayToWebPage */
static void m453(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ClearRichPresence */
static void m454(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ClearRichPresence"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamFriends017::CloseClanChatWindowInSteam */
static void m455(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_CloseClanChatWindowInSteam"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::DownloadClanActivityCounts */
static void m456(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_DownloadClanActivityCounts"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::EnumerateFollowingList */
static void m457(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_EnumerateFollowingList"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::GetChatMemberByIndex */
static void m458(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetChatMemberByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanActivityCounts */
static void m459(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanActivityCounts"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::GetClanByIndex */
static void m460(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanChatMemberCount */
static void m461(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanChatMemberCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanChatMessage */
static void m462(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanChatMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    void *a5 = steam_out(c, R32(args + 24u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanCount */
static void m463(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanName */
static void m464(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetClanOfficerByIndex */
static void m465(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanOfficerByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanOfficerCount */
static void m466(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanOfficerCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanOwner */
static void m467(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanOwner"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanTag */
static void m468(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetClanTag"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetCoplayFriend */
static void m469(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetCoplayFriend"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetCoplayFriendCount */
static void m470(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetCoplayFriendCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFollowerCount */
static void m471(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFollowerCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::GetFriendByIndex */
static void m472(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetFriendCoplayGame */
static void m473(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCoplayGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCoplayTime */
static void m474(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCoplayTime"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCount */
static void m475(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCount"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCountFromSource */
static void m476(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendCountFromSource"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendFromSourceByIndex */
static void m477(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendFromSourceByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetFriendGamePlayed */
static void m478(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendGamePlayed"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 24u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::GetFriendMessage */
static void m479(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendPersonaName */
static void m480(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendPersonaName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendPersonaNameHistory */
static void m481(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendPersonaNameHistory"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendPersonaState */
static void m482(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendPersonaState"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendRelationship */
static void m483(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRelationship"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendRichPresence */
static void m484(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRichPresence"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendRichPresenceKeyByIndex */
static void m485(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRichPresenceKeyByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendRichPresenceKeyCount */
static void m486(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendRichPresenceKeyCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendSteamLevel */
static void m487(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendSteamLevel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupCount */
static void m488(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupIDByIndex */
static void m489(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int16_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupIDByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int16_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupMembersCount */
static void m490(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupMembersCount"), __ATOMIC_RELAXED);
    int16_t a0 = (int16_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupMembersList */
static void m491(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int16_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupMembersList"), __ATOMIC_RELAXED);
    int16_t a0 = (int16_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends017::GetFriendsGroupName */
static void m492(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetFriendsGroupName"), __ATOMIC_RELAXED);
    int16_t a0 = (int16_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetLargeFriendAvatar */
static void m493(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetLargeFriendAvatar"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetMediumFriendAvatar */
static void m494(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetMediumFriendAvatar"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetNumChatsWithUnreadPriorityMessages */
static void m495(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetNumChatsWithUnreadPriorityMessages"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetPersonaName */
static void m496(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetPersonaName"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetPersonaState */
static void m497(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetPersonaState"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetPlayerNickname */
static void m498(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetPlayerNickname"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetSmallFriendAvatar */
static void m499(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetSmallFriendAvatar"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetUserRestrictions */
static void m500(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_GetUserRestrictions"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::HasFriend */
static void m501(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_HasFriend"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::InviteUserToGame */
static void m502(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_InviteUserToGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanChatAdmin */
static void m503(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanChatAdmin"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanChatWindowOpenInSteam */
static void m504(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanChatWindowOpenInSteam"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanOfficialGameGroup */
static void m505(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanOfficialGameGroup"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanPublic */
static void m506(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsClanPublic"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsFollowing */
static void m507(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsFollowing"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::IsUserInSource */
static void m508(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_IsUserInSource"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::JoinClanChatRoom */
static void m509(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_JoinClanChatRoom"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::LeaveClanChatRoom */
static void m510(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_LeaveClanChatRoom"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::OpenClanChatWindowInSteam */
static void m511(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_OpenClanChatWindowInSteam"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::RegisterProtocolInOverlayBrowser */
static void m512(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RegisterProtocolInOverlayBrowser"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::ReplyToFriendMessage */
static void m513(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_ReplyToFriendMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::RequestClanOfficerList */
static void m514(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RequestClanOfficerList"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::RequestFriendRichPresence */
static void m515(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RequestFriendRichPresence"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::RequestUserInformation */
static void m516(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_RequestUserInformation"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SendClanChatMessage */
static void m517(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SendClanChatMessage"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SetInGameVoiceSpeaking */
static void m518(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetInGameVoiceSpeaking"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::SetListenForFriendsMessages */
static void m519(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetListenForFriendsMessages"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SetPersonaName */
static void m520(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetPersonaName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::SetPlayedWith */
static void m521(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetPlayedWith"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::SetRichPresence */
static void m522(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamFriends_SetRichPresence"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::AssociateWithClan */
static void m523(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_AssociateWithClan"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::BLoggedOn */
static void m524(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BLoggedOn"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BSecure */
static void m525(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BSecure"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BUpdateUserData */
static void m526(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BUpdateUserData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BeginAuthSession */
static void m527(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_BeginAuthSession"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::CancelAuthTicket */
static void m528(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_CancelAuthTicket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::ClearAllKeyValues */
static void m529(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_ClearAllKeyValues"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::ComputeNewPlayerCompatibility */
static void m530(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_ComputeNewPlayerCompatibility"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::CreateUnauthenticatedUserConnection */
static void m531(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_CreateUnauthenticatedUserConnection"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer014::EndAuthSession */
static void m532(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_EndAuthSession"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::GetAuthSessionTicket */
static void m533(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetAuthSessionTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::GetGameplayStats */
static void m534(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetGameplayStats"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::GetNextOutgoingPacket */
static void m535(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetNextOutgoingPacket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::GetServerReputation */
static void m536(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetServerReputation"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::GetSteamID */
static void m537(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_GetSteamID"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer014::HandleIncomingPacket */
static void m538(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, uint32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_HandleIncomingPacket"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::LogOff */
static void m539(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_LogOff"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::LogOn */
static void m540(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_LogOn"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::LogOnAnonymous */
static void m541(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_LogOnAnonymous"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamGameServer014::RequestUserGroupStatus */
static void m542(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_RequestUserGroupStatus"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::SendUserConnectAndAuthenticate_DEPRECATED */
static void m543(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate_DEPRECATED"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::SendUserDisconnect_DEPRECATED */
static void m544(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SendUserDisconnect_DEPRECATED"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetAdvertiseServerActive */
static void m545(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetAdvertiseServerActive"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetBotPlayerCount */
static void m546(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetBotPlayerCount"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetDedicatedServer */
static void m547(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetDedicatedServer"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameData */
static void m548(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetGameData"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameDescription */
static void m549(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetGameDescription"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameTags */
static void m550(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetGameTags"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetKeyValue */
static void m551(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetKeyValue"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamGameServer014::SetMapName */
static void m552(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetMapName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetMaxPlayerCount */
static void m553(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetMaxPlayerCount"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetModDir */
static void m554(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetModDir"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetPasswordProtected */
static void m555(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetPasswordProtected"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetProduct */
static void m556(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetProduct"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetRegion */
static void m557(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetRegion"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetServerName */
static void m558(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetServerName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetSpectatorPort */
static void m559(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetSpectatorPort"), __ATOMIC_RELAXED);
    uint16_t a0 = (uint16_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetSpectatorServerName */
static void m560(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_SetSpectatorServerName"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::UserHasLicenseForApp */
static void m561(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_UserHasLicenseForApp"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::WasRestartRequested */
static void m562(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServer_WasRestartRequested"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::ClearUserAchievement */
static void m563(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_ClearUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserAchievement */
static void m564(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_GetUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserStatFloat */
static void m565(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_GetUserStatFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserStatInt32 */
static void m566(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_GetUserStatInt32"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::RequestUserStats */
static void m567(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_RequestUserStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServerStats001::SetUserAchievement */
static void m568(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_SetUserAchievement"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::SetUserStatFloat */
static void m569(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, float);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_SetUserStatFloat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    float a2 = RF(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::SetUserStatInt32 */
static void m570(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_SetUserStatInt32"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::StoreUserStats */
static void m571(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_StoreUserStats"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServerStats001::UpdateUserAvgRateStat */
static void m572(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, float, double);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameServerStats_UpdateUserAvgRateStat"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    float a2 = RF(args + 12u);
    double a3 = RD(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::ActivateActionSet */
static void m573(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_ActivateActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::ActivateActionSetLayer */
static void m574(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_ActivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::BNewDataAvailable */
static void m575(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_BNewDataAvailable"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::BWaitForData */
static void m576(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_BWaitForData"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::DeactivateActionSetLayer */
static void m577(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_DeactivateActionSetLayer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::DeactivateAllActionSetLayers */
static void m578(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_DeactivateAllActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamInput006::EnableDeviceCallbacks */
static void m580(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_EnableDeviceCallbacks"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamInput006::GetActionOriginFromXboxOrigin */
static void m581(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetActionOriginFromXboxOrigin"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetActionSetHandle */
static void m582(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetActionSetHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetActiveActionSetLayers */
static void m583(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetActiveActionSetLayers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), 16, 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetAnalogActionHandle */
static void m584(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetAnalogActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetAnalogActionOrigins */
static void m585(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetAnalogActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetConnectedControllers */
static void m586(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetConnectedControllers"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetControllerForGamepadIndex */
static void m587(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetControllerForGamepadIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetCurrentActionSet */
static void m588(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetCurrentActionSet"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetDeviceBindingRevision */
static void m589(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetDeviceBindingRevision"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::GetDigitalActionHandle */
static void m590(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetDigitalActionHandle"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetDigitalActionOrigins */
static void m591(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetDigitalActionOrigins"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetGamepadIndexForController */
static void m592(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGamepadIndexForController"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetGlyphForActionOrigin_Legacy */
static void m593(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphForActionOrigin_Legacy"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphForXboxOrigin */
static void m594(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphPNGForActionOrigin */
static void m595(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, int32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphPNGForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphSVGForActionOrigin */
static void m596(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetGlyphSVGForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetInputTypeForHandle */
static void m597(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetInputTypeForHandle"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetRemotePlaySessionID */
static void m598(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetRemotePlaySessionID"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetSessionInputConfigurationSettings */
static void m599(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint16_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetSessionInputConfigurationSettings"), __ATOMIC_RELAXED);
    uint16_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetStringForActionOrigin */
static void m600(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForAnalogActionName */
static void m601(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForAnalogActionName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForDigitalActionName */
static void m602(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForDigitalActionName"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForXboxOrigin */
static void m603(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_GetStringForXboxOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::Init */
static void m604(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Init"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::Legacy_TriggerHapticPulse */
static void m605(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Legacy_TriggerHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamInput006::Legacy_TriggerRepeatedHapticPulse */
static void m606(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Legacy_TriggerRepeatedHapticPulse"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamInput006::RunFrame */
static void m607(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_RunFrame"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamInput006::SetInputActionManifestFilePath */
static void m608(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_SetInputActionManifestFilePath"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::SetLEDColor */
static void m609(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_SetLEDColor"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamInput006::ShowBindingPanel */
static void m610(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_ShowBindingPanel"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::Shutdown */
static void m611(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_Shutdown"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::StopAnalogActionMomentum */
static void m612(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_StopAnalogActionMomentum"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::TranslateActionOrigin */
static void m613(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_TranslateActionOrigin"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::TriggerVibration */
static void m615(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_TriggerVibration"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamInput006::TriggerVibrationExtended */
static void m616(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t, uint16_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamInput_TriggerVibrationExtended"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamMatchGameSearch001::AcceptGame */
static void m617(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_AcceptGame"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::AddGameSearchParams */
static void m618(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_AddGameSearchParams"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::CancelRequestPlayersForGame */
static void m619(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_CancelRequestPlayersForGame"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::DeclineGame */
static void m620(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_DeclineGame"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::EndGame */
static void m621(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_EndGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::EndGameSearch */
static void m622(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_EndGameSearch"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::HostConfirmGameStart */
static void m623(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_HostConfirmGameStart"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::RequestPlayersForGame */
static void m624(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_RequestPlayersForGame"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::RetrieveConnectionDetails */
static void m625(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_RetrieveConnectionDetails"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SearchForGameSolo */
static void m626(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SearchForGameSolo"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SearchForGameWithLobby */
static void m627(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SearchForGameWithLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SetConnectionDetails */
static void m628(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SetConnectionDetails"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SetGameHostParams */
static void m629(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SetGameHostParams"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SubmitPlayerResult */
static void m630(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamGameSearch_SubmitPlayerResult"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    int32_t a2 = (int32_t)R32(args + 16u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::AddFavoriteGame */
static void m631(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, uint32_t, uint16_t, uint16_t, uint32_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddFavoriteGame"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint16_t a2 = (uint16_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::AddRequestLobbyListCompatibleMembersFilter */
static void m632(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListCompatibleMembersFilter"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListDistanceFilter */
static void m633(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListDistanceFilter"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListFilterSlotsAvailable */
static void m634(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListFilterSlotsAvailable"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListNearValueFilter */
static void m635(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListNearValueFilter"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamMatchMaking009::AddRequestLobbyListNumericalFilter */
static void m636(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListNumericalFilter"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::AddRequestLobbyListResultCountFilter */
static void m637(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListResultCountFilter"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListStringFilter */
static void m638(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_AddRequestLobbyListStringFilter"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::CreateLobby */
static void m639(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_CreateLobby"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::DeleteLobbyData */
static void m640(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_DeleteLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetFavoriteGame */
static void m641(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *, void *, void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetFavoriteGame"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    void *a4 = steam_out(c, R32(args + 16u), 2u);
    void *a5 = steam_out(c, R32(args + 20u), 4u);
    void *a6 = steam_out(c, R32(args + 24u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetFavoriteGameCount */
static void m642(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetFavoriteGameCount"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyByIndex */
static void m643(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyByIndex"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetLobbyChatEntry */
static void m644(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyChatEntry"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    int32_t a4 = (int32_t)R32(args + 20u);
    void *a5 = steam_out(c, R32(args + 24u), 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyData */
static void m645(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamMatchMaking009::GetLobbyDataByIndex */
static void m646(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyDataByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out_buffer(c, R32(args + 20u), (int64_t)(int32_t)R32(args + 24u), 1u);
    int32_t a5 = (int32_t)R32(args + 24u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetLobbyDataCount */
static void m647(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyDataCount"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyGameServer */
static void m648(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyGameServer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 2u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetLobbyMemberByIndex */
static void m649(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyMemberByIndex"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetLobbyMemberData */
static void m650(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, uint64_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyMemberData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamMatchMaking009::GetLobbyMemberLimit */
static void m651(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyMemberLimit"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyOwner */
static void m652(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetLobbyOwner"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetNumLobbyMembers */
static void m653(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_GetNumLobbyMembers"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::InviteUserToLobby */
static void m654(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_InviteUserToLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::JoinLobby */
static void m655(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_JoinLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::LeaveLobby */
static void m656(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_LeaveLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::RemoveFavoriteGame */
static void m657(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t, uint16_t, uint16_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_RemoveFavoriteGame"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint16_t a2 = (uint16_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::RequestLobbyData */
static void m658(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_RequestLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::RequestLobbyList */
static void m659(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_RequestLobbyList"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::SendLobbyChatMsg */
static void m660(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SendLobbyChatMsg"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLinkedLobby */
static void m661(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLinkedLobby"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyData */
static void m662(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyGameServer */
static void m663(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyGameServer"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint64_t a3 = (uint64_t)R64(args + 16u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* SteamMatchMaking009::SetLobbyJoinable */
static void m664(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyJoinable"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyMemberData */
static void m665(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, const char *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyMemberData"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::SetLobbyMemberLimit */
static void m666(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyMemberLimit"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyOwner */
static void m667(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyOwner"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyType */
static void m668(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmaking_SetLobbyType"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMakingServers002::CancelServerQuery */
static void m670(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamMatchmakingServers_CancelServerQuery"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamNetworking006::AcceptP2PSessionWithUser */
static void m686(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_AcceptP2PSessionWithUser"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::AllowP2PPacketRelay */
static void m687(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_AllowP2PPacketRelay"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CloseP2PChannelWithUser */
static void m688(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_CloseP2PChannelWithUser"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CloseP2PSessionWithUser */
static void m689(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_CloseP2PSessionWithUser"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CreateP2PConnectionSocket */
static void m690(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, int32_t, int32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_CreateP2PConnectionSocket"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool a3 = (R8(args + 16u) != 0);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::DestroyListenSocket */
static void m691(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_DestroyListenSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::DestroySocket */
static void m692(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_DestroySocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::GetMaxPacketSize */
static void m694(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_GetMaxPacketSize"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::GetSocketConnectionType */
static void m696(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_GetSocketConnectionType"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::IsDataAvailable */
static void m698(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_IsDataAvailable"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::IsDataAvailableOnSocket */
static void m699(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_IsDataAvailableOnSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::IsP2PPacketAvailable */
static void m700(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_IsP2PPacketAvailable"), __ATOMIC_RELAXED);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::ReadP2PPacket */
static void m701(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t, void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_ReadP2PPacket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::RetrieveData */
static void m702(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_RetrieveData"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::RetrieveDataFromSocket */
static void m703(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_RetrieveDataFromSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::SendDataOnSocket */
static void m704(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_SendDataOnSocket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::SendP2PPacket */
static void m705(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamNetworking_SendP2PPacket"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::AdvertiseGame */
static void m706(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_AdvertiseGame"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser021::BIsBehindNAT */
static void m707(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsBehindNAT"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneIdentifying */
static void m708(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsPhoneIdentifying"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneRequiringVerification */
static void m709(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsPhoneRequiringVerification"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneVerified */
static void m710(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsPhoneVerified"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsTwoFactorEnabled */
static void m711(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BIsTwoFactorEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BLoggedOn */
static void m712(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BLoggedOn"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BSetDurationControlOnlineState */
static void m713(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BSetDurationControlOnlineState"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BeginAuthSession */
static void m714(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_BeginAuthSession"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser021::CancelAuthTicket */
static void m715(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_CancelAuthTicket"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser021::DecompressVoice */
static void m716(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, uint32_t, void *, uint32_t, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_DecompressVoice"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser021::EndAuthSession */
static void m717(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_EndAuthSession"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser021::GetAuthSessionTicket */
static void m718(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetAuthSessionTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetDurationControl */
static void m720(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetDurationControl"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::GetEncryptedAppTicket */
static void m721(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetEncryptedAppTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::GetGameBadgeLevel */
static void m722(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetGameBadgeLevel"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetHSteamUser */
static void m723(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetHSteamUser"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetMarketEligibility */
static void m724(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetMarketEligibility"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::GetPlayerSteamLevel */
static void m725(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetPlayerSteamLevel"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetSteamID */
static void m726(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetSteamID"), __ATOMIC_RELAXED);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamUser021::GetUserDataFolder */
static void m727(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetUserDataFolder"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::GetVoiceOptimalSampleRate */
static void m728(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_GetVoiceOptimalSampleRate"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::InitiateGameConnection_DEPRECATED */
static void m729(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t, uint32_t, uint16_t, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_InitiateGameConnection_DEPRECATED"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    bool a5 = (R8(args + 24u) != 0);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser021::RequestEncryptedAppTicket */
static void m730(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_RequestEncryptedAppTicket"), __ATOMIC_RELAXED);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::RequestStoreAuthURL */
static void m731(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_RequestStoreAuthURL"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::StartVoiceRecording */
static void m732(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_StartVoiceRecording"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamUser021::StopVoiceRecording */
static void m733(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_StopVoiceRecording"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
/* SteamUser021::TerminateGameConnection_DEPRECATED */
static void m734(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint16_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_TerminateGameConnection_DEPRECATED"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser021::UserHasLicenseForApp */
static void m735(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUser_UserHasLicenseForApp"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::BOverlayNeedsPresent */
static void m736(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_BOverlayNeedsPresent"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::CheckFileSignature */
static void m737(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_CheckFileSignature"), __ATOMIC_RELAXED);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUtils010::DismissFloatingGamepadTextInput */
static void m738(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_DismissFloatingGamepadTextInput"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::FilterText */
static void m739(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, uint64_t, const char *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_FilterText"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetAPICallFailureReason */
static void m740(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetAPICallFailureReason"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetAPICallResult */
static void m741(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t, int32_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetAPICallResult"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetAppID */
static void m742(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetAppID"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetConnectedUniverse */
static void m743(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetConnectedUniverse"), __ATOMIC_RELAXED);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetCurrentBatteryPower */
static void m744(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint8_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetCurrentBatteryPower"), __ATOMIC_RELAXED);
    uint8_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetEnteredGamepadTextInput */
static void m745(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetEnteredGamepadTextInput"), __ATOMIC_RELAXED);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetEnteredGamepadTextLength */
static void m746(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetEnteredGamepadTextLength"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetIPCCallCount */
static void m747(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetIPCCallCount"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetIPCountry */
static void m748(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetIPCountry"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils010::GetIPv6ConnectivityState */
static void m749(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetIPv6ConnectivityState"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetImageRGBA */
static void m750(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetImageRGBA"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetImageSize */
static void m751(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetImageSize"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetSecondsSinceAppActive */
static void m752(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetSecondsSinceAppActive"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetSecondsSinceComputerActive */
static void m753(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetSecondsSinceComputerActive"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetServerRealTime */
static void m754(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetServerRealTime"), __ATOMIC_RELAXED);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetSteamUILanguage */
static void m755(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_GetSteamUILanguage"), __ATOMIC_RELAXED);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils010::InitFilterText */
static void m756(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_InitFilterText"), __ATOMIC_RELAXED);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsAPICallCompleted */
static void m757(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsAPICallCompleted"), __ATOMIC_RELAXED);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsOverlayEnabled */
static void m758(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsOverlayEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamChinaLauncher */
static void m759(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamChinaLauncher"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamInBigPictureMode */
static void m760(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamInBigPictureMode"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamRunningInVR */
static void m761(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamRunningInVR"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamRunningOnSteamDeck */
static void m762(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsSteamRunningOnSteamDeck"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsVRHeadsetStreamingEnabled */
static void m763(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_IsVRHeadsetStreamingEnabled"), __ATOMIC_RELAXED);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::SetGameLauncherMode */
static void m764(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetGameLauncherMode"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils010::SetOverlayNotificationInset */
static void m765(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetOverlayNotificationInset"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUtils010::SetOverlayNotificationPosition */
static void m766(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetOverlayNotificationPosition"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUtils010::SetVRHeadsetStreamingEnabled */
static void m767(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_SetVRHeadsetStreamingEnabled"), __ATOMIC_RELAXED);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils010::ShowFloatingGamepadTextInput */
static void m769(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, int32_t, int32_t, int32_t);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_ShowFloatingGamepadTextInput"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::ShowGamepadTextInput */
static void m770(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *, uint32_t, const char *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_ShowGamepadTextInput"), __ATOMIC_RELAXED);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::StartVRDashboard */
static void m771(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    static void *cache;
    void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);
    if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "SteamAPI_ISteamUtils_StartVRDashboard"), __ATOMIC_RELAXED);
    ((F)fn)(self);
}
const SteamInterfaceInfo steam_interfaces[] = {
    {"SteamClient020", "ISteamClient", 41u},
    {"SteamUser021", "ISteamUser", 32u},
    {"SteamFriends017", "ISteamFriends", 76u},
    {"SteamUtils010", "ISteamUtils", 38u},
    {"SteamMatchMaking009", "ISteamMatchmaking", 38u},
    {"SteamMatchGameSearch001", "ISteamGameSearch", 14u},
    {"SteamMatchMakingServers002", "ISteamMatchmakingServers", 17u},
    {"STEAMUSERSTATS_INTERFACE_VERSION012", "ISteamUserStats", 45u},
    {"STEAMAPPS_INTERFACE_VERSION008", "ISteamApps", 29u},
    {"SteamNetworking006", "ISteamNetworking", 22u},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016", "ISteamRemoteStorage", 59u},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003", "ISteamScreenshots", 9u},
    {"STEAMHTTP_INTERFACE_VERSION003", "ISteamHTTP", 25u},
    {"SteamController008", "ISteamController", 34u},
    {"STEAMUGC_INTERFACE_VERSION016", "ISteamUGC", 86u},
    {"STEAMAPPLIST_INTERFACE_VERSION001", "ISteamAppList", 5u},
    {"STEAMMUSIC_INTERFACE_VERSION001", "ISteamMusic", 9u},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001", "ISteamMusicRemote", 32u},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005", "ISteamHTMLSurface", 39u},
    {"STEAMINVENTORY_INTERFACE_V003", "ISteamInventory", 38u},
    {"STEAMVIDEO_INTERFACE_V002", "ISteamVideo", 4u},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001", "ISteamParentalSettings", 6u},
    {"SteamInput006", "ISteamInput", 47u},
    {"SteamGameServer014", "ISteamGameServer", 42u},
    {"SteamGameServerStats001", "ISteamGameServerStats", 10u},
};
const size_t steam_interface_count = 25;
const SteamMethodInfo steam_methods[] = {
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetAppBuildId", 15u, 4u, 0, m0, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetAppInstallDir", 15u, 3u, 0, m1, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetAppName", 15u, 2u, 0, m2, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetInstalledApps", 15u, 1u, 0, m3, NULL},
    {"STEAMAPPLIST_INTERFACE_VERSION001::GetNumInstalledApps", 15u, 0u, 0, m4, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BGetDLCDataByIndex", 8u, 11u, 0, m5, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsAppInstalled", 8u, 19u, 0, m6, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsCybercafe", 8u, 2u, 0, m7, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsDlcInstalled", 8u, 7u, 0, m8, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsLowViolence", 8u, 1u, 0, m9, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribed", 8u, 0u, 0, m10, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedApp", 8u, 6u, 0, m11, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFamilySharing", 8u, 27u, 0, m12, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFreeWeekend", 8u, 9u, 0, m13, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsTimedTrial", 8u, 28u, 0, m14, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::BIsVACBanned", 8u, 3u, 0, m15, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAppBuildId", 8u, 23u, 0, m16, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAppInstallDir", 8u, 18u, 0, m17, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAppOwner", 8u, 20u, 1, m18, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetAvailableGameLanguages", 8u, 5u, 0, m19, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetCurrentBetaName", 8u, 15u, 0, m20, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetCurrentGameLanguage", 8u, 4u, 0, m21, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetDLCCount", 8u, 10u, 0, m22, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetDlcDownloadProgress", 8u, 22u, 0, m23, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetEarliestPurchaseUnixTime", 8u, 8u, 0, m24, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetFileDetails", 8u, 25u, 0, m25, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetInstalledDepots", 8u, 17u, 0, m26, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetLaunchCommandLine", 8u, 26u, 0, m27, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::GetLaunchQueryParam", 8u, 21u, 0, m28, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::InstallDLC", 8u, 12u, 0, m29, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::MarkContentCorrupt", 8u, 16u, 0, m30, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::RequestAllProofOfPurchaseKeys", 8u, 24u, 0, m31, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::RequestAppProofOfPurchaseKey", 8u, 14u, 0, m32, NULL},
    {"STEAMAPPS_INTERFACE_VERSION008::UninstallDLC", 8u, 13u, 0, m33, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::AddHeader", 18u, 12u, 0, m34, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::AllowStartRequest", 18u, 36u, 0, m35, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::CopyToClipboard", 18u, 26u, 0, m36, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::CreateBrowser", 18u, 4u, 0, m37, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::ExecuteJavascript", 18u, 13u, 0, m38, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::FileLoadDialogResponse", 18u, 38u, 0, NULL, "tampon IntPtr pchSelectedFiles sans taille"},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Find", 18u, 28u, 0, m40, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::GetLinkAtPosition", 18u, 30u, 0, m41, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoBack", 18u, 10u, 0, m42, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoForward", 18u, 11u, 0, m43, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Init", 18u, 2u, 0, m44, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::JSDialogResponse", 18u, 37u, 0, m45, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyChar", 18u, 21u, 0, m46, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyDown", 18u, 19u, 0, m47, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyUp", 18u, 20u, 0, m48, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::LoadURL", 18u, 6u, 0, m49, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDoubleClick", 18u, 16u, 0, m50, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDown", 18u, 15u, 0, m51, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseMove", 18u, 17u, 0, m52, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseUp", 18u, 14u, 0, m53, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseWheel", 18u, 18u, 0, m54, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::OpenDeveloperTools", 18u, 35u, 0, m55, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::PasteFromClipboard", 18u, 27u, 0, m56, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Reload", 18u, 9u, 0, m57, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::RemoveBrowser", 18u, 5u, 0, m58, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetBackgroundMode", 18u, 33u, 0, m59, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetCookie", 18u, 31u, 0, m60, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetDPIScalingFactor", 18u, 34u, 0, m61, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetHorizontalScroll", 18u, 22u, 0, m62, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetKeyFocus", 18u, 24u, 0, m63, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetPageScaleFactor", 18u, 32u, 0, m64, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetSize", 18u, 7u, 0, m65, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetVerticalScroll", 18u, 23u, 0, m66, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::Shutdown", 18u, 3u, 0, m67, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopFind", 18u, 29u, 0, m68, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopLoad", 18u, 8u, 0, m69, NULL},
    {"STEAMHTMLSURFACE_INTERFACE_VERSION_005::ViewSource", 18u, 25u, 0, m70, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::CreateCookieContainer", 12u, 17u, 0, m71, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::CreateHTTPRequest", 12u, 0u, 0, m72, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::DeferHTTPRequest", 12u, 7u, 0, m73, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPDownloadProgressPct", 12u, 15u, 0, m74, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPRequestWasTimedOut", 12u, 24u, 0, m75, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodyData", 12u, 12u, 0, m76, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodySize", 12u, 11u, 0, m77, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderSize", 12u, 9u, 0, m78, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderValue", 12u, 10u, 0, m79, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::GetHTTPStreamingResponseBodyData", 12u, 13u, 0, m80, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::PrioritizeHTTPRequest", 12u, 8u, 0, m81, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::ReleaseCookieContainer", 12u, 18u, 0, m82, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::ReleaseHTTPRequest", 12u, 14u, 0, m83, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequest", 12u, 5u, 0, m84, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequestAndStreamResponse", 12u, 6u, 0, m85, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetCookie", 12u, 19u, 0, m86, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestAbsoluteTimeoutMS", 12u, 23u, 0, m87, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestContextValue", 12u, 1u, 0, m88, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestCookieContainer", 12u, 20u, 0, m89, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestGetOrPostParameter", 12u, 4u, 0, m90, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestHeaderValue", 12u, 3u, 0, m91, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestNetworkActivityTimeout", 12u, 2u, 0, m92, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRawPostBody", 12u, 16u, 0, NULL, "tampon byte[] pubBody sans taille"},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRequiresVerifiedCertificate", 12u, 22u, 0, m94, NULL},
    {"STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestUserAgentInfo", 12u, 21u, 0, m95, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::AddPromoItem", 19u, 12u, 0, m96, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::AddPromoItems", 19u, 13u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::CheckResultSteamID", 19u, 4u, 0, m98, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::ConsumeItem", 19u, 14u, 0, m99, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::DeserializeResult", 19u, 9u, 0, m100, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::DestroyResult", 19u, 5u, 0, m101, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::ExchangeItems", 19u, 15u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayGenerate sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GenerateItems", 19u, 10u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetAllItems", 19u, 6u, 0, m104, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetEligiblePromoItemDefinitionIDs", 19u, 24u, 0, NULL, "tampon inout SteamItemDef_t[] pItemDefIDs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemDefinitionIDs", 19u, 21u, 0, NULL, "tampon inout SteamItemDef_t[] pItemDefIDs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemDefinitionProperty", 19u, 22u, 0, NULL, "tampon IntPtr pchValueBuffer sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemPrice", 19u, 29u, 0, m108, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemsByID", 19u, 7u, 0, m109, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetItemsWithPrices", 19u, 28u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetNumItemsWithPrices", 19u, 27u, 0, m111, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultItemProperty", 19u, 2u, 0, NULL, "tampon IntPtr pchValueBuffer sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultItems", 19u, 1u, 0, NULL, "tableau inout SteamItemDetails_t[] pOutItemsArray"},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultStatus", 19u, 0u, 0, m114, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GetResultTimestamp", 19u, 3u, 0, m115, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::GrantPromoItems", 19u, 11u, 0, m116, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::InspectItem", 19u, 37u, 0, m117, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::LoadItemDefinitions", 19u, 20u, 0, m118, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::RemoveProperty", 19u, 31u, 0, m119, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::RequestEligiblePromoItemDefinitionsIDs", 19u, 23u, 0, m120, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::RequestPrices", 19u, 26u, 0, m121, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SendItemDropHeartbeat", 19u, 17u, 0, m122, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SerializeResult", 19u, 8u, 0, NULL, "tampon byte[] pOutBuffer sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyBool", 19u, 33u, 0, m124, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyFloat", 19u, 35u, 0, m125, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyInt64", 19u, 34u, 0, m126, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SetPropertyString", 19u, 32u, 0, m127, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::StartPurchase", 19u, 25u, 0, NULL, "tampon inout SteamItemDef_t[] pArrayItemDefs sans taille"},
    {"STEAMINVENTORY_INTERFACE_V003::StartUpdateProperties", 19u, 30u, 0, m129, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::SubmitUpdateProperties", 19u, 36u, 0, m130, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::TransferItemQuantity", 19u, 16u, 0, m131, NULL},
    {"STEAMINVENTORY_INTERFACE_V003::TriggerItemDrop", 19u, 18u, 0, m132, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::BActivationSuccess", 17u, 3u, 0, m133, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::BIsCurrentMusicRemote", 17u, 2u, 0, m134, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryDidChange", 17u, 21u, 0, m135, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryIsAvailable", 17u, 17u, 0, m136, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryWillChange", 17u, 16u, 0, m137, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::DeregisterSteamMusicRemote", 17u, 1u, 0, m138, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableLooped", 17u, 9u, 0, m139, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayNext", 17u, 7u, 0, m140, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayPrevious", 17u, 6u, 0, m141, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlaylists", 17u, 11u, 0, m142, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableQueue", 17u, 10u, 0, m143, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableShuffled", 17u, 8u, 0, m144, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistDidChange", 17u, 31u, 0, m145, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistWillChange", 17u, 27u, 0, m146, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueDidChange", 17u, 26u, 0, m147, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueWillChange", 17u, 22u, 0, m148, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::RegisterSteamMusicRemote", 17u, 0u, 0, m149, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetPlaylistEntries", 17u, 28u, 0, m150, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetQueueEntries", 17u, 23u, 0, m151, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentPlaylistEntry", 17u, 30u, 0, m152, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentQueueEntry", 17u, 25u, 0, m153, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetDisplayName", 17u, 4u, 0, m154, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPNGIcon_64x64", 17u, 5u, 0, m155, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPlaylistEntry", 17u, 29u, 0, m156, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::SetQueueEntry", 17u, 24u, 0, m157, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryCoverArt", 17u, 20u, 0, m158, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryElapsedSeconds", 17u, 19u, 0, m159, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryText", 17u, 18u, 0, m160, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateLooped", 17u, 14u, 0, m161, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdatePlaybackStatus", 17u, 12u, 0, m162, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateShuffled", 17u, 13u, 0, m163, NULL},
    {"STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateVolume", 17u, 15u, 0, m164, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::BIsEnabled", 16u, 0u, 0, m165, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::BIsPlaying", 16u, 1u, 0, m166, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::GetPlaybackStatus", 16u, 2u, 0, m167, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::GetVolume", 16u, 8u, 0, m168, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::Pause", 16u, 4u, 0, m169, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::Play", 16u, 3u, 0, m170, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::PlayNext", 16u, 6u, 0, m171, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::PlayPrevious", 16u, 5u, 0, m172, NULL},
    {"STEAMMUSIC_INTERFACE_VERSION001::SetVolume", 16u, 7u, 0, m173, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppBlocked", 21u, 2u, 0, m174, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppInBlockList", 21u, 3u, 0, m175, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureBlocked", 21u, 4u, 0, m176, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureInBlockList", 21u, 5u, 0, m177, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockEnabled", 21u, 0u, 0, m178, NULL},
    {"STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockLocked", 21u, 1u, 0, m179, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::BeginFileWriteBatch", 10u, 57u, 0, m180, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::CommitPublishedFileUpdate", 10u, 38u, 0, m181, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::CreatePublishedFileUpdateRequest", 10u, 31u, 0, m182, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::DeletePublishedFile", 10u, 40u, 0, m183, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EndFileWriteBatch", 10u, 58u, 0, m184, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedFilesByUserAction", 10u, 52u, 0, m185, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedWorkshopFiles", 10u, 53u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserPublishedFiles", 10u, 41u, 0, m187, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSharedWorkshopFiles", 10u, 49u, 0, NULL, "tampon IntPtr pRequiredTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSubscribedFiles", 10u, 43u, 0, m189, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileDelete", 10u, 6u, 0, m190, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileExists", 10u, 13u, 0, m191, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileForget", 10u, 5u, 0, m192, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FilePersisted", 10u, 14u, 0, m193, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileRead", 10u, 1u, 0, m194, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsync", 10u, 3u, 0, m195, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsyncComplete", 10u, 4u, 0, m196, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileShare", 10u, 7u, 0, m197, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWrite", 10u, 0u, 0, m198, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteAsync", 10u, 2u, 0, m199, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamCancel", 10u, 12u, 0, m200, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamClose", 10u, 11u, 0, m201, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamOpen", 10u, 9u, 0, m202, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamWriteChunk", 10u, 10u, 0, m203, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCCount", 10u, 28u, 0, m204, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCHandle", 10u, 29u, 0, m205, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileCount", 10u, 18u, 0, m206, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileNameAndSize", 10u, 19u, 0, m207, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileSize", 10u, 15u, 0, m208, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileTimestamp", 10u, 16u, 0, m209, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChange", 10u, 56u, 0, m210, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChangeCount", 10u, 55u, 0, m211, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedFileDetails", 10u, 39u, 0, m212, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedItemVoteDetails", 10u, 46u, 0, m213, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetQuota", 10u, 20u, 0, m214, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetSyncPlatforms", 10u, 17u, 0, m215, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDetails", 10u, 26u, 0, m216, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDownloadProgress", 10u, 25u, 0, m217, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUserPublishedItemVoteDetails", 10u, 48u, 0, m218, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForAccount", 10u, 21u, 0, m219, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForApp", 10u, 22u, 0, m220, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetCloudEnabledForApp", 10u, 23u, 0, m221, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetSyncPlatforms", 10u, 8u, 0, m222, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetUserPublishedFileAction", 10u, 51u, 0, m223, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SubscribePublishedFile", 10u, 42u, 0, m224, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownload", 10u, 24u, 0, m225, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownloadToLocation", 10u, 54u, 0, m226, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCRead", 10u, 27u, 0, m227, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UnsubscribePublishedFile", 10u, 44u, 0, m228, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileDescription", 10u, 35u, 0, m229, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileFile", 10u, 32u, 0, m230, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFilePreviewFile", 10u, 33u, 0, m231, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileSetChangeDescription", 10u, 45u, 0, m232, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTags", 10u, 37u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTitle", 10u, 34u, 0, m234, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileVisibility", 10u, 36u, 0, m235, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdateUserPublishedItemVote", 10u, 47u, 0, m236, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::AddScreenshotToLibrary", 11u, 1u, 0, m237, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::AddVRScreenshotToLibrary", 11u, 8u, 0, m238, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::HookScreenshots", 11u, 3u, 0, m239, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::IsScreenshotsHooked", 11u, 7u, 0, m240, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::SetLocation", 11u, 4u, 0, m241, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TagPublishedFile", 11u, 6u, 0, m242, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TagUser", 11u, 5u, 0, m243, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TriggerScreenshot", 11u, 2u, 0, m244, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::WriteScreenshot", 11u, 0u, 0, m245, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddAppDependency", 14u, 80u, 0, m246, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddDependency", 14u, 78u, 0, m247, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddExcludedTag", 14u, 21u, 0, m248, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemKeyValueTag", 14u, 53u, 0, m249, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemPreviewFile", 14u, 54u, 0, m250, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemPreviewVideo", 14u, 55u, 0, m251, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemToFavorites", 14u, 63u, 0, m252, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredKeyValueTag", 14u, 38u, 0, m253, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredTag", 14u, 19u, 0, m254, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredTagGroup", 14u, 20u, 0, NULL, "tampon IntPtr pTagGroups sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::BInitWorkshopForGameServer", 14u, 73u, 0, m256, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateItem", 14u, 40u, 0, m257, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestCursor", 14u, 2u, 0, m258, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestPage", 14u, 1u, 0, m259, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryUGCDetailsRequest", 14u, 3u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryUserUGCRequest", 14u, 0u, 0, m261, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::DeleteItem", 14u, 83u, 0, m262, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::DownloadItem", 14u, 72u, 0, m263, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetAppDependencies", 14u, 82u, 0, m264, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemDownloadInfo", 14u, 71u, 0, m265, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemInstallInfo", 14u, 70u, 0, m266, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemState", 14u, 69u, 0, m267, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemUpdateProgress", 14u, 60u, 0, m268, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetNumSubscribedItems", 14u, 67u, 0, m269, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryFirstUGCKeyValueTag", 14u, 17u, 0, m270, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCChildren", 14u, 11u, 0, m271, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCKeyValueTag", 14u, 16u, 0, m272, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCMetadata", 14u, 10u, 0, m273, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumAdditionalPreviews", 14u, 13u, 0, m274, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumKeyValueTags", 14u, 15u, 0, m275, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumTags", 14u, 6u, 0, m276, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCPreviewURL", 14u, 9u, 0, m277, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCResult", 14u, 5u, 0, NULL, "param\u00e8tre out SteamUGCDetails_t pDetails"},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCStatistic", 14u, 12u, 0, m279, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTag", 14u, 7u, 0, m280, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTagDisplayName", 14u, 8u, 0, m281, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetSubscribedItems", 14u, 68u, 0, m282, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetUserItemVote", 14u, 62u, 0, m283, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetWorkshopEULAStatus", 14u, 85u, 0, m284, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::ReleaseQueryUGCRequest", 14u, 18u, 0, m285, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveAllItemKeyValueTags", 14u, 51u, 0, m286, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveAppDependency", 14u, 81u, 0, m287, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveDependency", 14u, 79u, 0, m288, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemFromFavorites", 14u, 64u, 0, m289, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemKeyValueTags", 14u, 52u, 0, m290, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemPreview", 14u, 58u, 0, m291, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RequestUGCDetails", 14u, 39u, 0, m292, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SendQueryUGCRequest", 14u, 4u, 0, m293, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetAllowCachedResponse", 14u, 31u, 0, m294, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetAllowLegacyUpload", 14u, 50u, 0, m295, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetCloudFileNameFilter", 14u, 32u, 0, m296, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemContent", 14u, 48u, 0, m297, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemDescription", 14u, 43u, 0, m298, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemMetadata", 14u, 45u, 0, m299, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemPreview", 14u, 49u, 0, m300, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemTags", 14u, 47u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemTitle", 14u, 42u, 0, m302, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemUpdateLanguage", 14u, 44u, 0, m303, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemVisibility", 14u, 46u, 0, m304, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetLanguage", 14u, 30u, 0, m305, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetMatchAnyTag", 14u, 33u, 0, m306, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetRankedByTrendDays", 14u, 35u, 0, m307, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnAdditionalPreviews", 14u, 27u, 0, m308, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnChildren", 14u, 26u, 0, m309, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnKeyValueTags", 14u, 23u, 0, m310, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnLongDescription", 14u, 24u, 0, m311, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnMetadata", 14u, 25u, 0, m312, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnOnlyIDs", 14u, 22u, 0, m313, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnPlaytimeStats", 14u, 29u, 0, m314, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnTotalOnly", 14u, 28u, 0, m315, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetSearchText", 14u, 34u, 0, m316, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetTimeCreatedDateRange", 14u, 36u, 0, m317, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetTimeUpdatedDateRange", 14u, 37u, 0, m318, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetUserItemVote", 14u, 61u, 0, m319, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::ShowWorkshopEULA", 14u, 84u, 0, m320, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StartItemUpdate", 14u, 41u, 0, m321, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StartPlaytimeTracking", 14u, 75u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTracking", 14u, 76u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTrackingForAllItems", 14u, 77u, 0, m324, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SubmitItemUpdate", 14u, 59u, 0, m325, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SubscribeItem", 14u, 65u, 0, m326, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SuspendDownloads", 14u, 74u, 0, m327, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UnsubscribeItem", 14u, 66u, 0, m328, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewFile", 14u, 56u, 0, m329, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewVideo", 14u, 57u, 0, m330, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::AttachLeaderboardUGC", 7u, 32u, 0, m331, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::ClearAchievement", 7u, 8u, 0, m332, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntries", 7u, 28u, 0, m333, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntriesForUsers", 7u, 29u, 0, m334, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::FindLeaderboard", 7u, 23u, 0, m335, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::FindOrCreateLeaderboard", 7u, 22u, 0, m336, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievement", 7u, 6u, 0, m337, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAchievedPercent", 7u, 37u, 0, m338, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAndUnlockTime", 7u, 9u, 0, m339, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementDisplayAttribute", 7u, 12u, 0, m340, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementIcon", 7u, 11u, 0, m341, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementName", 7u, 15u, 0, m342, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsFloat", 7u, 44u, 0, m343, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsInt32", 7u, 43u, 0, m344, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetDownloadedLeaderboardEntry", 7u, 30u, 0, m345, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatDouble", 7u, 40u, 0, m346, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryDouble", 7u, 42u, 0, m347, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryInt64", 7u, 41u, 0, m348, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatInt64", 7u, 39u, 0, m349, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardDisplayType", 7u, 27u, 0, m350, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardEntryCount", 7u, 25u, 0, m351, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardName", 7u, 24u, 0, m352, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardSortMethod", 7u, 26u, 0, m353, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetMostAchievedAchievementInfo", 7u, 35u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNextMostAchievedAchievementInfo", 7u, 36u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNumAchievements", 7u, 14u, 0, m356, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNumberOfCurrentPlayers", 7u, 33u, 0, m357, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetStatFloat", 7u, 2u, 0, m358, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetStatInt32", 7u, 1u, 0, m359, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievement", 7u, 19u, 0, m360, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievementAndUnlockTime", 7u, 20u, 0, m361, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatFloat", 7u, 18u, 0, m362, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatInt32", 7u, 17u, 0, m363, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::IndicateAchievementProgress", 7u, 13u, 0, m364, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestCurrentStats", 7u, 0u, 0, m365, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalAchievementPercentages", 7u, 34u, 0, m366, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalStats", 7u, 38u, 0, m367, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestUserStats", 7u, 16u, 0, m368, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::ResetAllStats", 7u, 21u, 0, m369, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetAchievement", 7u, 7u, 0, m370, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetStatFloat", 7u, 4u, 0, m371, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetStatInt32", 7u, 3u, 0, m372, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::StoreStats", 7u, 10u, 0, m373, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::UpdateAvgRateStat", 7u, 5u, 0, m374, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::UploadLeaderboardScore", 7u, 31u, 0, m375, NULL},
    {"STEAMVIDEO_INTERFACE_V002::GetOPFSettings", 20u, 2u, 0, m376, NULL},
    {"STEAMVIDEO_INTERFACE_V002::GetOPFStringForApp", 20u, 3u, 0, NULL, "tampon IntPtr pchBuffer sans taille"},
    {"STEAMVIDEO_INTERFACE_V002::GetVideoURL", 20u, 0u, 0, m378, NULL},
    {"STEAMVIDEO_INTERFACE_V002::IsBroadcasting", 20u, 1u, 0, m379, NULL},
    {"SteamClient020::BReleaseSteamPipe", 0u, 1u, 0, m380, NULL},
    {"SteamClient020::BShutdownIfAllPipesClosed", 0u, 23u, 0, m381, NULL},
    {"SteamClient020::ConnectToGlobalUser", 0u, 2u, 0, m382, NULL},
    {"SteamClient020::CreateLocalUser", 0u, 3u, 0, m383, NULL},
    {"SteamClient020::CreateSteamPipe", 0u, 0u, 0, m384, NULL},
    {"SteamClient020::GetIPCCallCount", 0u, 21u, 0, m385, NULL},
    {"SteamClient020::GetISteamAppList", 0u, 28u, 0, m386, NULL},
    {"SteamClient020::GetISteamApps", 0u, 15u, 0, m387, NULL},
    {"SteamClient020::GetISteamController", 0u, 26u, 0, m388, NULL},
    {"SteamClient020::GetISteamFriends", 0u, 8u, 0, m389, NULL},
    {"SteamClient020::GetISteamGameSearch", 0u, 19u, 0, m390, NULL},
    {"SteamClient020::GetISteamGameServer", 0u, 6u, 0, m391, NULL},
    {"SteamClient020::GetISteamGameServerStats", 0u, 14u, 0, m392, NULL},
    {"SteamClient020::GetISteamGenericInterface", 0u, 12u, 0, m393, NULL},
    {"SteamClient020::GetISteamHTMLSurface", 0u, 31u, 0, m394, NULL},
    {"SteamClient020::GetISteamHTTP", 0u, 24u, 0, m395, NULL},
    {"SteamClient020::GetISteamInput", 0u, 38u, 0, m396, NULL},
    {"SteamClient020::GetISteamInventory", 0u, 35u, 0, m397, NULL},
    {"SteamClient020::GetISteamMatchmaking", 0u, 10u, 0, m398, NULL},
    {"SteamClient020::GetISteamMatchmakingServers", 0u, 11u, 0, m399, NULL},
    {"SteamClient020::GetISteamMusic", 0u, 29u, 0, m400, NULL},
    {"SteamClient020::GetISteamMusicRemote", 0u, 30u, 0, m401, NULL},
    {"SteamClient020::GetISteamNetworking", 0u, 16u, 0, m402, NULL},
    {"SteamClient020::GetISteamParentalSettings", 0u, 37u, 0, m403, NULL},
    {"SteamClient020::GetISteamParties", 0u, 39u, 0, m404, NULL},
    {"SteamClient020::GetISteamRemotePlay", 0u, 40u, 0, m405, NULL},
    {"SteamClient020::GetISteamRemoteStorage", 0u, 17u, 0, m406, NULL},
    {"SteamClient020::GetISteamScreenshots", 0u, 18u, 0, m407, NULL},
    {"SteamClient020::GetISteamUGC", 0u, 27u, 0, m408, NULL},
    {"SteamClient020::GetISteamUser", 0u, 5u, 0, m409, NULL},
    {"SteamClient020::GetISteamUserStats", 0u, 13u, 0, m410, NULL},
    {"SteamClient020::GetISteamUtils", 0u, 9u, 0, m411, NULL},
    {"SteamClient020::GetISteamVideo", 0u, 36u, 0, m412, NULL},
    {"SteamClient020::ReleaseUser", 0u, 4u, 0, m413, NULL},
    {"SteamClient020::SetLocalIPBinding", 0u, 7u, 0, NULL, "param\u00e8tre ref SteamIPAddress_t unIP"},
    {"SteamClient020::SetWarningMessageHook", 0u, 22u, 0, steam_manual_client_warning_hook, NULL},
    {"SteamController008::ActivateActionSet", 13u, 5u, 0, m416, NULL},
    {"SteamController008::ActivateActionSetLayer", 13u, 7u, 0, m417, NULL},
    {"SteamController008::DeactivateActionSetLayer", 13u, 8u, 0, m418, NULL},
    {"SteamController008::DeactivateAllActionSetLayers", 13u, 9u, 0, m419, NULL},
    {"SteamController008::GetActionOriginFromXboxOrigin", 13u, 31u, 0, m420, NULL},
    {"SteamController008::GetActionSetHandle", 13u, 4u, 0, m421, NULL},
    {"SteamController008::GetActiveActionSetLayers", 13u, 10u, 0, m422, NULL},
    {"SteamController008::GetAnalogActionHandle", 13u, 14u, 0, m423, NULL},
    {"SteamController008::GetAnalogActionOrigins", 13u, 16u, 0, m424, NULL},
    {"SteamController008::GetConnectedControllers", 13u, 3u, 0, m425, NULL},
    {"SteamController008::GetControllerBindingRevision", 13u, 33u, 0, m426, NULL},
    {"SteamController008::GetControllerForGamepadIndex", 13u, 27u, 0, m427, NULL},
    {"SteamController008::GetCurrentActionSet", 13u, 6u, 0, m428, NULL},
    {"SteamController008::GetDigitalActionHandle", 13u, 11u, 0, m429, NULL},
    {"SteamController008::GetDigitalActionOrigins", 13u, 13u, 0, m430, NULL},
    {"SteamController008::GetGamepadIndexForController", 13u, 28u, 0, m431, NULL},
    {"SteamController008::GetGlyphForActionOrigin", 13u, 17u, 0, m432, NULL},
    {"SteamController008::GetGlyphForXboxOrigin", 13u, 30u, 0, m433, NULL},
    {"SteamController008::GetInputTypeForHandle", 13u, 26u, 0, m434, NULL},
    {"SteamController008::GetStringForActionOrigin", 13u, 18u, 0, m435, NULL},
    {"SteamController008::GetStringForXboxOrigin", 13u, 29u, 0, m436, NULL},
    {"SteamController008::Init", 13u, 0u, 0, m437, NULL},
    {"SteamController008::RunFrame", 13u, 2u, 0, m438, NULL},
    {"SteamController008::SetLEDColor", 13u, 24u, 0, m439, NULL},
    {"SteamController008::ShowBindingPanel", 13u, 25u, 0, m440, NULL},
    {"SteamController008::Shutdown", 13u, 1u, 0, m441, NULL},
    {"SteamController008::StopAnalogActionMomentum", 13u, 19u, 0, m442, NULL},
    {"SteamController008::TranslateActionOrigin", 13u, 32u, 0, m443, NULL},
    {"SteamController008::TriggerHapticPulse", 13u, 21u, 0, m444, NULL},
    {"SteamController008::TriggerRepeatedHapticPulse", 13u, 22u, 0, m445, NULL},
    {"SteamController008::TriggerVibration", 13u, 23u, 0, m446, NULL},
    {"SteamFriends017::ActivateGameOverlay", 2u, 28u, 0, m447, NULL},
    {"SteamFriends017::ActivateGameOverlayInviteDialog", 2u, 33u, 0, m448, NULL},
    {"SteamFriends017::ActivateGameOverlayInviteDialogConnectString", 2u, 75u, 0, m449, NULL},
    {"SteamFriends017::ActivateGameOverlayRemotePlayTogetherInviteDialog", 2u, 73u, 0, m450, NULL},
    {"SteamFriends017::ActivateGameOverlayToStore", 2u, 31u, 0, m451, NULL},
    {"SteamFriends017::ActivateGameOverlayToUser", 2u, 29u, 0, m452, NULL},
    {"SteamFriends017::ActivateGameOverlayToWebPage", 2u, 30u, 0, m453, NULL},
    {"SteamFriends017::ClearRichPresence", 2u, 44u, 0, m454, NULL},
    {"SteamFriends017::CloseClanChatWindowInSteam", 2u, 63u, 0, m455, NULL},
    {"SteamFriends017::DownloadClanActivityCounts", 2u, 23u, 0, m456, NULL},
    {"SteamFriends017::EnumerateFollowingList", 2u, 69u, 0, m457, NULL},
    {"SteamFriends017::GetChatMemberByIndex", 2u, 57u, 1, m458, NULL},
    {"SteamFriends017::GetClanActivityCounts", 2u, 22u, 0, m459, NULL},
    {"SteamFriends017::GetClanByIndex", 2u, 19u, 1, m460, NULL},
    {"SteamFriends017::GetClanChatMemberCount", 2u, 56u, 0, m461, NULL},
    {"SteamFriends017::GetClanChatMessage", 2u, 59u, 0, m462, NULL},
    {"SteamFriends017::GetClanCount", 2u, 18u, 0, m463, NULL},
    {"SteamFriends017::GetClanName", 2u, 20u, 0, m464, NULL},
    {"SteamFriends017::GetClanOfficerByIndex", 2u, 41u, 1, m465, NULL},
    {"SteamFriends017::GetClanOfficerCount", 2u, 40u, 0, m466, NULL},
    {"SteamFriends017::GetClanOwner", 2u, 39u, 1, m467, NULL},
    {"SteamFriends017::GetClanTag", 2u, 21u, 0, m468, NULL},
    {"SteamFriends017::GetCoplayFriend", 2u, 51u, 1, m469, NULL},
    {"SteamFriends017::GetCoplayFriendCount", 2u, 50u, 0, m470, NULL},
    {"SteamFriends017::GetFollowerCount", 2u, 67u, 0, m471, NULL},
    {"SteamFriends017::GetFriendByIndex", 2u, 4u, 1, m472, NULL},
    {"SteamFriends017::GetFriendCoplayGame", 2u, 53u, 0, m473, NULL},
    {"SteamFriends017::GetFriendCoplayTime", 2u, 52u, 0, m474, NULL},
    {"SteamFriends017::GetFriendCount", 2u, 3u, 0, m475, NULL},
    {"SteamFriends017::GetFriendCountFromSource", 2u, 24u, 0, m476, NULL},
    {"SteamFriends017::GetFriendFromSourceByIndex", 2u, 25u, 1, m477, NULL},
    {"SteamFriends017::GetFriendGamePlayed", 2u, 8u, 0, m478, NULL},
    {"SteamFriends017::GetFriendMessage", 2u, 66u, 0, m479, NULL},
    {"SteamFriends017::GetFriendPersonaName", 2u, 7u, 0, m480, NULL},
    {"SteamFriends017::GetFriendPersonaNameHistory", 2u, 9u, 0, m481, NULL},
    {"SteamFriends017::GetFriendPersonaState", 2u, 6u, 0, m482, NULL},
    {"SteamFriends017::GetFriendRelationship", 2u, 5u, 0, m483, NULL},
    {"SteamFriends017::GetFriendRichPresence", 2u, 45u, 0, m484, NULL},
    {"SteamFriends017::GetFriendRichPresenceKeyByIndex", 2u, 47u, 0, m485, NULL},
    {"SteamFriends017::GetFriendRichPresenceKeyCount", 2u, 46u, 0, m486, NULL},
    {"SteamFriends017::GetFriendSteamLevel", 2u, 10u, 0, m487, NULL},
    {"SteamFriends017::GetFriendsGroupCount", 2u, 12u, 0, m488, NULL},
    {"SteamFriends017::GetFriendsGroupIDByIndex", 2u, 13u, 0, m489, NULL},
    {"SteamFriends017::GetFriendsGroupMembersCount", 2u, 15u, 0, m490, NULL},
    {"SteamFriends017::GetFriendsGroupMembersList", 2u, 16u, 0, m491, NULL},
    {"SteamFriends017::GetFriendsGroupName", 2u, 14u, 0, m492, NULL},
    {"SteamFriends017::GetLargeFriendAvatar", 2u, 36u, 0, m493, NULL},
    {"SteamFriends017::GetMediumFriendAvatar", 2u, 35u, 0, m494, NULL},
    {"SteamFriends017::GetNumChatsWithUnreadPriorityMessages", 2u, 72u, 0, m495, NULL},
    {"SteamFriends017::GetPersonaName", 2u, 0u, 0, m496, NULL},
    {"SteamFriends017::GetPersonaState", 2u, 2u, 0, m497, NULL},
    {"SteamFriends017::GetPlayerNickname", 2u, 11u, 0, m498, NULL},
    {"SteamFriends017::GetSmallFriendAvatar", 2u, 34u, 0, m499, NULL},
    {"SteamFriends017::GetUserRestrictions", 2u, 42u, 0, m500, NULL},
    {"SteamFriends017::HasFriend", 2u, 17u, 0, m501, NULL},
    {"SteamFriends017::InviteUserToGame", 2u, 49u, 0, m502, NULL},
    {"SteamFriends017::IsClanChatAdmin", 2u, 60u, 0, m503, NULL},
    {"SteamFriends017::IsClanChatWindowOpenInSteam", 2u, 61u, 0, m504, NULL},
    {"SteamFriends017::IsClanOfficialGameGroup", 2u, 71u, 0, m505, NULL},
    {"SteamFriends017::IsClanPublic", 2u, 70u, 0, m506, NULL},
    {"SteamFriends017::IsFollowing", 2u, 68u, 0, m507, NULL},
    {"SteamFriends017::IsUserInSource", 2u, 26u, 0, m508, NULL},
    {"SteamFriends017::JoinClanChatRoom", 2u, 54u, 0, m509, NULL},
    {"SteamFriends017::LeaveClanChatRoom", 2u, 55u, 0, m510, NULL},
    {"SteamFriends017::OpenClanChatWindowInSteam", 2u, 62u, 0, m511, NULL},
    {"SteamFriends017::RegisterProtocolInOverlayBrowser", 2u, 74u, 0, m512, NULL},
    {"SteamFriends017::ReplyToFriendMessage", 2u, 65u, 0, m513, NULL},
    {"SteamFriends017::RequestClanOfficerList", 2u, 38u, 0, m514, NULL},
    {"SteamFriends017::RequestFriendRichPresence", 2u, 48u, 0, m515, NULL},
    {"SteamFriends017::RequestUserInformation", 2u, 37u, 0, m516, NULL},
    {"SteamFriends017::SendClanChatMessage", 2u, 58u, 0, m517, NULL},
    {"SteamFriends017::SetInGameVoiceSpeaking", 2u, 27u, 0, m518, NULL},
    {"SteamFriends017::SetListenForFriendsMessages", 2u, 64u, 0, m519, NULL},
    {"SteamFriends017::SetPersonaName", 2u, 1u, 0, m520, NULL},
    {"SteamFriends017::SetPlayedWith", 2u, 32u, 0, m521, NULL},
    {"SteamFriends017::SetRichPresence", 2u, 43u, 0, m522, NULL},
    {"SteamGameServer014::AssociateWithClan", 23u, 36u, 0, m523, NULL},
    {"SteamGameServer014::BLoggedOn", 23u, 8u, 0, m524, NULL},
    {"SteamGameServer014::BSecure", 23u, 9u, 0, m525, NULL},
    {"SteamGameServer014::BUpdateUserData", 23u, 41u, 0, m526, NULL},
    {"SteamGameServer014::BeginAuthSession", 23u, 26u, 0, m527, NULL},
    {"SteamGameServer014::CancelAuthTicket", 23u, 28u, 0, m528, NULL},
    {"SteamGameServer014::ClearAllKeyValues", 23u, 19u, 0, m529, NULL},
    {"SteamGameServer014::ComputeNewPlayerCompatibility", 23u, 37u, 0, m530, NULL},
    {"SteamGameServer014::CreateUnauthenticatedUserConnection", 23u, 39u, 1, m531, NULL},
    {"SteamGameServer014::EndAuthSession", 23u, 27u, 0, m532, NULL},
    {"SteamGameServer014::GetAuthSessionTicket", 23u, 25u, 0, m533, NULL},
    {"SteamGameServer014::GetGameplayStats", 23u, 31u, 0, m534, NULL},
    {"SteamGameServer014::GetNextOutgoingPacket", 23u, 35u, 0, m535, NULL},
    {"SteamGameServer014::GetServerReputation", 23u, 32u, 0, m536, NULL},
    {"SteamGameServer014::GetSteamID", 23u, 10u, 1, m537, NULL},
    {"SteamGameServer014::HandleIncomingPacket", 23u, 34u, 0, m538, NULL},
    {"SteamGameServer014::LogOff", 23u, 7u, 0, m539, NULL},
    {"SteamGameServer014::LogOn", 23u, 5u, 0, m540, NULL},
    {"SteamGameServer014::LogOnAnonymous", 23u, 6u, 0, m541, NULL},
    {"SteamGameServer014::RequestUserGroupStatus", 23u, 30u, 0, m542, NULL},
    {"SteamGameServer014::SendUserConnectAndAuthenticate_DEPRECATED", 23u, 38u, 0, m543, NULL},
    {"SteamGameServer014::SendUserDisconnect_DEPRECATED", 23u, 40u, 0, m544, NULL},
    {"SteamGameServer014::SetAdvertiseServerActive", 23u, 24u, 0, m545, NULL},
    {"SteamGameServer014::SetBotPlayerCount", 23u, 13u, 0, m546, NULL},
    {"SteamGameServer014::SetDedicatedServer", 23u, 4u, 0, m547, NULL},
    {"SteamGameServer014::SetGameData", 23u, 22u, 0, m548, NULL},
    {"SteamGameServer014::SetGameDescription", 23u, 2u, 0, m549, NULL},
    {"SteamGameServer014::SetGameTags", 23u, 21u, 0, m550, NULL},
    {"SteamGameServer014::SetKeyValue", 23u, 20u, 0, m551, NULL},
    {"SteamGameServer014::SetMapName", 23u, 15u, 0, m552, NULL},
    {"SteamGameServer014::SetMaxPlayerCount", 23u, 12u, 0, m553, NULL},
    {"SteamGameServer014::SetModDir", 23u, 3u, 0, m554, NULL},
    {"SteamGameServer014::SetPasswordProtected", 23u, 16u, 0, m555, NULL},
    {"SteamGameServer014::SetProduct", 23u, 1u, 0, m556, NULL},
    {"SteamGameServer014::SetRegion", 23u, 23u, 0, m557, NULL},
    {"SteamGameServer014::SetServerName", 23u, 14u, 0, m558, NULL},
    {"SteamGameServer014::SetSpectatorPort", 23u, 17u, 0, m559, NULL},
    {"SteamGameServer014::SetSpectatorServerName", 23u, 18u, 0, m560, NULL},
    {"SteamGameServer014::UserHasLicenseForApp", 23u, 29u, 0, m561, NULL},
    {"SteamGameServer014::WasRestartRequested", 23u, 11u, 0, m562, NULL},
    {"SteamGameServerStats001::ClearUserAchievement", 24u, 8u, 0, m563, NULL},
    {"SteamGameServerStats001::GetUserAchievement", 24u, 3u, 0, m564, NULL},
    {"SteamGameServerStats001::GetUserStatFloat", 24u, 2u, 0, m565, NULL},
    {"SteamGameServerStats001::GetUserStatInt32", 24u, 1u, 0, m566, NULL},
    {"SteamGameServerStats001::RequestUserStats", 24u, 0u, 0, m567, NULL},
    {"SteamGameServerStats001::SetUserAchievement", 24u, 7u, 0, m568, NULL},
    {"SteamGameServerStats001::SetUserStatFloat", 24u, 5u, 0, m569, NULL},
    {"SteamGameServerStats001::SetUserStatInt32", 24u, 4u, 0, m570, NULL},
    {"SteamGameServerStats001::StoreUserStats", 24u, 9u, 0, m571, NULL},
    {"SteamGameServerStats001::UpdateUserAvgRateStat", 24u, 6u, 0, m572, NULL},
    {"SteamInput006::ActivateActionSet", 22u, 10u, 0, m573, NULL},
    {"SteamInput006::ActivateActionSetLayer", 22u, 12u, 0, m574, NULL},
    {"SteamInput006::BNewDataAvailable", 22u, 5u, 0, m575, NULL},
    {"SteamInput006::BWaitForData", 22u, 4u, 0, m576, NULL},
    {"SteamInput006::DeactivateActionSetLayer", 22u, 13u, 0, m577, NULL},
    {"SteamInput006::DeactivateAllActionSetLayers", 22u, 14u, 0, m578, NULL},
    {"SteamInput006::EnableActionEventCallbacks", 22u, 8u, 0, NULL, "param\u00e8tre SteamInputActionEventCallbackPointer pCallback"},
    {"SteamInput006::EnableDeviceCallbacks", 22u, 7u, 0, m580, NULL},
    {"SteamInput006::GetActionOriginFromXboxOrigin", 22u, 42u, 0, m581, NULL},
    {"SteamInput006::GetActionSetHandle", 22u, 9u, 0, m582, NULL},
    {"SteamInput006::GetActiveActionSetLayers", 22u, 15u, 0, m583, NULL},
    {"SteamInput006::GetAnalogActionHandle", 22u, 20u, 0, m584, NULL},
    {"SteamInput006::GetAnalogActionOrigins", 22u, 22u, 0, m585, NULL},
    {"SteamInput006::GetConnectedControllers", 22u, 6u, 0, m586, NULL},
    {"SteamInput006::GetControllerForGamepadIndex", 22u, 38u, 0, m587, NULL},
    {"SteamInput006::GetCurrentActionSet", 22u, 11u, 0, m588, NULL},
    {"SteamInput006::GetDeviceBindingRevision", 22u, 44u, 0, m589, NULL},
    {"SteamInput006::GetDigitalActionHandle", 22u, 16u, 0, m590, NULL},
    {"SteamInput006::GetDigitalActionOrigins", 22u, 18u, 0, m591, NULL},
    {"SteamInput006::GetGamepadIndexForController", 22u, 39u, 0, m592, NULL},
    {"SteamInput006::GetGlyphForActionOrigin_Legacy", 22u, 25u, 0, m593, NULL},
    {"SteamInput006::GetGlyphForXboxOrigin", 22u, 41u, 0, m594, NULL},
    {"SteamInput006::GetGlyphPNGForActionOrigin", 22u, 23u, 0, m595, NULL},
    {"SteamInput006::GetGlyphSVGForActionOrigin", 22u, 24u, 0, m596, NULL},
    {"SteamInput006::GetInputTypeForHandle", 22u, 37u, 0, m597, NULL},
    {"SteamInput006::GetRemotePlaySessionID", 22u, 45u, 0, m598, NULL},
    {"SteamInput006::GetSessionInputConfigurationSettings", 22u, 46u, 0, m599, NULL},
    {"SteamInput006::GetStringForActionOrigin", 22u, 26u, 0, m600, NULL},
    {"SteamInput006::GetStringForAnalogActionName", 22u, 27u, 0, m601, NULL},
    {"SteamInput006::GetStringForDigitalActionName", 22u, 19u, 0, m602, NULL},
    {"SteamInput006::GetStringForXboxOrigin", 22u, 40u, 0, m603, NULL},
    {"SteamInput006::Init", 22u, 0u, 0, m604, NULL},
    {"SteamInput006::Legacy_TriggerHapticPulse", 22u, 34u, 0, m605, NULL},
    {"SteamInput006::Legacy_TriggerRepeatedHapticPulse", 22u, 35u, 0, m606, NULL},
    {"SteamInput006::RunFrame", 22u, 3u, 0, m607, NULL},
    {"SteamInput006::SetInputActionManifestFilePath", 22u, 2u, 0, m608, NULL},
    {"SteamInput006::SetLEDColor", 22u, 33u, 0, m609, NULL},
    {"SteamInput006::ShowBindingPanel", 22u, 36u, 0, m610, NULL},
    {"SteamInput006::Shutdown", 22u, 1u, 0, m611, NULL},
    {"SteamInput006::StopAnalogActionMomentum", 22u, 28u, 0, m612, NULL},
    {"SteamInput006::TranslateActionOrigin", 22u, 43u, 0, m613, NULL},
    {"SteamInput006::TriggerSimpleHapticEvent", 22u, 32u, 0, NULL, "param\u00e8tre char nGainDB"},
    {"SteamInput006::TriggerVibration", 22u, 30u, 0, m615, NULL},
    {"SteamInput006::TriggerVibrationExtended", 22u, 31u, 0, m616, NULL},
    {"SteamMatchGameSearch001::AcceptGame", 5u, 3u, 0, m617, NULL},
    {"SteamMatchGameSearch001::AddGameSearchParams", 5u, 0u, 0, m618, NULL},
    {"SteamMatchGameSearch001::CancelRequestPlayersForGame", 5u, 11u, 0, m619, NULL},
    {"SteamMatchGameSearch001::DeclineGame", 5u, 4u, 0, m620, NULL},
    {"SteamMatchGameSearch001::EndGame", 5u, 13u, 0, m621, NULL},
    {"SteamMatchGameSearch001::EndGameSearch", 5u, 6u, 0, m622, NULL},
    {"SteamMatchGameSearch001::HostConfirmGameStart", 5u, 10u, 0, m623, NULL},
    {"SteamMatchGameSearch001::RequestPlayersForGame", 5u, 9u, 0, m624, NULL},
    {"SteamMatchGameSearch001::RetrieveConnectionDetails", 5u, 5u, 0, m625, NULL},
    {"SteamMatchGameSearch001::SearchForGameSolo", 5u, 2u, 0, m626, NULL},
    {"SteamMatchGameSearch001::SearchForGameWithLobby", 5u, 1u, 0, m627, NULL},
    {"SteamMatchGameSearch001::SetConnectionDetails", 5u, 8u, 0, m628, NULL},
    {"SteamMatchGameSearch001::SetGameHostParams", 5u, 7u, 0, m629, NULL},
    {"SteamMatchGameSearch001::SubmitPlayerResult", 5u, 12u, 0, m630, NULL},
    {"SteamMatchMaking009::AddFavoriteGame", 4u, 2u, 0, m631, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListCompatibleMembersFilter", 4u, 11u, 0, m632, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListDistanceFilter", 4u, 9u, 0, m633, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListFilterSlotsAvailable", 4u, 8u, 0, m634, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListNearValueFilter", 4u, 7u, 0, m635, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListNumericalFilter", 4u, 6u, 0, m636, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListResultCountFilter", 4u, 10u, 0, m637, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListStringFilter", 4u, 5u, 0, m638, NULL},
    {"SteamMatchMaking009::CreateLobby", 4u, 13u, 0, m639, NULL},
    {"SteamMatchMaking009::DeleteLobbyData", 4u, 23u, 0, m640, NULL},
    {"SteamMatchMaking009::GetFavoriteGame", 4u, 1u, 0, m641, NULL},
    {"SteamMatchMaking009::GetFavoriteGameCount", 4u, 0u, 0, m642, NULL},
    {"SteamMatchMaking009::GetLobbyByIndex", 4u, 12u, 1, m643, NULL},
    {"SteamMatchMaking009::GetLobbyChatEntry", 4u, 27u, 0, m644, NULL},
    {"SteamMatchMaking009::GetLobbyData", 4u, 19u, 0, m645, NULL},
    {"SteamMatchMaking009::GetLobbyDataByIndex", 4u, 22u, 0, m646, NULL},
    {"SteamMatchMaking009::GetLobbyDataCount", 4u, 21u, 0, m647, NULL},
    {"SteamMatchMaking009::GetLobbyGameServer", 4u, 30u, 0, m648, NULL},
    {"SteamMatchMaking009::GetLobbyMemberByIndex", 4u, 18u, 1, m649, NULL},
    {"SteamMatchMaking009::GetLobbyMemberData", 4u, 24u, 0, m650, NULL},
    {"SteamMatchMaking009::GetLobbyMemberLimit", 4u, 32u, 0, m651, NULL},
    {"SteamMatchMaking009::GetLobbyOwner", 4u, 35u, 1, m652, NULL},
    {"SteamMatchMaking009::GetNumLobbyMembers", 4u, 17u, 0, m653, NULL},
    {"SteamMatchMaking009::InviteUserToLobby", 4u, 16u, 0, m654, NULL},
    {"SteamMatchMaking009::JoinLobby", 4u, 14u, 0, m655, NULL},
    {"SteamMatchMaking009::LeaveLobby", 4u, 15u, 0, m656, NULL},
    {"SteamMatchMaking009::RemoveFavoriteGame", 4u, 3u, 0, m657, NULL},
    {"SteamMatchMaking009::RequestLobbyData", 4u, 28u, 0, m658, NULL},
    {"SteamMatchMaking009::RequestLobbyList", 4u, 4u, 0, m659, NULL},
    {"SteamMatchMaking009::SendLobbyChatMsg", 4u, 26u, 0, m660, NULL},
    {"SteamMatchMaking009::SetLinkedLobby", 4u, 37u, 0, m661, NULL},
    {"SteamMatchMaking009::SetLobbyData", 4u, 20u, 0, m662, NULL},
    {"SteamMatchMaking009::SetLobbyGameServer", 4u, 29u, 0, m663, NULL},
    {"SteamMatchMaking009::SetLobbyJoinable", 4u, 34u, 0, m664, NULL},
    {"SteamMatchMaking009::SetLobbyMemberData", 4u, 25u, 0, m665, NULL},
    {"SteamMatchMaking009::SetLobbyMemberLimit", 4u, 31u, 0, m666, NULL},
    {"SteamMatchMaking009::SetLobbyOwner", 4u, 36u, 0, m667, NULL},
    {"SteamMatchMaking009::SetLobbyType", 4u, 33u, 0, m668, NULL},
    {"SteamMatchMakingServers002::CancelQuery", 6u, 8u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::CancelServerQuery", 6u, 16u, 0, m670, NULL},
    {"SteamMatchMakingServers002::GetServerCount", 6u, 11u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::GetServerDetails", 6u, 7u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::IsRefreshing", 6u, 10u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::PingServer", 6u, 13u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamMatchMakingServers002::PlayerDetails", 6u, 14u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamMatchMakingServers002::RefreshQuery", 6u, 9u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::RefreshServer", 6u, 12u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::ReleaseRequest", 6u, 6u, 0, NULL, "param\u00e8tre HServerListRequest hServerListRequest"},
    {"SteamMatchMakingServers002::RequestFavoritesServerList", 6u, 3u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestFriendsServerList", 6u, 2u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestHistoryServerList", 6u, 4u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestInternetServerList", 6u, 0u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::RequestLANServerList", 6u, 1u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamMatchMakingServers002::RequestSpectatorServerList", 6u, 5u, 0, NULL, "tampon IntPtr ppchFilters sans taille"},
    {"SteamMatchMakingServers002::ServerRules", 6u, 15u, 0, NULL, "tampon IntPtr pRequestServersResponse sans taille"},
    {"SteamNetworking006::AcceptP2PSessionWithUser", 9u, 3u, 0, m686, NULL},
    {"SteamNetworking006::AllowP2PPacketRelay", 9u, 7u, 0, m687, NULL},
    {"SteamNetworking006::CloseP2PChannelWithUser", 9u, 5u, 0, m688, NULL},
    {"SteamNetworking006::CloseP2PSessionWithUser", 9u, 4u, 0, m689, NULL},
    {"SteamNetworking006::CreateP2PConnectionSocket", 9u, 9u, 0, m690, NULL},
    {"SteamNetworking006::DestroyListenSocket", 9u, 12u, 0, m691, NULL},
    {"SteamNetworking006::DestroySocket", 9u, 11u, 0, m692, NULL},
    {"SteamNetworking006::GetListenSocketInfo", 9u, 19u, 0, NULL, "param\u00e8tre out SteamIPAddress_t pnIP"},
    {"SteamNetworking006::GetMaxPacketSize", 9u, 21u, 0, m694, NULL},
    {"SteamNetworking006::GetP2PSessionState", 9u, 6u, 0, NULL, "param\u00e8tre out P2PSessionState_t pConnectionState"},
    {"SteamNetworking006::GetSocketConnectionType", 9u, 20u, 0, m696, NULL},
    {"SteamNetworking006::GetSocketInfo", 9u, 18u, 0, NULL, "param\u00e8tre out SteamIPAddress_t punIPRemote"},
    {"SteamNetworking006::IsDataAvailable", 9u, 16u, 0, m698, NULL},
    {"SteamNetworking006::IsDataAvailableOnSocket", 9u, 14u, 0, m699, NULL},
    {"SteamNetworking006::IsP2PPacketAvailable", 9u, 1u, 0, m700, NULL},
    {"SteamNetworking006::ReadP2PPacket", 9u, 2u, 0, m701, NULL},
    {"SteamNetworking006::RetrieveData", 9u, 17u, 0, m702, NULL},
    {"SteamNetworking006::RetrieveDataFromSocket", 9u, 15u, 0, m703, NULL},
    {"SteamNetworking006::SendDataOnSocket", 9u, 13u, 0, m704, NULL},
    {"SteamNetworking006::SendP2PPacket", 9u, 0u, 0, m705, NULL},
    {"SteamUser021::AdvertiseGame", 1u, 19u, 0, m706, NULL},
    {"SteamUser021::BIsBehindNAT", 1u, 18u, 0, m707, NULL},
    {"SteamUser021::BIsPhoneIdentifying", 1u, 27u, 0, m708, NULL},
    {"SteamUser021::BIsPhoneRequiringVerification", 1u, 28u, 0, m709, NULL},
    {"SteamUser021::BIsPhoneVerified", 1u, 25u, 0, m710, NULL},
    {"SteamUser021::BIsTwoFactorEnabled", 1u, 26u, 0, m711, NULL},
    {"SteamUser021::BLoggedOn", 1u, 1u, 0, m712, NULL},
    {"SteamUser021::BSetDurationControlOnlineState", 1u, 31u, 0, m713, NULL},
    {"SteamUser021::BeginAuthSession", 1u, 14u, 0, m714, NULL},
    {"SteamUser021::CancelAuthTicket", 1u, 16u, 0, m715, NULL},
    {"SteamUser021::DecompressVoice", 1u, 11u, 0, m716, NULL},
    {"SteamUser021::EndAuthSession", 1u, 15u, 0, m717, NULL},
    {"SteamUser021::GetAuthSessionTicket", 1u, 13u, 0, m718, NULL},
    {"SteamUser021::GetAvailableVoice", 1u, 9u, 0, NULL, "tampon IntPtr pcbUncompressed_Deprecated sans taille"},
    {"SteamUser021::GetDurationControl", 1u, 30u, 0, m720, NULL},
    {"SteamUser021::GetEncryptedAppTicket", 1u, 21u, 0, m721, NULL},
    {"SteamUser021::GetGameBadgeLevel", 1u, 22u, 0, m722, NULL},
    {"SteamUser021::GetHSteamUser", 1u, 0u, 0, m723, NULL},
    {"SteamUser021::GetMarketEligibility", 1u, 29u, 0, m724, NULL},
    {"SteamUser021::GetPlayerSteamLevel", 1u, 23u, 0, m725, NULL},
    {"SteamUser021::GetSteamID", 1u, 2u, 1, m726, NULL},
    {"SteamUser021::GetUserDataFolder", 1u, 6u, 0, m727, NULL},
    {"SteamUser021::GetVoiceOptimalSampleRate", 1u, 12u, 0, m728, NULL},
    {"SteamUser021::InitiateGameConnection_DEPRECATED", 1u, 3u, 0, m729, NULL},
    {"SteamUser021::RequestEncryptedAppTicket", 1u, 20u, 0, m730, NULL},
    {"SteamUser021::RequestStoreAuthURL", 1u, 24u, 0, m731, NULL},
    {"SteamUser021::StartVoiceRecording", 1u, 7u, 0, m732, NULL},
    {"SteamUser021::StopVoiceRecording", 1u, 8u, 0, m733, NULL},
    {"SteamUser021::TerminateGameConnection_DEPRECATED", 1u, 4u, 0, m734, NULL},
    {"SteamUser021::UserHasLicenseForApp", 1u, 17u, 0, m735, NULL},
    {"SteamUtils010::BOverlayNeedsPresent", 3u, 18u, 0, m736, NULL},
    {"SteamUtils010::CheckFileSignature", 3u, 19u, 0, m737, NULL},
    {"SteamUtils010::DismissFloatingGamepadTextInput", 3u, 37u, 0, m738, NULL},
    {"SteamUtils010::FilterText", 3u, 32u, 0, m739, NULL},
    {"SteamUtils010::GetAPICallFailureReason", 3u, 12u, 0, m740, NULL},
    {"SteamUtils010::GetAPICallResult", 3u, 13u, 0, m741, NULL},
    {"SteamUtils010::GetAppID", 3u, 9u, 0, m742, NULL},
    {"SteamUtils010::GetConnectedUniverse", 3u, 2u, 0, m743, NULL},
    {"SteamUtils010::GetCurrentBatteryPower", 3u, 8u, 0, m744, NULL},
    {"SteamUtils010::GetEnteredGamepadTextInput", 3u, 22u, 0, m745, NULL},
    {"SteamUtils010::GetEnteredGamepadTextLength", 3u, 21u, 0, m746, NULL},
    {"SteamUtils010::GetIPCCallCount", 3u, 15u, 0, m747, NULL},
    {"SteamUtils010::GetIPCountry", 3u, 4u, 0, m748, NULL},
    {"SteamUtils010::GetIPv6ConnectivityState", 3u, 33u, 0, m749, NULL},
    {"SteamUtils010::GetImageRGBA", 3u, 6u, 0, m750, NULL},
    {"SteamUtils010::GetImageSize", 3u, 5u, 0, m751, NULL},
    {"SteamUtils010::GetSecondsSinceAppActive", 3u, 0u, 0, m752, NULL},
    {"SteamUtils010::GetSecondsSinceComputerActive", 3u, 1u, 0, m753, NULL},
    {"SteamUtils010::GetServerRealTime", 3u, 3u, 0, m754, NULL},
    {"SteamUtils010::GetSteamUILanguage", 3u, 23u, 0, m755, NULL},
    {"SteamUtils010::InitFilterText", 3u, 31u, 0, m756, NULL},
    {"SteamUtils010::IsAPICallCompleted", 3u, 11u, 0, m757, NULL},
    {"SteamUtils010::IsOverlayEnabled", 3u, 17u, 0, m758, NULL},
    {"SteamUtils010::IsSteamChinaLauncher", 3u, 30u, 0, m759, NULL},
    {"SteamUtils010::IsSteamInBigPictureMode", 3u, 26u, 0, m760, NULL},
    {"SteamUtils010::IsSteamRunningInVR", 3u, 24u, 0, m761, NULL},
    {"SteamUtils010::IsSteamRunningOnSteamDeck", 3u, 34u, 0, m762, NULL},
    {"SteamUtils010::IsVRHeadsetStreamingEnabled", 3u, 28u, 0, m763, NULL},
    {"SteamUtils010::SetGameLauncherMode", 3u, 36u, 0, m764, NULL},
    {"SteamUtils010::SetOverlayNotificationInset", 3u, 25u, 0, m765, NULL},
    {"SteamUtils010::SetOverlayNotificationPosition", 3u, 10u, 0, m766, NULL},
    {"SteamUtils010::SetVRHeadsetStreamingEnabled", 3u, 29u, 0, m767, NULL},
    {"SteamUtils010::SetWarningMessageHook", 3u, 16u, 0, steam_manual_utils_warning_hook, NULL},
    {"SteamUtils010::ShowFloatingGamepadTextInput", 3u, 35u, 0, m769, NULL},
    {"SteamUtils010::ShowGamepadTextInput", 3u, 20u, 0, m770, NULL},
    {"SteamUtils010::StartVRDashboard", 3u, 27u, 0, m771, NULL},
};
const size_t steam_method_count = 772;
