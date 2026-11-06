/* Généré par tools/gen_steam.py depuis les libsteam_api.dylib officielles (SDK 1.53a, 1.60,
 * 1.61) et Steamworks.NET 20.1.0, 2024.8.0, 2025.161.0 (MIT). Ne pas modifier : relancer
 * le générateur. */
#include "steam.h"

void steam_manual_client_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret);
void steam_manual_utils_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret);
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppBuildId */
static void m0(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetAppInstallDir */
static void m1(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
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
    void *fn = steam_slot(c, self, 2u);
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
    void *fn = steam_slot(c, self, 1u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 4u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMAPPLIST_INTERFACE_VERSION001::GetNumInstalledApps */
static void m4(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::BGetDLCDataByIndex */
static void m5(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *, void *, int32_t);
    void *fn = steam_slot(c, self, 11u);
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
    void *fn = steam_slot(c, self, 19u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsCybercafe */
static void m7(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsDlcInstalled */
static void m8(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsLowViolence */
static void m9(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribed */
static void m10(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedApp */
static void m11(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFamilySharing */
static void m12(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsSubscribedFromFreeWeekend */
static void m13(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsTimedTrial */
static void m14(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    void *fn = steam_slot(c, self, 28u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::BIsVACBanned */
static void m15(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 3u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppBuildId */
static void m16(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAppInstallDir */
static void m17(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 18u);
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
    void *fn = steam_slot(c, self, 20u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetAvailableGameLanguages */
static void m19(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 5u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetCurrentBetaName */
static void m20(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 15u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetCurrentGameLanguage */
static void m21(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 4u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetDLCCount */
static void m22(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetDlcDownloadProgress */
static void m23(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 22u);
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
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetFileDetails */
static void m25(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 25u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMAPPS_INTERFACE_VERSION008::GetInstalledDepots */
static void m26(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 17u);
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
    void *fn = steam_slot(c, self, 26u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMAPPS_INTERFACE_VERSION008::GetLaunchQueryParam */
static void m28(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMAPPS_INTERFACE_VERSION008::InstallDLC */
static void m29(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 12u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMAPPS_INTERFACE_VERSION008::MarkContentCorrupt */
static void m30(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 16u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMAPPS_INTERFACE_VERSION008::RequestAllProofOfPurchaseKeys */
static void m31(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    ((F)fn)(self);
}
/* STEAMAPPS_INTERFACE_VERSION008::RequestAppProofOfPurchaseKey */
static void m32(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMAPPS_INTERFACE_VERSION008::UninstallDLC */
static void m33(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::AddHeader */
static void m34(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, const char *);
    void *fn = steam_slot(c, self, 12u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::AllowStartRequest */
static void m35(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 36u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::CopyToClipboard */
static void m36(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 26u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::CreateBrowser */
static void m37(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::ExecuteJavascript */
static void m38(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Find */
static void m40(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, bool, bool);
    void *fn = steam_slot(c, self, 28u);
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
    void *fn = steam_slot(c, self, 30u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoBack */
static void m42(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 10u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::GoForward */
static void m43(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Init */
static void m44(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::JSDialogResponse */
static void m45(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 37u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyChar */
static void m46(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 21u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::KeyDown */
static void m47(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t, int32_t, bool);
    void *fn = steam_slot(c, self, 19u);
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
    void *fn = steam_slot(c, self, 20u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::LoadURL */
static void m49(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *, const char *);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDoubleClick */
static void m50(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 16u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseDown */
static void m51(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseMove */
static void m52(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseUp */
static void m53(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::MouseWheel */
static void m54(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 18u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::OpenDeveloperTools */
static void m55(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 35u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::PasteFromClipboard */
static void m56(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 27u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Reload */
static void m57(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::RemoveBrowser */
static void m58(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetBackgroundMode */
static void m59(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 33u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetCookie */
static void m60(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, const char *, const char *, uint32_t, bool, bool);
    void *fn = steam_slot(c, self, 31u);
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
    void *fn = steam_slot(c, self, 34u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    float a1 = RF(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetHorizontalScroll */
static void m62(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 22u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetKeyFocus */
static void m63(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 24u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetPageScaleFactor */
static void m64(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, float, int32_t, int32_t);
    void *fn = steam_slot(c, self, 32u);
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
    void *fn = steam_slot(c, self, 7u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::SetVerticalScroll */
static void m66(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 23u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::Shutdown */
static void m67(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 3u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopFind */
static void m68(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 29u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::StopLoad */
static void m69(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTMLSURFACE_INTERFACE_VERSION_005::ViewSource */
static void m70(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 25u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMHTTP_INTERFACE_VERSION003::CreateCookieContainer */
static void m71(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, bool);
    void *fn = steam_slot(c, self, 17u);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION003::CreateHTTPRequest */
static void m72(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *);
    void *fn = steam_slot(c, self, 0u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMHTTP_INTERFACE_VERSION003::DeferHTTPRequest */
static void m73(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPDownloadProgressPct */
static void m74(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPRequestWasTimedOut */
static void m75(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 24u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseBodyData */
static void m76(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 12u);
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
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::GetHTTPResponseHeaderSize */
static void m78(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, void *);
    void *fn = steam_slot(c, self, 9u);
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
    void *fn = steam_slot(c, self, 10u);
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
    void *fn = steam_slot(c, self, 13u);
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
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::ReleaseCookieContainer */
static void m82(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::ReleaseHTTPRequest */
static void m83(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequest */
static void m84(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SendHTTPRequestAndStreamResponse */
static void m85(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetCookie */
static void m86(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *, const char *);
    void *fn = steam_slot(c, self, 19u);
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
    void *fn = steam_slot(c, self, 23u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestContextValue */
static void m88(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 1u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestCookieContainer */
static void m89(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 20u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestGetOrPostParameter */
static void m90(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *, const char *);
    void *fn = steam_slot(c, self, 4u);
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
    void *fn = steam_slot(c, self, 3u);
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
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestRequiresVerifiedCertificate */
static void m94(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 22u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMHTTP_INTERFACE_VERSION003::SetHTTPRequestUserAgentInfo */
static void m95(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::AddPromoItem */
static void m96(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::CheckResultSteamID */
static void m98(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::ConsumeItem */
static void m99(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 14u);
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
    void *fn = steam_slot(c, self, 9u);
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
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMINVENTORY_INTERFACE_V003::GetAllItems */
static void m104(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 6u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::GetItemPrice */
static void m108(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 29u);
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
    void *fn = steam_slot(c, self, 7u);
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
    void *fn = steam_slot(c, self, 27u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GetResultStatus */
static void m114(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 0u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GetResultTimestamp */
static void m115(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMINVENTORY_INTERFACE_V003::GrantPromoItems */
static void m116(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 11u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::InspectItem */
static void m117(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, const char *);
    void *fn = steam_slot(c, self, 37u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::LoadItemDefinitions */
static void m118(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 20u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::RemoveProperty */
static void m119(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *);
    void *fn = steam_slot(c, self, 31u);
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
    void *fn = steam_slot(c, self, 23u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::RequestPrices */
static void m121(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::SendItemDropHeartbeat */
static void m122(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 17u);
    ((F)fn)(self);
}
/* STEAMINVENTORY_INTERFACE_V003::SetPropertyBool */
static void m124(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t, const char *, bool);
    void *fn = steam_slot(c, self, 33u);
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
    void *fn = steam_slot(c, self, 35u);
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
    void *fn = steam_slot(c, self, 34u);
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
    void *fn = steam_slot(c, self, 32u);
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
    void *fn = steam_slot(c, self, 30u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMINVENTORY_INTERFACE_V003::SubmitUpdateProperties */
static void m130(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMINVENTORY_INTERFACE_V003::TransferItemQuantity */
static void m131(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint64_t, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 16u);
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
    void *fn = steam_slot(c, self, 18u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::BActivationSuccess */
static void m133(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 3u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::BIsCurrentMusicRemote */
static void m134(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryDidChange */
static void m135(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryIsAvailable */
static void m136(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 17u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::CurrentEntryWillChange */
static void m137(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 16u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::DeregisterSteamMusicRemote */
static void m138(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableLooped */
static void m139(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 9u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayNext */
static void m140(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 7u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlayPrevious */
static void m141(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 6u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnablePlaylists */
static void m142(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 11u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableQueue */
static void m143(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 10u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::EnableShuffled */
static void m144(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 8u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistDidChange */
static void m145(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::PlaylistWillChange */
static void m146(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueDidChange */
static void m147(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::QueueWillChange */
static void m148(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 22u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::RegisterSteamMusicRemote */
static void m149(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 0u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetPlaylistEntries */
static void m150(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::ResetQueueEntries */
static void m151(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentPlaylistEntry */
static void m152(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 30u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetCurrentQueueEntry */
static void m153(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetDisplayName */
static void m154(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPNGIcon_64x64 */
static void m155(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 5u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::SetPlaylistEntry */
static void m156(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 29u);
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
    void *fn = steam_slot(c, self, 24u);
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
    void *fn = steam_slot(c, self, 20u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryElapsedSeconds */
static void m159(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateCurrentEntryText */
static void m160(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 18u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateLooped */
static void m161(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 14u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdatePlaybackStatus */
static void m162(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateShuffled */
static void m163(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 13u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSICREMOTE_INTERFACE_VERSION001::UpdateVolume */
static void m164(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, float);
    void *fn = steam_slot(c, self, 15u);
    float a0 = RF(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::BIsEnabled */
static void m165(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::BIsPlaying */
static void m166(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMMUSIC_INTERFACE_VERSION001::GetPlaybackStatus */
static void m167(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMMUSIC_INTERFACE_VERSION001::GetVolume */
static void m168(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef float (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    float r = ((F)fn)(self);
    rt_return_st0(c, r);
}
/* STEAMMUSIC_INTERFACE_VERSION001::Pause */
static void m169(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 4u);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::Play */
static void m170(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 3u);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::PlayNext */
static void m171(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 6u);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::PlayPrevious */
static void m172(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 5u);
    ((F)fn)(self);
}
/* STEAMMUSIC_INTERFACE_VERSION001::SetVolume */
static void m173(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, float);
    void *fn = steam_slot(c, self, 7u);
    float a0 = RF(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppBlocked */
static void m174(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsAppInBlockList */
static void m175(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureBlocked */
static void m176(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsFeatureInBlockList */
static void m177(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockEnabled */
static void m178(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMPARENTALSETTINGS_INTERFACE_VERSION001::BIsParentalLockLocked */
static void m179(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::BGetSessionClientResolution */
static void m180(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::BSendRemotePlayTogetherInvite */
static void m181(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::BStartRemotePlayTogether */
static void m182(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 6u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientFormFactor */
static void m183(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientName */
static void m184(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionCount */
static void m185(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionID */
static void m186(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionSteamID */
static void m187(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::BeginFileWriteBatch */
static void m188(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 57u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::CommitPublishedFileUpdate */
static void m189(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::CreatePublishedFileUpdateRequest */
static void m190(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::DeletePublishedFile */
static void m191(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EndFileWriteBatch */
static void m192(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 58u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedFilesByUserAction */
static void m193(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, uint32_t);
    void *fn = steam_slot(c, self, 52u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserPublishedFiles */
static void m195(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSubscribedFiles */
static void m197(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 43u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileDelete */
static void m198(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 6u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileExists */
static void m199(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 13u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileForget */
static void m200(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FilePersisted */
static void m201(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileRead */
static void m202(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsync */
static void m203(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsyncComplete */
static void m204(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileShare */
static void m205(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 7u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWrite */
static void m206(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, int32_t);
    void *fn = steam_slot(c, self, 0u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteAsync */
static void m207(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamCancel */
static void m208(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamClose */
static void m209(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamOpen */
static void m210(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 9u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamWriteChunk */
static void m211(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCCount */
static void m212(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCHandle */
static void m213(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileCount */
static void m214(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileNameAndSize */
static void m215(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileSize */
static void m216(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileTimestamp */
static void m217(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 16u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChange */
static void m218(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 56u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChangeCount */
static void m219(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 55u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedFileDetails */
static void m220(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedItemVoteDetails */
static void m221(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetQuota */
static void m222(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, void *);
    void *fn = steam_slot(c, self, 20u);
    void *a0 = steam_out(c, R32(args + 0u), 8u);
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetSyncPlatforms */
static void m223(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 17u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDetails */
static void m224(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, const char **, void *, void *);
    void *fn = steam_slot(c, self, 26u);
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
static void m225(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUserPublishedItemVoteDetails */
static void m226(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForAccount */
static void m227(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForApp */
static void m228(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 22u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetCloudEnabledForApp */
static void m229(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 23u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetSyncPlatforms */
static void m230(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 8u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetUserPublishedFileAction */
static void m231(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 51u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::SubscribePublishedFile */
static void m232(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownload */
static void m233(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownloadToLocation */
static void m234(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *, uint32_t);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCRead */
static void m235(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UnsubscribePublishedFile */
static void m236(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 44u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileDescription */
static void m237(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileFile */
static void m238(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFilePreviewFile */
static void m239(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileSetChangeDescription */
static void m240(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTitle */
static void m242(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileVisibility */
static void m243(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdateUserPublishedItemVote */
static void m244(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::AddScreenshotToLibrary */
static void m245(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, const char *, const char *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::AddVRScreenshotToLibrary */
static void m246(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, int32_t, const char *, const char *);
    void *fn = steam_slot(c, self, 8u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::HookScreenshots */
static void m247(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 3u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::IsScreenshotsHooked */
static void m248(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::SetLocation */
static void m249(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TagPublishedFile */
static void m250(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TagUser */
static void m251(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::TriggerScreenshot */
static void m252(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    ((F)fn)(self);
}
/* STEAMSCREENSHOTS_INTERFACE_VERSION003::WriteScreenshot */
static void m253(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 0u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMTIMELINE_INTERFACE_V004::AddGamePhaseTag */
static void m254(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, const char *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* STEAMTIMELINE_INTERFACE_V004::AddInstantaneousTimelineEvent */
static void m255(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *, const char *, uint32_t, float, int32_t);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    float a4 = RF(args + 16u);
    int32_t a5 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::AddRangeTimelineEvent */
static void m256(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *, const char *, uint32_t, float, float, int32_t);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    float a4 = RF(args + 16u);
    float a5 = RF(args + 20u);
    int32_t a6 = (int32_t)R32(args + 24u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5, a6);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::ClearTimelineTooltip */
static void m257(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, float);
    void *fn = steam_slot(c, self, 1u);
    float a0 = RF(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::DoesEventRecordingExist */
static void m258(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::DoesGamePhaseRecordingExist */
static void m259(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 13u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::EndGamePhase */
static void m260(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    ((F)fn)(self);
}
/* STEAMTIMELINE_INTERFACE_V004::EndRangeTimelineEvent */
static void m261(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, float);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    float a1 = RF(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* STEAMTIMELINE_INTERFACE_V004::OpenOverlayToGamePhase */
static void m262(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 16u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::OpenOverlayToTimelineEvent */
static void m263(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::RemoveTimelineEvent */
static void m264(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::SetGamePhaseAttribute */
static void m265(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    uint32_t a2 = (uint32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* STEAMTIMELINE_INTERFACE_V004::SetGamePhaseID */
static void m266(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 12u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::SetTimelineGameMode */
static void m267(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMTIMELINE_INTERFACE_V004::SetTimelineTooltip */
static void m268(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, float);
    void *fn = steam_slot(c, self, 0u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* STEAMTIMELINE_INTERFACE_V004::StartGamePhase */
static void m269(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    ((F)fn)(self);
}
/* STEAMTIMELINE_INTERFACE_V004::StartRangeTimelineEvent */
static void m270(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, const char *, const char *, uint32_t, float, int32_t);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    float a4 = RF(args + 16u);
    int32_t a5 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMTIMELINE_INTERFACE_V004::UpdateRangeTimelineEvent */
static void m271(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, const char *, const char *, const char *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    const char *a3 = steam_in_string(c, R32(args + 16u));
    uint32_t a4 = (uint32_t)R32(args + 20u);
    int32_t a5 = (int32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* STEAMUGC_INTERFACE_VERSION016::AddAppDependency */
static void m272(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 80u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddDependency */
static void m273(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 78u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddExcludedTag */
static void m274(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemKeyValueTag */
static void m275(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 53u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemPreviewFile */
static void m276(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemPreviewVideo */
static void m277(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 55u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddItemToFavorites */
static void m278(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 63u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::AddRequiredKeyValueTag */
static void m279(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::AddRequiredTag */
static void m280(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::BInitWorkshopForGameServer */
static void m282(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 73u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::CreateItem */
static void m283(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 40u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestCursor */
static void m284(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestPage */
static void m285(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::CreateQueryUserUGCRequest */
static void m287(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 0u);
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
static void m288(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 83u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::DownloadItem */
static void m289(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 72u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetAppDependencies */
static void m290(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 82u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemDownloadInfo */
static void m291(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 71u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemInstallInfo */
static void m292(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 70u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemState */
static void m293(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 69u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetItemUpdateProgress */
static void m294(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 60u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetNumSubscribedItems */
static void m295(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 67u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryFirstUGCKeyValueTag */
static void m296(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCChildren */
static void m297(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCKeyValueTag */
static void m298(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 16u);
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
static void m299(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumAdditionalPreviews */
static void m300(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumKeyValueTags */
static void m301(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumTags */
static void m302(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCPreviewURL */
static void m303(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCStatistic */
static void m305(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTag */
static void m306(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTagDisplayName */
static void m307(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::GetSubscribedItems */
static void m308(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 68u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION016::GetUserItemVote */
static void m309(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 62u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::GetWorkshopEULAStatus */
static void m310(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 85u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::ReleaseQueryUGCRequest */
static void m311(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveAllItemKeyValueTags */
static void m312(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 51u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveAppDependency */
static void m313(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 81u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveDependency */
static void m314(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 79u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemFromFavorites */
static void m315(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 64u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemKeyValueTags */
static void m316(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 52u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RemoveItemPreview */
static void m317(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 58u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::RequestUGCDetails */
static void m318(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SendQueryUGCRequest */
static void m319(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SetAllowCachedResponse */
static void m320(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetAllowLegacyUpload */
static void m321(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 50u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetCloudFileNameFilter */
static void m322(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemContent */
static void m323(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemDescription */
static void m324(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 43u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemMetadata */
static void m325(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemPreview */
static void m326(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 49u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemTitle */
static void m328(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemUpdateLanguage */
static void m329(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 44u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetItemVisibility */
static void m330(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetLanguage */
static void m331(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetMatchAnyTag */
static void m332(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetRankedByTrendDays */
static void m333(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnAdditionalPreviews */
static void m334(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnChildren */
static void m335(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnKeyValueTags */
static void m336(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 23u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnLongDescription */
static void m337(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnMetadata */
static void m338(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnOnlyIDs */
static void m339(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnPlaytimeStats */
static void m340(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetReturnTotalOnly */
static void m341(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetSearchText */
static void m342(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetTimeCreatedDateRange */
static void m343(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetTimeUpdatedDateRange */
static void m344(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::SetUserItemVote */
static void m345(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 61u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::ShowWorkshopEULA */
static void m346(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 84u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::StartItemUpdate */
static void m347(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 41u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTrackingForAllItems */
static void m350(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 77u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SubmitItemUpdate */
static void m351(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 59u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SubscribeItem */
static void m352(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 65u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::SuspendDownloads */
static void m353(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 74u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMUGC_INTERFACE_VERSION016::UnsubscribeItem */
static void m354(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 66u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewFile */
static void m355(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 56u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewVideo */
static void m356(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 57u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddAppDependency */
static void m357(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 87u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::AddContentDescriptor */
static void m358(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 63u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddDependency */
static void m359(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 85u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::AddExcludedTag */
static void m360(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemKeyValueTag */
static void m361(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 57u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemPreviewFile */
static void m362(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    void *fn = steam_slot(c, self, 58u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemPreviewVideo */
static void m363(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 59u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddItemToFavorites */
static void m364(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 70u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::AddRequiredKeyValueTag */
static void m365(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::AddRequiredTag */
static void m366(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::BInitWorkshopForGameServer */
static void m368(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 80u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::CreateItem */
static void m369(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 44u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestCursor */
static void m370(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestPage */
static void m371(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::CreateQueryUserUGCRequest */
static void m373(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 0u);
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
/* STEAMUGC_INTERFACE_VERSION020::DeleteItem */
static void m374(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 90u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::DownloadItem */
static void m375(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 79u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetAppDependencies */
static void m376(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 89u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemDownloadInfo */
static void m377(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 78u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemInstallInfo */
static void m378(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 77u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemState */
static void m379(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 76u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetItemUpdateProgress */
static void m380(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 67u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 8u);
    void *a2 = steam_out(c, R32(args + 12u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetNumSubscribedItems */
static void m381(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 74u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetNumSupportedGameVersions */
static void m382(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryFirstUGCKeyValueTag */
static void m383(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCChildren */
static void m384(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCContentDescriptors */
static void m385(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 4u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCKeyValueTag */
static void m386(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 16u);
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
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCMetadata */
static void m387(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumAdditionalPreviews */
static void m388(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumKeyValueTags */
static void m389(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumTags */
static void m390(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCPreviewURL */
static void m391(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCStatistic */
static void m393(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTag */
static void m394(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTagDisplayName */
static void m395(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::GetSubscribedItems */
static void m396(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 75u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetUserContentDescriptorPreferences */
static void m398(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 93u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 4u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* STEAMUGC_INTERFACE_VERSION020::GetUserItemVote */
static void m399(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 69u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::GetWorkshopEULAStatus */
static void m400(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 92u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::ReleaseQueryUGCRequest */
static void m401(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveAllItemKeyValueTags */
static void m402(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 55u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveAppDependency */
static void m403(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 88u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveContentDescriptor */
static void m404(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 64u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveDependency */
static void m405(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 86u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveItemFromFavorites */
static void m406(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 71u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveItemKeyValueTags */
static void m407(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 56u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RemoveItemPreview */
static void m408(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 62u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::RequestUGCDetails */
static void m409(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 43u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SendQueryUGCRequest */
static void m410(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SetAdminQuery */
static void m411(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetAllowCachedResponse */
static void m412(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetAllowLegacyUpload */
static void m413(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetCloudFileNameFilter */
static void m414(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemContent */
static void m415(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 52u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemDescription */
static void m416(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemMetadata */
static void m417(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 49u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemPreview */
static void m418(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 53u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemTitle */
static void m420(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemUpdateLanguage */
static void m421(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetItemVisibility */
static void m422(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 50u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetLanguage */
static void m423(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetMatchAnyTag */
static void m424(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetRankedByTrendDays */
static void m425(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetRequiredGameVersions */
static void m426(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 65u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnAdditionalPreviews */
static void m427(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnChildren */
static void m428(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnKeyValueTags */
static void m429(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnLongDescription */
static void m430(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnMetadata */
static void m431(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnOnlyIDs */
static void m432(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnPlaytimeStats */
static void m433(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetReturnTotalOnly */
static void m434(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetSearchText */
static void m435(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetTimeCreatedDateRange */
static void m436(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetTimeUpdatedDateRange */
static void m437(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::SetUserItemVote */
static void m438(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 68u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::ShowWorkshopEULA */
static void m439(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 91u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::StartItemUpdate */
static void m440(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t, uint64_t);
    void *fn = steam_slot(c, self, 45u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTrackingForAllItems */
static void m443(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 84u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SubmitItemUpdate */
static void m444(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 66u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SubscribeItem */
static void m445(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 72u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::SuspendDownloads */
static void m446(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 81u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* STEAMUGC_INTERFACE_VERSION020::UnsubscribeItem */
static void m447(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 73u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewFile */
static void m448(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 60u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewVideo */
static void m449(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint32_t, const char *);
    void *fn = steam_slot(c, self, 61u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::AttachLeaderboardUGC */
static void m450(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::ClearAchievement */
static void m451(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 8u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntries */
static void m452(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntriesForUsers */
static void m453(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, void *, int32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::FindLeaderboard */
static void m454(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 23u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::FindOrCreateLeaderboard */
static void m455(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 22u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1, a2);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievement */
static void m456(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 6u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAchievedPercent */
static void m457(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 37u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAndUnlockTime */
static void m458(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    void *fn = steam_slot(c, self, 9u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 1u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementDisplayAttribute */
static void m459(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 12u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementIcon */
static void m460(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 11u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementName */
static void m461(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsFloat */
static void m462(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    void *fn = steam_slot(c, self, 44u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsInt32 */
static void m463(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *, void *);
    void *fn = steam_slot(c, self, 43u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetDownloadedLeaderboardEntry */
static void m464(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, void *, int32_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out(c, R32(args + 12u), 28u);
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatDouble */
static void m465(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 40u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryDouble */
static void m466(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 42u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryInt64 */
static void m467(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatInt64 */
static void m468(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 39u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardDisplayType */
static void m469(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardEntryCount */
static void m470(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardName */
static void m471(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardSortMethod */
static void m472(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetNumAchievements */
static void m475(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 14u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetNumberOfCurrentPlayers */
static void m476(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetStatFloat */
static void m477(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetStatInt32 */
static void m478(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, void *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievement */
static void m479(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievementAndUnlockTime */
static void m480(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *, void *);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatFloat */
static void m481(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatInt32 */
static void m482(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::IndicateAchievementProgress */
static void m483(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestCurrentStats */
static void m484(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalAchievementPercentages */
static void m485(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 34u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalStats */
static void m486(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 38u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::RequestUserStats */
static void m487(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::ResetAllStats */
static void m488(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 21u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetAchievement */
static void m489(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 7u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetStatFloat */
static void m490(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::SetStatInt32 */
static void m491(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::StoreStats */
static void m492(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::UpdateAvgRateStat */
static void m493(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, float, double);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    float a1 = RF(args + 4u);
    double a2 = RD(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* STEAMUSERSTATS_INTERFACE_VERSION012::UploadLeaderboardScore */
static void m494(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t, int32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    void *a3 = steam_in_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 4u);
    int32_t a4 = (int32_t)R32(args + 20u);
    uint64_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    steam_ret64(c, (uint64_t)r);
}
/* STEAMVIDEO_INTERFACE_V002::GetOPFSettings */
static void m495(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V002::GetVideoURL */
static void m497(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 0u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V002::IsBroadcasting */
static void m498(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 1u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* STEAMVIDEO_INTERFACE_V007::GetOPFSettings */
static void m499(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 2u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V007::GetVideoURL */
static void m501(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 0u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* STEAMVIDEO_INTERFACE_V007::IsBroadcasting */
static void m502(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 1u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::BReleaseSteamPipe */
static void m503(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::BShutdownIfAllPipesClosed */
static void m504(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamClient020::ConnectToGlobalUser */
static void m505(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamClient020::CreateLocalUser */
static void m506(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamClient020::CreateSteamPipe */
static void m507(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient020::GetIPCCallCount */
static void m508(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient020::GetISteamAppList */
static void m509(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 28u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamApps */
static void m510(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 15u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamController */
static void m511(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 26u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamFriends */
static void m512(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 8u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameSearch */
static void m513(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameServer */
static void m514(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 6u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGameServerStats */
static void m515(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 14u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamGenericInterface */
static void m516(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamHTMLSurface */
static void m517(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 31u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamHTTP */
static void m518(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamInput */
static void m519(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 38u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamInventory */
static void m520(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMatchmaking */
static void m521(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMatchmakingServers */
static void m522(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 11u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMusic */
static void m523(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamMusicRemote */
static void m524(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 30u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamNetworking */
static void m525(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 16u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamParentalSettings */
static void m526(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 37u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamParties */
static void m527(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 39u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamRemotePlay */
static void m528(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 40u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamRemoteStorage */
static void m529(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 17u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamScreenshots */
static void m530(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUGC */
static void m531(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 27u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUser */
static void m532(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUserStats */
static void m533(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::GetISteamUtils */
static void m534(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, const char *);
    void *fn = steam_slot(c, self, 9u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void * r = ((F)fn)(self, a0, a1);
    c->eax = steam_proxy(c, r, a1);
}
/* SteamClient020::GetISteamVideo */
static void m535(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient020::ReleaseUser */
static void m536(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamClient021::BReleaseSteamPipe */
static void m539(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamClient021::BShutdownIfAllPipesClosed */
static void m540(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamClient021::ConnectToGlobalUser */
static void m541(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamClient021::CreateLocalUser */
static void m542(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamClient021::CreateSteamPipe */
static void m543(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient021::GetIPCCallCount */
static void m544(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamClient021::GetISteamApps */
static void m545(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 15u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamController */
static void m546(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamFriends */
static void m547(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 8u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGameSearch */
static void m548(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGameServer */
static void m549(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 6u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGameServerStats */
static void m550(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 14u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamGenericInterface */
static void m551(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamHTMLSurface */
static void m552(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamHTTP */
static void m553(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamInput */
static void m554(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 36u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamInventory */
static void m555(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 33u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMatchmaking */
static void m556(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMatchmakingServers */
static void m557(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 11u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMusic */
static void m558(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 27u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamMusicRemote */
static void m559(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 28u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamNetworking */
static void m560(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 16u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamParentalSettings */
static void m561(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 35u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamParties */
static void m562(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 37u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamRemotePlay */
static void m563(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 38u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamRemoteStorage */
static void m564(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 17u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamScreenshots */
static void m565(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUGC */
static void m566(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 26u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUser */
static void m567(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUserStats */
static void m568(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::GetISteamUtils */
static void m569(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, const char *);
    void *fn = steam_slot(c, self, 9u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    void * r = ((F)fn)(self, a0, a1);
    c->eax = steam_proxy(c, r, a1);
}
/* SteamClient021::GetISteamVideo */
static void m570(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void * (*F)(void *, int32_t, int32_t, const char *);
    void *fn = steam_slot(c, self, 34u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    void * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_proxy(c, r, a2);
}
/* SteamClient021::ReleaseUser */
static void m571(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::ActivateActionSet */
static void m574(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::ActivateActionSetLayer */
static void m575(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::DeactivateActionSetLayer */
static void m576(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::DeactivateAllActionSetLayers */
static void m577(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamController008::GetActionOriginFromXboxOrigin */
static void m578(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetActionSetHandle */
static void m579(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 4u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetActiveActionSetLayers */
static void m580(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), 16, 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetAnalogActionHandle */
static void m581(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetAnalogActionOrigins */
static void m582(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetConnectedControllers */
static void m583(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetControllerBindingRevision */
static void m584(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::GetControllerForGamepadIndex */
static void m585(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 27u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetCurrentActionSet */
static void m586(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetDigitalActionHandle */
static void m587(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 11u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamController008::GetDigitalActionOrigins */
static void m588(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetGamepadIndexForController */
static void m589(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetGlyphForActionOrigin */
static void m590(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 17u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetGlyphForXboxOrigin */
static void m591(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 30u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetInputTypeForHandle */
static void m592(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamController008::GetStringForActionOrigin */
static void m593(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::GetStringForXboxOrigin */
static void m594(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 29u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamController008::Init */
static void m595(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::RunFrame */
static void m596(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    ((F)fn)(self);
}
/* SteamController008::SetLEDColor */
static void m597(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamController008::ShowBindingPanel */
static void m598(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::Shutdown */
static void m599(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamController008::StopAnalogActionMomentum */
static void m600(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamController008::TranslateActionOrigin */
static void m601(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 32u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamController008::TriggerHapticPulse */
static void m602(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamController008::TriggerRepeatedHapticPulse */
static void m603(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamController008::TriggerVibration */
static void m604(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    void *fn = steam_slot(c, self, 23u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends017::ActivateGameOverlay */
static void m605(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 28u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayInviteDialog */
static void m606(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayInviteDialogConnectString */
static void m607(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 75u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayRemotePlayTogetherInviteDialog */
static void m608(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 73u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::ActivateGameOverlayToStore */
static void m609(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, int32_t);
    void *fn = steam_slot(c, self, 31u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ActivateGameOverlayToUser */
static void m610(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, uint64_t);
    void *fn = steam_slot(c, self, 29u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t a1 = (uint64_t)R64(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ActivateGameOverlayToWebPage */
static void m611(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 30u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::ClearRichPresence */
static void m612(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 44u);
    ((F)fn)(self);
}
/* SteamFriends017::CloseClanChatWindowInSteam */
static void m613(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 63u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::DownloadClanActivityCounts */
static void m614(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 23u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 8u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::EnumerateFollowingList */
static void m615(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 69u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::GetChatMemberByIndex */
static void m616(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 57u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanActivityCounts */
static void m617(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    void *a3 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::GetClanByIndex */
static void m618(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanChatMemberCount */
static void m619(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 56u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanChatMessage */
static void m620(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 59u);
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
static void m621(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanName */
static void m622(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetClanOfficerByIndex */
static void m623(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanOfficerCount */
static void m624(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetClanOwner */
static void m625(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetClanTag */
static void m626(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetCoplayFriend */
static void m627(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 51u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetCoplayFriendCount */
static void m628(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 50u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFollowerCount */
static void m629(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 67u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::GetFriendByIndex */
static void m630(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 4u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetFriendCoplayGame */
static void m631(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 53u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCoplayTime */
static void m632(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 52u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCount */
static void m633(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 3u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendCountFromSource */
static void m634(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendFromSourceByIndex */
static void m635(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamFriends017::GetFriendGamePlayed */
static void m636(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 24u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::GetFriendMessage */
static void m637(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 66u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    void *a2 = steam_out_buffer(c, R32(args + 12u), (int64_t)(int32_t)R32(args + 16u), 1u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendPersonaName */
static void m638(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendPersonaNameHistory */
static void m639(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendPersonaState */
static void m640(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendRelationship */
static void m641(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendRichPresence */
static void m642(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendRichPresenceKeyByIndex */
static void m643(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 47u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetFriendRichPresenceKeyCount */
static void m644(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 46u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendSteamLevel */
static void m645(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupCount */
static void m646(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupIDByIndex */
static void m647(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int16_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int16_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupMembersCount */
static void m648(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int16_t);
    void *fn = steam_slot(c, self, 15u);
    int16_t a0 = (int16_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetFriendsGroupMembersList */
static void m649(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int16_t, void *, int32_t);
    void *fn = steam_slot(c, self, 16u);
    int16_t a0 = (int16_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 8u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamFriends017::GetFriendsGroupName */
static void m650(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int16_t);
    void *fn = steam_slot(c, self, 14u);
    int16_t a0 = (int16_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetLargeFriendAvatar */
static void m651(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetMediumFriendAvatar */
static void m652(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetNumChatsWithUnreadPriorityMessages */
static void m653(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 72u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetPersonaName */
static void m654(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetPersonaState */
static void m655(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetPlayerNickname */
static void m656(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamFriends017::GetSmallFriendAvatar */
static void m657(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::GetUserRestrictions */
static void m658(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 42u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamFriends017::HasFriend */
static void m659(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::InviteUserToGame */
static void m660(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 49u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanChatAdmin */
static void m661(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 60u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanChatWindowOpenInSteam */
static void m662(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 61u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanOfficialGameGroup */
static void m663(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 71u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsClanPublic */
static void m664(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 70u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::IsFollowing */
static void m665(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 68u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::IsUserInSource */
static void m666(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::JoinClanChatRoom */
static void m667(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 54u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::LeaveClanChatRoom */
static void m668(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 55u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::OpenClanChatWindowInSteam */
static void m669(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 62u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::RegisterProtocolInOverlayBrowser */
static void m670(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 74u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::ReplyToFriendMessage */
static void m671(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 65u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::RequestClanOfficerList */
static void m672(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 38u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::RequestFriendRichPresence */
static void m673(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 48u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::RequestUserInformation */
static void m674(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SendClanChatMessage */
static void m675(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 58u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SetInGameVoiceSpeaking */
static void m676(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    ((F)fn)(self, a0, a1);
}
/* SteamFriends017::SetListenForFriendsMessages */
static void m677(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 64u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamFriends017::SetPersonaName */
static void m678(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamFriends017::SetPlayedWith */
static void m679(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamFriends017::SetRichPresence */
static void m680(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 43u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::AssociateWithClan */
static void m681(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::BLoggedOn */
static void m682(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BSecure */
static void m683(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BUpdateUserData */
static void m684(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::BeginAuthSession */
static void m685(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::CancelAuthTicket */
static void m686(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 28u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::ClearAllKeyValues */
static void m687(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 19u);
    ((F)fn)(self);
}
/* SteamGameServer014::ComputeNewPlayerCompatibility */
static void m688(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::CreateUnauthenticatedUserConnection */
static void m689(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 39u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer014::EndAuthSession */
static void m690(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::GetAuthSessionTicket */
static void m691(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 25u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::GetGameplayStats */
static void m692(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    ((F)fn)(self);
}
/* SteamGameServer014::GetNextOutgoingPacket */
static void m693(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 35u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::GetServerReputation */
static void m694(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 32u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer014::GetSteamID */
static void m695(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer014::HandleIncomingPacket */
static void m696(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 34u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::LogOff */
static void m697(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamGameServer014::LogOn */
static void m698(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::LogOnAnonymous */
static void m699(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 6u);
    ((F)fn)(self);
}
/* SteamGameServer014::RequestUserGroupStatus */
static void m700(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::SendUserConnectAndAuthenticate_DEPRECATED */
static void m701(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 38u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer014::SendUserDisconnect_DEPRECATED */
static void m702(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetAdvertiseServerActive */
static void m703(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 24u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetBotPlayerCount */
static void m704(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetDedicatedServer */
static void m705(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 4u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameData */
static void m706(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 22u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameDescription */
static void m707(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetGameTags */
static void m708(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetKeyValue */
static void m709(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 20u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamGameServer014::SetMapName */
static void m710(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetMaxPlayerCount */
static void m711(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetModDir */
static void m712(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetPasswordProtected */
static void m713(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 16u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetProduct */
static void m714(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetRegion */
static void m715(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 23u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetServerName */
static void m716(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetSpectatorPort */
static void m717(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint16_t);
    void *fn = steam_slot(c, self, 17u);
    uint16_t a0 = (uint16_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer014::SetSpectatorServerName */
static void m718(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 18u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer014::UserHasLicenseForApp */
static void m719(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamGameServer014::WasRestartRequested */
static void m720(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::AssociateWithClan */
static void m721(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer015::BLoggedOn */
static void m722(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::BSecure */
static void m723(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::BUpdateUserData */
static void m724(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, uint32_t);
    void *fn = steam_slot(c, self, 41u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    uint32_t a2 = (uint32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::BeginAuthSession */
static void m725(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 26u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamGameServer015::CancelAuthTicket */
static void m726(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 28u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::ClearAllKeyValues */
static void m727(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 19u);
    ((F)fn)(self);
}
/* SteamGameServer015::ComputeNewPlayerCompatibility */
static void m728(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer015::CreateUnauthenticatedUserConnection */
static void m729(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 39u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer015::EndAuthSession */
static void m730(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::GetGameplayStats */
static void m732(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    ((F)fn)(self);
}
/* SteamGameServer015::GetNextOutgoingPacket */
static void m733(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 35u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 2u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamGameServer015::GetServerReputation */
static void m734(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 32u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServer015::GetSteamID */
static void m735(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamGameServer015::HandleIncomingPacket */
static void m736(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 34u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::LogOff */
static void m737(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamGameServer015::LogOn */
static void m738(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::LogOnAnonymous */
static void m739(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 6u);
    ((F)fn)(self);
}
/* SteamGameServer015::RequestUserGroupStatus */
static void m740(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::SendUserConnectAndAuthenticate_DEPRECATED */
static void m741(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 38u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServer015::SendUserDisconnect_DEPRECATED */
static void m742(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 40u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetAdvertiseServerActive */
static void m743(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 24u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetBotPlayerCount */
static void m744(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetDedicatedServer */
static void m745(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 4u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetGameData */
static void m746(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 22u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetGameDescription */
static void m747(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetGameTags */
static void m748(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetKeyValue */
static void m749(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 20u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamGameServer015::SetMapName */
static void m750(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 15u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetMaxPlayerCount */
static void m751(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetModDir */
static void m752(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 3u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetPasswordProtected */
static void m753(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 16u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetProduct */
static void m754(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 1u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetRegion */
static void m755(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 23u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetServerName */
static void m756(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetSpectatorPort */
static void m757(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint16_t);
    void *fn = steam_slot(c, self, 17u);
    uint16_t a0 = (uint16_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamGameServer015::SetSpectatorServerName */
static void m758(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 18u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    ((F)fn)(self, a0);
}
/* SteamGameServer015::UserHasLicenseForApp */
static void m759(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamGameServer015::WasRestartRequested */
static void m760(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::ClearUserAchievement */
static void m761(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserAchievement */
static void m762(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 3u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserStatFloat */
static void m763(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 2u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::GetUserStatInt32 */
static void m764(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, void *);
    void *fn = steam_slot(c, self, 1u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::RequestUserStats */
static void m765(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 0u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServerStats001::SetUserAchievement */
static void m766(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::SetUserStatFloat */
static void m767(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, float);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    float a2 = RF(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::SetUserStatInt32 */
static void m768(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, int32_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamGameServerStats001::StoreUserStats */
static void m769(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamGameServerStats001::UpdateUserAvgRateStat */
static void m770(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, float, double);
    void *fn = steam_slot(c, self, 6u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    float a2 = RF(args + 12u);
    double a3 = RD(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::ActivateActionSet */
static void m771(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::ActivateActionSetLayer */
static void m772(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::BNewDataAvailable */
static void m773(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 5u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::BWaitForData */
static void m774(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    bool a0 = (R8(args + 0u) != 0);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::DeactivateActionSetLayer */
static void m775(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::DeactivateAllActionSetLayers */
static void m776(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 14u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamInput006::EnableDeviceCallbacks */
static void m778(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamInput006::GetActionOriginFromXboxOrigin */
static void m779(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 42u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetActionSetHandle */
static void m780(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 9u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetActiveActionSetLayers */
static void m781(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), 16, 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetAnalogActionHandle */
static void m782(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 20u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetAnalogActionOrigins */
static void m783(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    void *fn = steam_slot(c, self, 22u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetConnectedControllers */
static void m784(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 6u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), 16, 8u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetControllerForGamepadIndex */
static void m785(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 38u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetCurrentActionSet */
static void m786(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetDeviceBindingRevision */
static void m787(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *);
    void *fn = steam_slot(c, self, 44u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::GetDigitalActionHandle */
static void m788(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 16u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamInput006::GetDigitalActionOrigins */
static void m789(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, uint64_t, void *);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    uint64_t a2 = (uint64_t)R64(args + 16u);
    void *a3 = steam_out_buffer(c, R32(args + 24u), 8, 4u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetGamepadIndexForController */
static void m790(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 39u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetGlyphForActionOrigin_Legacy */
static void m791(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphForXboxOrigin */
static void m792(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 41u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphPNGForActionOrigin */
static void m793(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, int32_t, uint32_t);
    void *fn = steam_slot(c, self, 23u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetGlyphSVGForActionOrigin */
static void m794(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, uint32_t);
    void *fn = steam_slot(c, self, 24u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetInputTypeForHandle */
static void m795(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetRemotePlaySessionID */
static void m796(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 45u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetSessionInputConfigurationSettings */
static void m797(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint16_t (*F)(void *);
    void *fn = steam_slot(c, self, 46u);
    uint16_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamInput006::GetStringForActionOrigin */
static void m798(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 26u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForAnalogActionName */
static void m799(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 27u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForDigitalActionName */
static void m800(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::GetStringForXboxOrigin */
static void m801(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 40u);
    int32_t a0 = (int32_t)R32(args + 0u);
    const char * r = ((F)fn)(self, a0);
    c->eax = steam_guest_string(c, r);
}
/* SteamInput006::Init */
static void m802(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 0u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::Legacy_TriggerHapticPulse */
static void m803(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamInput006::Legacy_TriggerRepeatedHapticPulse */
static void m804(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, int32_t, uint16_t, uint16_t, uint16_t, uint32_t);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    uint32_t a5 = (uint32_t)R32(args + 24u);
    ((F)fn)(self, a0, a1, a2, a3, a4, a5);
}
/* SteamInput006::RunFrame */
static void m805(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 3u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamInput006::SetInputActionManifestFilePath */
static void m806(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 2u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::SetLEDColor */
static void m807(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint8_t, uint8_t, uint8_t, uint32_t);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint8_t a1 = (uint8_t)R32(args + 8u);
    uint8_t a2 = (uint8_t)R32(args + 12u);
    uint8_t a3 = (uint8_t)R32(args + 16u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamInput006::ShowBindingPanel */
static void m808(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::Shutdown */
static void m809(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamInput006::StopAnalogActionMomentum */
static void m810(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamInput006::TranslateActionOrigin */
static void m811(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 43u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamInput006::TriggerVibration */
static void m813(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamInput006::TriggerVibrationExtended */
static void m814(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint16_t, uint16_t, uint16_t, uint16_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint16_t a3 = (uint16_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    ((F)fn)(self, a0, a1, a2, a3, a4);
}
/* SteamMatchGameSearch001::AcceptGame */
static void m815(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 3u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::AddGameSearchParams */
static void m816(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 0u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::CancelRequestPlayersForGame */
static void m817(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 11u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::DeclineGame */
static void m818(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 4u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::EndGame */
static void m819(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::EndGameSearch */
static void m820(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 6u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::HostConfirmGameStart */
static void m821(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::RequestPlayersForGame */
static void m822(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 9u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::RetrieveConnectionDetails */
static void m823(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, void *, int32_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SearchForGameSolo */
static void m824(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 2u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SearchForGameWithLobby */
static void m825(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 1u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SetConnectionDetails */
static void m826(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 8u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SetGameHostParams */
static void m827(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, const char *, const char *);
    void *fn = steam_slot(c, self, 7u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamMatchGameSearch001::SubmitPlayerResult */
static void m828(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    int32_t a2 = (int32_t)R32(args + 16u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::AddFavoriteGame */
static void m829(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, uint32_t, uint16_t, uint16_t, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 2u);
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
static void m830(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListDistanceFilter */
static void m831(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 9u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListFilterSlotsAvailable */
static void m832(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 8u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListNearValueFilter */
static void m833(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t);
    void *fn = steam_slot(c, self, 7u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamMatchMaking009::AddRequestLobbyListNumericalFilter */
static void m834(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 6u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::AddRequestLobbyListResultCountFilter */
static void m835(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::AddRequestLobbyListStringFilter */
static void m836(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, const char *, const char *, int32_t);
    void *fn = steam_slot(c, self, 5u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    const char *a1 = steam_in_string(c, R32(args + 4u));
    int32_t a2 = (int32_t)R32(args + 8u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::CreateLobby */
static void m837(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 13u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::DeleteLobbyData */
static void m838(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 23u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetFavoriteGame */
static void m839(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *, void *, void *, void *, void *);
    void *fn = steam_slot(c, self, 1u);
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
static void m840(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyByIndex */
static void m841(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 12u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetLobbyChatEntry */
static void m842(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, int32_t, void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 27u);
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
static void m843(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, const char *);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char * r = ((F)fn)(self, a0, a1);
    c->eax = steam_guest_string(c, r);
}
/* SteamMatchMaking009::GetLobbyDataByIndex */
static void m844(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t, void *, int32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 22u);
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
static void m845(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 21u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyGameServer */
static void m846(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, void *, void *);
    void *fn = steam_slot(c, self, 30u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 4u);
    void *a2 = steam_out(c, R32(args + 12u), 2u);
    void *a3 = steam_out(c, R32(args + 16u), 8u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::GetLobbyMemberByIndex */
static void m847(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetLobbyMemberData */
static void m848(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, uint64_t, uint64_t, const char *);
    void *fn = steam_slot(c, self, 24u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    const char *a2 = steam_in_string(c, R32(args + 16u));
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamMatchMaking009::GetLobbyMemberLimit */
static void m849(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 32u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::GetLobbyOwner */
static void m850(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 35u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret_sret(c, sret, r);
}
/* SteamMatchMaking009::GetNumLobbyMembers */
static void m851(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamMatchMaking009::InviteUserToLobby */
static void m852(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::JoinLobby */
static void m853(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 14u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::LeaveLobby */
static void m854(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamMatchMaking009::RemoveFavoriteGame */
static void m855(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t, uint16_t, uint16_t, uint32_t);
    void *fn = steam_slot(c, self, 3u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    uint16_t a2 = (uint16_t)R32(args + 8u);
    uint16_t a3 = (uint16_t)R32(args + 12u);
    uint32_t a4 = (uint32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::RequestLobbyData */
static void m856(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 28u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::RequestLobbyList */
static void m857(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 4u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamMatchMaking009::SendLobbyChatMsg */
static void m858(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t);
    void *fn = steam_slot(c, self, 26u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLinkedLobby */
static void m859(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 37u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyData */
static void m860(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyGameServer */
static void m861(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t, uint64_t);
    void *fn = steam_slot(c, self, 29u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    uint64_t a3 = (uint64_t)R64(args + 16u);
    ((F)fn)(self, a0, a1, a2, a3);
}
/* SteamMatchMaking009::SetLobbyJoinable */
static void m862(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, bool);
    void *fn = steam_slot(c, self, 34u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool a1 = (R8(args + 8u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyMemberData */
static void m863(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, const char *, const char *);
    void *fn = steam_slot(c, self, 25u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 8u));
    const char *a2 = steam_in_string(c, R32(args + 12u));
    ((F)fn)(self, a0, a1, a2);
}
/* SteamMatchMaking009::SetLobbyMemberLimit */
static void m864(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 31u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyOwner */
static void m865(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 36u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMaking009::SetLobbyType */
static void m866(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 33u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamMatchMakingServers002::CancelServerQuery */
static void m868(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 16u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamNetworking006::AcceptP2PSessionWithUser */
static void m884(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 3u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::AllowP2PPacketRelay */
static void m885(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, bool);
    void *fn = steam_slot(c, self, 7u);
    bool a0 = (R8(args + 0u) != 0);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CloseP2PChannelWithUser */
static void m886(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, int32_t);
    void *fn = steam_slot(c, self, 5u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CloseP2PSessionWithUser */
static void m887(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 4u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::CreateP2PConnectionSocket */
static void m888(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, uint64_t, int32_t, int32_t, bool);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t a1 = (int32_t)R32(args + 8u);
    int32_t a2 = (int32_t)R32(args + 12u);
    bool a3 = (R8(args + 16u) != 0);
    uint32_t r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::DestroyListenSocket */
static void m889(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 12u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::DestroySocket */
static void m890(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::GetMaxPacketSize */
static void m892(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 21u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::GetSocketConnectionType */
static void m894(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 20u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworking006::IsDataAvailable */
static void m896(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 16u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::IsDataAvailableOnSocket */
static void m897(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::IsP2PPacketAvailable */
static void m898(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 1u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::ReadP2PPacket */
static void m899(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t, void *, void *, int32_t);
    void *fn = steam_slot(c, self, 2u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    void *a3 = steam_out(c, R32(args + 12u), 8u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::RetrieveData */
static void m900(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *, void *);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::RetrieveDataFromSocket */
static void m901(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 15u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    void *a3 = steam_out(c, R32(args + 12u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::SendDataOnSocket */
static void m902(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, uint32_t, bool);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworking006::SendP2PPacket */
static void m903(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, uint32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 0u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    int32_t a4 = (int32_t)R32(args + 20u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::AcceptConnection */
static void m910(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::BeginAsyncRequestFakeIP */
static void m911(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 42u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::CloseConnection */
static void m912(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, int32_t, const char *, bool);
    void *fn = steam_slot(c, self, 5u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    bool a3 = (R8(args + 12u) != 0);
    bool r = ((F)fn)(self, a0, a1, a2, a3);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::CloseListenSocket */
static void m913(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 6u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::CreatePollGroup */
static void m924(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::DestroyPollGroup */
static void m926(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 25u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::FlushMessagesOnConnection */
static void m928(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 13u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetConnectionName */
static void m932(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::GetConnectionUserData */
static void m934(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamNetworkingSockets012::GetDetailedConnectionStatus */
static void m935(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetHostedDedicatedServerAddress */
static void m938(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 33u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetHostedDedicatedServerPOPID */
static void m939(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 32u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetHostedDedicatedServerPort */
static void m940(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint16_t (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    uint16_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::GetListenSocketAddress */
static void m942(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 18u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 2u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::GetRemoteFakeIPForConnection */
static void m943(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 45u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 2u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::InitAuthentication */
static void m944(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 22u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::RunCallbacks */
static void m950(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 41u);
    ((F)fn)(self);
}
/* SteamNetworkingSockets012::SendMessageToConnection */
static void m951(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *, uint32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 11u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_in_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    uint32_t a2 = (uint32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingSockets012::SetConnectionName */
static void m954(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 9u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    ((F)fn)(self, a0, a1);
}
/* SteamNetworkingSockets012::SetConnectionPollGroup */
static void m955(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, uint32_t);
    void *fn = steam_slot(c, self, 26u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingSockets012::SetConnectionUserData */
static void m956(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t, int64_t);
    void *fn = steam_slot(c, self, 7u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int64_t a1 = (int64_t)R64(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingUtils004::CheckPingDataUpToDate */
static void m958(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, float);
    void *fn = steam_slot(c, self, 7u);
    float a0 = RF(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamNetworkingUtils004::GetConfigValueInfo */
static void m963(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 18u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    const char * r = ((F)fn)(self, a0, a1, a2);
    c->eax = steam_guest_string(c, r);
}
/* SteamNetworkingUtils004::GetDirectPingToPOP */
static void m964(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetIPv4FakeIPType */
static void m965(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 14u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetLocalTimestamp */
static void m967(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int64_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    int64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamNetworkingUtils004::GetPOPCount */
static void m968(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 10u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetPOPList */
static void m969(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 11u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::GetPingToDataCenter */
static void m970(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint32_t, void *);
    void *fn = steam_slot(c, self, 8u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::IterateGenericEditableConfigValues */
static void m973(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    void *fn = steam_slot(c, self, 19u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::SteamNetworkingIPAddr_GetFakeIPType */
static void m977(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *);
    void *fn = steam_slot(c, self, 22u);
    void *a0 = steam_out(c, R32(args + 0u), 2u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamNetworkingUtils004::SteamNetworkingIPAddr_ParseString */
static void m978(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, const char *);
    void *fn = steam_slot(c, self, 21u);
    void *a0 = steam_out(c, R32(args + 0u), 2u);
    const char *a1 = steam_in_string(c, R32(args + 4u));
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamParties002::CancelReservation */
static void m980(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 8u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamParties002::ChangeNumOpenSlots */
static void m981(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 9u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamParties002::DestroyBeacon */
static void m983(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 10u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamParties002::GetBeaconByIndex */
static void m985(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 1u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamParties002::GetNumActiveBeacons */
static void m987(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamParties002::GetNumAvailableBeaconLocations */
static void m988(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *);
    void *fn = steam_slot(c, self, 4u);
    void *a0 = steam_out(c, R32(args + 0u), 4u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamParties002::JoinParty */
static void m989(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 3u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamParties002::OnReservationCompleted */
static void m990(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint64_t);
    void *fn = steam_slot(c, self, 7u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 8u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser021::AdvertiseGame */
static void m991(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 19u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser021::BIsBehindNAT */
static void m992(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneIdentifying */
static void m993(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneRequiringVerification */
static void m994(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsPhoneVerified */
static void m995(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 25u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BIsTwoFactorEnabled */
static void m996(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BLoggedOn */
static void m997(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BSetDurationControlOnlineState */
static void m998(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 31u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::BeginAuthSession */
static void m999(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 14u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser021::CancelAuthTicket */
static void m1000(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 16u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser021::DecompressVoice */
static void m1001(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
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
static void m1002(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 15u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser021::GetAuthSessionTicket */
static void m1003(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 13u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    uint32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetDurationControl */
static void m1005(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 30u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::GetEncryptedAppTicket */
static void m1006(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 21u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::GetGameBadgeLevel */
static void m1007(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    void *fn = steam_slot(c, self, 22u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetHSteamUser */
static void m1008(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetMarketEligibility */
static void m1009(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 29u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::GetPlayerSteamLevel */
static void m1010(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::GetSteamID */
static void m1011(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamUser021::GetUserDataFolder */
static void m1012(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 6u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUser021::GetVoiceOptimalSampleRate */
static void m1013(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser021::InitiateGameConnection_DEPRECATED */
static void m1014(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t, uint32_t, uint16_t, bool);
    void *fn = steam_slot(c, self, 3u);
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
static void m1015(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 20u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::RequestStoreAuthURL */
static void m1016(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 24u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser021::StartVoiceRecording */
static void m1017(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamUser021::StopVoiceRecording */
static void m1018(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    ((F)fn)(self);
}
/* SteamUser021::TerminateGameConnection_DEPRECATED */
static void m1019(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser021::UserHasLicenseForApp */
static void m1020(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser023::AdvertiseGame */
static void m1021(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 20u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    uint16_t a2 = (uint16_t)R32(args + 12u);
    ((F)fn)(self, a0, a1, a2);
}
/* SteamUser023::BIsBehindNAT */
static void m1022(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 19u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsPhoneIdentifying */
static void m1023(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsPhoneRequiringVerification */
static void m1024(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 29u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsPhoneVerified */
static void m1025(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BIsTwoFactorEnabled */
static void m1026(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BLoggedOn */
static void m1027(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BSetDurationControlOnlineState */
static void m1028(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 32u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::BeginAuthSession */
static void m1029(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t);
    void *fn = steam_slot(c, self, 15u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1, a2);
    c->eax = (uint32_t)r;
}
/* SteamUser023::CancelAuthTicket */
static void m1030(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 17u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser023::DecompressVoice */
static void m1031(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, uint32_t, void *, uint32_t, void *, uint32_t);
    void *fn = steam_slot(c, self, 11u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    void *a2 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    uint32_t a3 = (uint32_t)R32(args + 12u);
    void *a4 = steam_out(c, R32(args + 16u), 4u);
    uint32_t a5 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser023::EndAuthSession */
static void m1032(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 16u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUser023::GetAuthTicketForWebApi */
static void m1034(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 14u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetDurationControl */
static void m1036(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 31u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::GetEncryptedAppTicket */
static void m1037(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t, void *);
    void *fn = steam_slot(c, self, 22u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::GetGameBadgeLevel */
static void m1038(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, bool);
    void *fn = steam_slot(c, self, 23u);
    int32_t a0 = (int32_t)R32(args + 0u);
    bool a1 = (R8(args + 4u) != 0);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetHSteamUser */
static void m1039(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetMarketEligibility */
static void m1040(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 30u);
    uint64_t r = ((F)fn)(self);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::GetPlayerSteamLevel */
static void m1041(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser023::GetSteamID */
static void m1042(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    uint64_t r = ((F)fn)(self);
    steam_ret_sret(c, sret, r);
}
/* SteamUser023::GetUserDataFolder */
static void m1043(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 6u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUser023::GetVoiceOptimalSampleRate */
static void m1044(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 12u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUser023::InitiateGameConnection_DEPRECATED */
static void m1045(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, void *, int32_t, uint64_t, uint32_t, uint16_t, bool);
    void *fn = steam_slot(c, self, 3u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t a2 = (uint64_t)R64(args + 8u);
    uint32_t a3 = (uint32_t)R32(args + 16u);
    uint16_t a4 = (uint16_t)R32(args + 20u);
    bool a5 = (R8(args + 24u) != 0);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4, a5);
    c->eax = (uint32_t)r;
}
/* SteamUser023::RequestEncryptedAppTicket */
static void m1046(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, void *, int32_t);
    void *fn = steam_slot(c, self, 21u);
    void *a0 = steam_in_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    int32_t a1 = (int32_t)R32(args + 4u);
    uint64_t r = ((F)fn)(self, a0, a1);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::RequestStoreAuthURL */
static void m1047(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 25u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUser023::StartVoiceRecording */
static void m1048(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 7u);
    ((F)fn)(self);
}
/* SteamUser023::StopVoiceRecording */
static void m1049(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    ((F)fn)(self);
}
/* SteamUser023::TerminateGameConnection_DEPRECATED */
static void m1050(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, uint32_t, uint16_t);
    void *fn = steam_slot(c, self, 4u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    uint16_t a1 = (uint16_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUser023::UserHasLicenseForApp */
static void m1051(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t, uint32_t);
    void *fn = steam_slot(c, self, 18u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    uint32_t a1 = (uint32_t)R32(args + 8u);
    int32_t r = ((F)fn)(self, a0, a1);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::BOverlayNeedsPresent */
static void m1052(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 18u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::CheckFileSignature */
static void m1053(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint64_t (*F)(void *, const char *);
    void *fn = steam_slot(c, self, 19u);
    const char *a0 = steam_in_string(c, R32(args + 0u));
    uint64_t r = ((F)fn)(self, a0);
    steam_ret64(c, (uint64_t)r);
}
/* SteamUtils010::DismissFloatingGamepadTextInput */
static void m1054(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 37u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::FilterText */
static void m1055(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t, uint64_t, const char *, void *, uint32_t);
    void *fn = steam_slot(c, self, 32u);
    int32_t a0 = (int32_t)R32(args + 0u);
    uint64_t a1 = (uint64_t)R64(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 12u));
    void *a3 = steam_out_buffer(c, R32(args + 16u), (int64_t)(int32_t)R32(args + 20u), 1u);
    uint32_t a4 = (uint32_t)R32(args + 20u);
    int32_t r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetAPICallFailureReason */
static void m1056(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, uint64_t);
    void *fn = steam_slot(c, self, 12u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetAPICallResult */
static void m1057(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *, int32_t, int32_t, void *);
    void *fn = steam_slot(c, self, 13u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 8u), (int64_t)(int32_t)R32(args + 12u), 1u);
    int32_t a2 = (int32_t)R32(args + 12u);
    int32_t a3 = (int32_t)R32(args + 16u);
    void *a4 = steam_out(c, R32(args + 20u), 1u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetAppID */
static void m1058(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 9u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetConnectedUniverse */
static void m1059(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *);
    void *fn = steam_slot(c, self, 2u);
    int32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetCurrentBatteryPower */
static void m1060(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint8_t (*F)(void *);
    void *fn = steam_slot(c, self, 8u);
    uint8_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetEnteredGamepadTextInput */
static void m1061(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, void *, uint32_t);
    void *fn = steam_slot(c, self, 22u);
    void *a0 = steam_out_buffer(c, R32(args + 0u), (int64_t)(int32_t)R32(args + 4u), 1u);
    uint32_t a1 = (uint32_t)R32(args + 4u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetEnteredGamepadTextLength */
static void m1062(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 21u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetIPCCallCount */
static void m1063(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 15u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetIPCountry */
static void m1064(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 4u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils010::GetIPv6ConnectivityState */
static void m1065(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef int32_t (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 33u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t r = ((F)fn)(self, a0);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetImageRGBA */
static void m1066(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, int32_t);
    void *fn = steam_slot(c, self, 6u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out_buffer(c, R32(args + 4u), (int64_t)(int32_t)R32(args + 8u), 1u);
    int32_t a2 = (int32_t)R32(args + 8u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetImageSize */
static void m1067(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, void *, void *);
    void *fn = steam_slot(c, self, 5u);
    int32_t a0 = (int32_t)R32(args + 0u);
    void *a1 = steam_out(c, R32(args + 4u), 4u);
    void *a2 = steam_out(c, R32(args + 8u), 4u);
    bool r = ((F)fn)(self, a0, a1, a2);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::GetSecondsSinceAppActive */
static void m1068(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 0u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetSecondsSinceComputerActive */
static void m1069(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 1u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetServerRealTime */
static void m1070(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef uint32_t (*F)(void *);
    void *fn = steam_slot(c, self, 3u);
    uint32_t r = ((F)fn)(self);
    c->eax = (uint32_t)r;
}
/* SteamUtils010::GetSteamUILanguage */
static void m1071(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef const char * (*F)(void *);
    void *fn = steam_slot(c, self, 23u);
    const char * r = ((F)fn)(self);
    c->eax = steam_guest_string(c, r);
}
/* SteamUtils010::InitFilterText */
static void m1072(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint32_t);
    void *fn = steam_slot(c, self, 31u);
    uint32_t a0 = (uint32_t)R32(args + 0u);
    bool r = ((F)fn)(self, a0);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsAPICallCompleted */
static void m1073(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, uint64_t, void *);
    void *fn = steam_slot(c, self, 11u);
    uint64_t a0 = (uint64_t)R64(args + 0u);
    void *a1 = steam_out(c, R32(args + 8u), 1u);
    bool r = ((F)fn)(self, a0, a1);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsOverlayEnabled */
static void m1074(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 17u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamChinaLauncher */
static void m1075(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 30u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamInBigPictureMode */
static void m1076(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 26u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamRunningInVR */
static void m1077(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 24u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsSteamRunningOnSteamDeck */
static void m1078(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 34u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::IsVRHeadsetStreamingEnabled */
static void m1079(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *);
    void *fn = steam_slot(c, self, 28u);
    bool r = ((F)fn)(self);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::SetGameLauncherMode */
static void m1080(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 36u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils010::SetOverlayNotificationInset */
static void m1081(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t, int32_t);
    void *fn = steam_slot(c, self, 25u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    ((F)fn)(self, a0, a1);
}
/* SteamUtils010::SetOverlayNotificationPosition */
static void m1082(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, int32_t);
    void *fn = steam_slot(c, self, 10u);
    int32_t a0 = (int32_t)R32(args + 0u);
    ((F)fn)(self, a0);
}
/* SteamUtils010::SetVRHeadsetStreamingEnabled */
static void m1083(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *, bool);
    void *fn = steam_slot(c, self, 29u);
    bool a0 = (R8(args + 0u) != 0);
    ((F)fn)(self, a0);
}
/* SteamUtils010::ShowFloatingGamepadTextInput */
static void m1085(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, int32_t, int32_t, int32_t);
    void *fn = steam_slot(c, self, 35u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    int32_t a2 = (int32_t)R32(args + 8u);
    int32_t a3 = (int32_t)R32(args + 12u);
    int32_t a4 = (int32_t)R32(args + 16u);
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::ShowGamepadTextInput */
static void m1086(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef bool (*F)(void *, int32_t, int32_t, const char *, uint32_t, const char *);
    void *fn = steam_slot(c, self, 20u);
    int32_t a0 = (int32_t)R32(args + 0u);
    int32_t a1 = (int32_t)R32(args + 4u);
    const char *a2 = steam_in_string(c, R32(args + 8u));
    uint32_t a3 = (uint32_t)R32(args + 12u);
    const char *a4 = steam_in_string(c, R32(args + 16u));
    bool r = ((F)fn)(self, a0, a1, a2, a3, a4);
    c->eax = r ? 1u : 0u;
}
/* SteamUtils010::StartVRDashboard */
static void m1087(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)args; (void)sret;
    typedef void (*F)(void *);
    void *fn = steam_slot(c, self, 27u);
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
    {"SteamClient021", "ISteamClient", 39u},
    {"SteamUser023", "ISteamUser", 33u},
    {"STEAMUGC_INTERFACE_VERSION020", "ISteamUGC", 94u},
    {"STEAMVIDEO_INTERFACE_V007", "ISteamVideo", 4u},
    {"SteamGameServer015", "ISteamGameServer", 42u},
    {"SteamParties002", "ISteamParties", 11u},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002", "ISteamRemotePlay", 8u},
    {"SteamNetworkingUtils004", "ISteamNetworkingUtils", 25u},
    {"SteamNetworkingSockets012", "ISteamNetworkingSockets", 47u},
    {"SteamNetworkingMessages002", "ISteamNetworkingMessages", 6u},
    {"STEAMTIMELINE_INTERFACE_V004", "ISteamTimeline", 18u},
};
const size_t steam_interface_count = 36;
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
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::BGetSessionClientResolution", 31u, 5u, 0, m180, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::BSendRemotePlayTogetherInvite", 31u, 7u, 0, m181, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::BStartRemotePlayTogether", 31u, 6u, 0, m182, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientFormFactor", 31u, 4u, 0, m183, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionClientName", 31u, 3u, 0, m184, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionCount", 31u, 0u, 0, m185, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionID", 31u, 1u, 0, m186, NULL},
    {"STEAMREMOTEPLAY_INTERFACE_VERSION002::GetSessionSteamID", 31u, 2u, 1, m187, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::BeginFileWriteBatch", 10u, 57u, 0, m188, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::CommitPublishedFileUpdate", 10u, 38u, 0, m189, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::CreatePublishedFileUpdateRequest", 10u, 31u, 0, m190, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::DeletePublishedFile", 10u, 40u, 0, m191, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EndFileWriteBatch", 10u, 58u, 0, m192, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedFilesByUserAction", 10u, 52u, 0, m193, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumeratePublishedWorkshopFiles", 10u, 53u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserPublishedFiles", 10u, 41u, 0, m195, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSharedWorkshopFiles", 10u, 49u, 0, NULL, "tampon IntPtr pRequiredTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::EnumerateUserSubscribedFiles", 10u, 43u, 0, m197, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileDelete", 10u, 6u, 0, m198, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileExists", 10u, 13u, 0, m199, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileForget", 10u, 5u, 0, m200, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FilePersisted", 10u, 14u, 0, m201, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileRead", 10u, 1u, 0, m202, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsync", 10u, 3u, 0, m203, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileReadAsyncComplete", 10u, 4u, 0, m204, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileShare", 10u, 7u, 0, m205, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWrite", 10u, 0u, 0, m206, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteAsync", 10u, 2u, 0, m207, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamCancel", 10u, 12u, 0, m208, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamClose", 10u, 11u, 0, m209, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamOpen", 10u, 9u, 0, m210, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::FileWriteStreamWriteChunk", 10u, 10u, 0, m211, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCCount", 10u, 28u, 0, m212, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetCachedUGCHandle", 10u, 29u, 0, m213, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileCount", 10u, 18u, 0, m214, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileNameAndSize", 10u, 19u, 0, m215, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileSize", 10u, 15u, 0, m216, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetFileTimestamp", 10u, 16u, 0, m217, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChange", 10u, 56u, 0, m218, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetLocalFileChangeCount", 10u, 55u, 0, m219, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedFileDetails", 10u, 39u, 0, m220, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetPublishedItemVoteDetails", 10u, 46u, 0, m221, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetQuota", 10u, 20u, 0, m222, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetSyncPlatforms", 10u, 17u, 0, m223, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDetails", 10u, 26u, 0, m224, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUGCDownloadProgress", 10u, 25u, 0, m225, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::GetUserPublishedItemVoteDetails", 10u, 48u, 0, m226, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForAccount", 10u, 21u, 0, m227, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::IsCloudEnabledForApp", 10u, 22u, 0, m228, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetCloudEnabledForApp", 10u, 23u, 0, m229, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetSyncPlatforms", 10u, 8u, 0, m230, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SetUserPublishedFileAction", 10u, 51u, 0, m231, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::SubscribePublishedFile", 10u, 42u, 0, m232, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownload", 10u, 24u, 0, m233, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCDownloadToLocation", 10u, 54u, 0, m234, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UGCRead", 10u, 27u, 0, m235, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UnsubscribePublishedFile", 10u, 44u, 0, m236, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileDescription", 10u, 35u, 0, m237, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileFile", 10u, 32u, 0, m238, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFilePreviewFile", 10u, 33u, 0, m239, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileSetChangeDescription", 10u, 45u, 0, m240, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTags", 10u, 37u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileTitle", 10u, 34u, 0, m242, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdatePublishedFileVisibility", 10u, 36u, 0, m243, NULL},
    {"STEAMREMOTESTORAGE_INTERFACE_VERSION016::UpdateUserPublishedItemVote", 10u, 47u, 0, m244, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::AddScreenshotToLibrary", 11u, 1u, 0, m245, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::AddVRScreenshotToLibrary", 11u, 8u, 0, m246, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::HookScreenshots", 11u, 3u, 0, m247, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::IsScreenshotsHooked", 11u, 7u, 0, m248, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::SetLocation", 11u, 4u, 0, m249, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TagPublishedFile", 11u, 6u, 0, m250, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TagUser", 11u, 5u, 0, m251, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::TriggerScreenshot", 11u, 2u, 0, m252, NULL},
    {"STEAMSCREENSHOTS_INTERFACE_VERSION003::WriteScreenshot", 11u, 0u, 0, m253, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::AddGamePhaseTag", 35u, 14u, 0, m254, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::AddInstantaneousTimelineEvent", 35u, 3u, 0, m255, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::AddRangeTimelineEvent", 35u, 4u, 0, m256, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::ClearTimelineTooltip", 35u, 1u, 0, m257, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::DoesEventRecordingExist", 35u, 9u, 0, m258, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::DoesGamePhaseRecordingExist", 35u, 13u, 0, m259, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::EndGamePhase", 35u, 11u, 0, m260, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::EndRangeTimelineEvent", 35u, 7u, 0, m261, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::OpenOverlayToGamePhase", 35u, 16u, 0, m262, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::OpenOverlayToTimelineEvent", 35u, 17u, 0, m263, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::RemoveTimelineEvent", 35u, 8u, 0, m264, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetGamePhaseAttribute", 35u, 15u, 0, m265, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetGamePhaseID", 35u, 12u, 0, m266, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetTimelineGameMode", 35u, 2u, 0, m267, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::SetTimelineTooltip", 35u, 0u, 0, m268, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::StartGamePhase", 35u, 10u, 0, m269, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::StartRangeTimelineEvent", 35u, 5u, 0, m270, NULL},
    {"STEAMTIMELINE_INTERFACE_V004::UpdateRangeTimelineEvent", 35u, 6u, 0, m271, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddAppDependency", 14u, 80u, 0, m272, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddDependency", 14u, 78u, 0, m273, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddExcludedTag", 14u, 21u, 0, m274, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemKeyValueTag", 14u, 53u, 0, m275, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemPreviewFile", 14u, 54u, 0, m276, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemPreviewVideo", 14u, 55u, 0, m277, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddItemToFavorites", 14u, 63u, 0, m278, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredKeyValueTag", 14u, 38u, 0, m279, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredTag", 14u, 19u, 0, m280, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::AddRequiredTagGroup", 14u, 20u, 0, NULL, "tampon IntPtr pTagGroups sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::BInitWorkshopForGameServer", 14u, 73u, 0, m282, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateItem", 14u, 40u, 0, m283, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestCursor", 14u, 2u, 0, m284, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryAllUGCRequestPage", 14u, 1u, 0, m285, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryUGCDetailsRequest", 14u, 3u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::CreateQueryUserUGCRequest", 14u, 0u, 0, m287, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::DeleteItem", 14u, 83u, 0, m288, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::DownloadItem", 14u, 72u, 0, m289, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetAppDependencies", 14u, 82u, 0, m290, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemDownloadInfo", 14u, 71u, 0, m291, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemInstallInfo", 14u, 70u, 0, m292, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemState", 14u, 69u, 0, m293, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetItemUpdateProgress", 14u, 60u, 0, m294, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetNumSubscribedItems", 14u, 67u, 0, m295, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryFirstUGCKeyValueTag", 14u, 17u, 0, m296, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCChildren", 14u, 11u, 0, m297, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCKeyValueTag", 14u, 16u, 0, m298, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCMetadata", 14u, 10u, 0, m299, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumAdditionalPreviews", 14u, 13u, 0, m300, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumKeyValueTags", 14u, 15u, 0, m301, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCNumTags", 14u, 6u, 0, m302, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCPreviewURL", 14u, 9u, 0, m303, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCResult", 14u, 5u, 0, NULL, "param\u00e8tre out SteamUGCDetails_t pDetails"},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCStatistic", 14u, 12u, 0, m305, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTag", 14u, 7u, 0, m306, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetQueryUGCTagDisplayName", 14u, 8u, 0, m307, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetSubscribedItems", 14u, 68u, 0, m308, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetUserItemVote", 14u, 62u, 0, m309, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::GetWorkshopEULAStatus", 14u, 85u, 0, m310, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::ReleaseQueryUGCRequest", 14u, 18u, 0, m311, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveAllItemKeyValueTags", 14u, 51u, 0, m312, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveAppDependency", 14u, 81u, 0, m313, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveDependency", 14u, 79u, 0, m314, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemFromFavorites", 14u, 64u, 0, m315, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemKeyValueTags", 14u, 52u, 0, m316, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RemoveItemPreview", 14u, 58u, 0, m317, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::RequestUGCDetails", 14u, 39u, 0, m318, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SendQueryUGCRequest", 14u, 4u, 0, m319, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetAllowCachedResponse", 14u, 31u, 0, m320, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetAllowLegacyUpload", 14u, 50u, 0, m321, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetCloudFileNameFilter", 14u, 32u, 0, m322, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemContent", 14u, 48u, 0, m323, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemDescription", 14u, 43u, 0, m324, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemMetadata", 14u, 45u, 0, m325, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemPreview", 14u, 49u, 0, m326, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemTags", 14u, 47u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemTitle", 14u, 42u, 0, m328, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemUpdateLanguage", 14u, 44u, 0, m329, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetItemVisibility", 14u, 46u, 0, m330, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetLanguage", 14u, 30u, 0, m331, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetMatchAnyTag", 14u, 33u, 0, m332, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetRankedByTrendDays", 14u, 35u, 0, m333, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnAdditionalPreviews", 14u, 27u, 0, m334, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnChildren", 14u, 26u, 0, m335, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnKeyValueTags", 14u, 23u, 0, m336, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnLongDescription", 14u, 24u, 0, m337, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnMetadata", 14u, 25u, 0, m338, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnOnlyIDs", 14u, 22u, 0, m339, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnPlaytimeStats", 14u, 29u, 0, m340, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetReturnTotalOnly", 14u, 28u, 0, m341, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetSearchText", 14u, 34u, 0, m342, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetTimeCreatedDateRange", 14u, 36u, 0, m343, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetTimeUpdatedDateRange", 14u, 37u, 0, m344, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SetUserItemVote", 14u, 61u, 0, m345, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::ShowWorkshopEULA", 14u, 84u, 0, m346, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StartItemUpdate", 14u, 41u, 0, m347, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::StartPlaytimeTracking", 14u, 75u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTracking", 14u, 76u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION016::StopPlaytimeTrackingForAllItems", 14u, 77u, 0, m350, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SubmitItemUpdate", 14u, 59u, 0, m351, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SubscribeItem", 14u, 65u, 0, m352, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::SuspendDownloads", 14u, 74u, 0, m353, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UnsubscribeItem", 14u, 66u, 0, m354, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewFile", 14u, 56u, 0, m355, NULL},
    {"STEAMUGC_INTERFACE_VERSION016::UpdateItemPreviewVideo", 14u, 57u, 0, m356, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddAppDependency", 27u, 87u, 0, m357, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddContentDescriptor", 27u, 63u, 0, m358, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddDependency", 27u, 85u, 0, m359, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddExcludedTag", 27u, 24u, 0, m360, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemKeyValueTag", 27u, 57u, 0, m361, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemPreviewFile", 27u, 58u, 0, m362, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemPreviewVideo", 27u, 59u, 0, m363, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddItemToFavorites", 27u, 70u, 0, m364, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddRequiredKeyValueTag", 27u, 42u, 0, m365, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddRequiredTag", 27u, 22u, 0, m366, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::AddRequiredTagGroup", 27u, 23u, 0, NULL, "tampon IntPtr pTagGroups sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::BInitWorkshopForGameServer", 27u, 80u, 0, m368, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateItem", 27u, 44u, 0, m369, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestCursor", 27u, 2u, 0, m370, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryAllUGCRequestPage", 27u, 1u, 0, m371, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryUGCDetailsRequest", 27u, 3u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::CreateQueryUserUGCRequest", 27u, 0u, 0, m373, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::DeleteItem", 27u, 90u, 0, m374, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::DownloadItem", 27u, 79u, 0, m375, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetAppDependencies", 27u, 89u, 0, m376, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemDownloadInfo", 27u, 78u, 0, m377, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemInstallInfo", 27u, 77u, 0, m378, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemState", 27u, 76u, 0, m379, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetItemUpdateProgress", 27u, 67u, 0, m380, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetNumSubscribedItems", 27u, 74u, 0, m381, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetNumSupportedGameVersions", 27u, 18u, 0, m382, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryFirstUGCKeyValueTag", 27u, 17u, 0, m383, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCChildren", 27u, 11u, 0, m384, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCContentDescriptors", 27u, 20u, 0, m385, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCKeyValueTag", 27u, 16u, 0, m386, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCMetadata", 27u, 10u, 0, m387, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumAdditionalPreviews", 27u, 13u, 0, m388, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumKeyValueTags", 27u, 15u, 0, m389, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCNumTags", 27u, 6u, 0, m390, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCPreviewURL", 27u, 9u, 0, m391, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCResult", 27u, 5u, 0, NULL, "param\u00e8tre out SteamUGCDetails_t pDetails"},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCStatistic", 27u, 12u, 0, m393, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTag", 27u, 7u, 0, m394, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetQueryUGCTagDisplayName", 27u, 8u, 0, m395, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetSubscribedItems", 27u, 75u, 0, m396, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetSupportedGameVersionData", 27u, 19u, 0, NULL, "tampon IntPtr pchGameBranchMin sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::GetUserContentDescriptorPreferences", 27u, 93u, 0, m398, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetUserItemVote", 27u, 69u, 0, m399, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::GetWorkshopEULAStatus", 27u, 92u, 0, m400, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::ReleaseQueryUGCRequest", 27u, 21u, 0, m401, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveAllItemKeyValueTags", 27u, 55u, 0, m402, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveAppDependency", 27u, 88u, 0, m403, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveContentDescriptor", 27u, 64u, 0, m404, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveDependency", 27u, 86u, 0, m405, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveItemFromFavorites", 27u, 71u, 0, m406, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveItemKeyValueTags", 27u, 56u, 0, m407, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RemoveItemPreview", 27u, 62u, 0, m408, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::RequestUGCDetails", 27u, 43u, 0, m409, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SendQueryUGCRequest", 27u, 4u, 0, m410, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetAdminQuery", 27u, 35u, 0, m411, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetAllowCachedResponse", 27u, 34u, 0, m412, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetAllowLegacyUpload", 27u, 54u, 0, m413, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetCloudFileNameFilter", 27u, 36u, 0, m414, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemContent", 27u, 52u, 0, m415, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemDescription", 27u, 47u, 0, m416, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemMetadata", 27u, 49u, 0, m417, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemPreview", 27u, 53u, 0, m418, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemTags", 27u, 51u, 0, NULL, "tampon IntPtr pTags sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemTitle", 27u, 46u, 0, m420, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemUpdateLanguage", 27u, 48u, 0, m421, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetItemVisibility", 27u, 50u, 0, m422, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetLanguage", 27u, 33u, 0, m423, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetMatchAnyTag", 27u, 37u, 0, m424, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetRankedByTrendDays", 27u, 39u, 0, m425, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetRequiredGameVersions", 27u, 65u, 0, m426, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnAdditionalPreviews", 27u, 30u, 0, m427, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnChildren", 27u, 29u, 0, m428, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnKeyValueTags", 27u, 26u, 0, m429, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnLongDescription", 27u, 27u, 0, m430, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnMetadata", 27u, 28u, 0, m431, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnOnlyIDs", 27u, 25u, 0, m432, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnPlaytimeStats", 27u, 32u, 0, m433, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetReturnTotalOnly", 27u, 31u, 0, m434, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetSearchText", 27u, 38u, 0, m435, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetTimeCreatedDateRange", 27u, 40u, 0, m436, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetTimeUpdatedDateRange", 27u, 41u, 0, m437, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SetUserItemVote", 27u, 68u, 0, m438, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::ShowWorkshopEULA", 27u, 91u, 0, m439, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::StartItemUpdate", 27u, 45u, 0, m440, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::StartPlaytimeTracking", 27u, 82u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTracking", 27u, 83u, 0, NULL, "tampon inout PublishedFileId_t[] pvecPublishedFileID sans taille"},
    {"STEAMUGC_INTERFACE_VERSION020::StopPlaytimeTrackingForAllItems", 27u, 84u, 0, m443, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SubmitItemUpdate", 27u, 66u, 0, m444, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SubscribeItem", 27u, 72u, 0, m445, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::SuspendDownloads", 27u, 81u, 0, m446, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::UnsubscribeItem", 27u, 73u, 0, m447, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewFile", 27u, 60u, 0, m448, NULL},
    {"STEAMUGC_INTERFACE_VERSION020::UpdateItemPreviewVideo", 27u, 61u, 0, m449, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::AttachLeaderboardUGC", 7u, 32u, 0, m450, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::ClearAchievement", 7u, 8u, 0, m451, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntries", 7u, 28u, 0, m452, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::DownloadLeaderboardEntriesForUsers", 7u, 29u, 0, m453, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::FindLeaderboard", 7u, 23u, 0, m454, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::FindOrCreateLeaderboard", 7u, 22u, 0, m455, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievement", 7u, 6u, 0, m456, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAchievedPercent", 7u, 37u, 0, m457, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementAndUnlockTime", 7u, 9u, 0, m458, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementDisplayAttribute", 7u, 12u, 0, m459, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementIcon", 7u, 11u, 0, m460, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementName", 7u, 15u, 0, m461, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsFloat", 7u, 44u, 0, m462, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetAchievementProgressLimitsInt32", 7u, 43u, 0, m463, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetDownloadedLeaderboardEntry", 7u, 30u, 0, m464, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatDouble", 7u, 40u, 0, m465, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryDouble", 7u, 42u, 0, m466, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatHistoryInt64", 7u, 41u, 0, m467, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetGlobalStatInt64", 7u, 39u, 0, m468, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardDisplayType", 7u, 27u, 0, m469, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardEntryCount", 7u, 25u, 0, m470, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardName", 7u, 24u, 0, m471, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetLeaderboardSortMethod", 7u, 26u, 0, m472, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetMostAchievedAchievementInfo", 7u, 35u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNextMostAchievedAchievementInfo", 7u, 36u, 0, NULL, "tampon IntPtr pchName sans taille"},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNumAchievements", 7u, 14u, 0, m475, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetNumberOfCurrentPlayers", 7u, 33u, 0, m476, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetStatFloat", 7u, 2u, 0, m477, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetStatInt32", 7u, 1u, 0, m478, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievement", 7u, 19u, 0, m479, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserAchievementAndUnlockTime", 7u, 20u, 0, m480, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatFloat", 7u, 18u, 0, m481, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::GetUserStatInt32", 7u, 17u, 0, m482, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::IndicateAchievementProgress", 7u, 13u, 0, m483, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestCurrentStats", 7u, 0u, 0, m484, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalAchievementPercentages", 7u, 34u, 0, m485, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestGlobalStats", 7u, 38u, 0, m486, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::RequestUserStats", 7u, 16u, 0, m487, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::ResetAllStats", 7u, 21u, 0, m488, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetAchievement", 7u, 7u, 0, m489, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetStatFloat", 7u, 4u, 0, m490, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::SetStatInt32", 7u, 3u, 0, m491, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::StoreStats", 7u, 10u, 0, m492, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::UpdateAvgRateStat", 7u, 5u, 0, m493, NULL},
    {"STEAMUSERSTATS_INTERFACE_VERSION012::UploadLeaderboardScore", 7u, 31u, 0, m494, NULL},
    {"STEAMVIDEO_INTERFACE_V002::GetOPFSettings", 20u, 2u, 0, m495, NULL},
    {"STEAMVIDEO_INTERFACE_V002::GetOPFStringForApp", 20u, 3u, 0, NULL, "tampon IntPtr pchBuffer sans taille"},
    {"STEAMVIDEO_INTERFACE_V002::GetVideoURL", 20u, 0u, 0, m497, NULL},
    {"STEAMVIDEO_INTERFACE_V002::IsBroadcasting", 20u, 1u, 0, m498, NULL},
    {"STEAMVIDEO_INTERFACE_V007::GetOPFSettings", 28u, 2u, 0, m499, NULL},
    {"STEAMVIDEO_INTERFACE_V007::GetOPFStringForApp", 28u, 3u, 0, NULL, "tampon IntPtr pchBuffer sans taille"},
    {"STEAMVIDEO_INTERFACE_V007::GetVideoURL", 28u, 0u, 0, m501, NULL},
    {"STEAMVIDEO_INTERFACE_V007::IsBroadcasting", 28u, 1u, 0, m502, NULL},
    {"SteamClient020::BReleaseSteamPipe", 0u, 1u, 0, m503, NULL},
    {"SteamClient020::BShutdownIfAllPipesClosed", 0u, 23u, 0, m504, NULL},
    {"SteamClient020::ConnectToGlobalUser", 0u, 2u, 0, m505, NULL},
    {"SteamClient020::CreateLocalUser", 0u, 3u, 0, m506, NULL},
    {"SteamClient020::CreateSteamPipe", 0u, 0u, 0, m507, NULL},
    {"SteamClient020::GetIPCCallCount", 0u, 21u, 0, m508, NULL},
    {"SteamClient020::GetISteamAppList", 0u, 28u, 0, m509, NULL},
    {"SteamClient020::GetISteamApps", 0u, 15u, 0, m510, NULL},
    {"SteamClient020::GetISteamController", 0u, 26u, 0, m511, NULL},
    {"SteamClient020::GetISteamFriends", 0u, 8u, 0, m512, NULL},
    {"SteamClient020::GetISteamGameSearch", 0u, 19u, 0, m513, NULL},
    {"SteamClient020::GetISteamGameServer", 0u, 6u, 0, m514, NULL},
    {"SteamClient020::GetISteamGameServerStats", 0u, 14u, 0, m515, NULL},
    {"SteamClient020::GetISteamGenericInterface", 0u, 12u, 0, m516, NULL},
    {"SteamClient020::GetISteamHTMLSurface", 0u, 31u, 0, m517, NULL},
    {"SteamClient020::GetISteamHTTP", 0u, 24u, 0, m518, NULL},
    {"SteamClient020::GetISteamInput", 0u, 38u, 0, m519, NULL},
    {"SteamClient020::GetISteamInventory", 0u, 35u, 0, m520, NULL},
    {"SteamClient020::GetISteamMatchmaking", 0u, 10u, 0, m521, NULL},
    {"SteamClient020::GetISteamMatchmakingServers", 0u, 11u, 0, m522, NULL},
    {"SteamClient020::GetISteamMusic", 0u, 29u, 0, m523, NULL},
    {"SteamClient020::GetISteamMusicRemote", 0u, 30u, 0, m524, NULL},
    {"SteamClient020::GetISteamNetworking", 0u, 16u, 0, m525, NULL},
    {"SteamClient020::GetISteamParentalSettings", 0u, 37u, 0, m526, NULL},
    {"SteamClient020::GetISteamParties", 0u, 39u, 0, m527, NULL},
    {"SteamClient020::GetISteamRemotePlay", 0u, 40u, 0, m528, NULL},
    {"SteamClient020::GetISteamRemoteStorage", 0u, 17u, 0, m529, NULL},
    {"SteamClient020::GetISteamScreenshots", 0u, 18u, 0, m530, NULL},
    {"SteamClient020::GetISteamUGC", 0u, 27u, 0, m531, NULL},
    {"SteamClient020::GetISteamUser", 0u, 5u, 0, m532, NULL},
    {"SteamClient020::GetISteamUserStats", 0u, 13u, 0, m533, NULL},
    {"SteamClient020::GetISteamUtils", 0u, 9u, 0, m534, NULL},
    {"SteamClient020::GetISteamVideo", 0u, 36u, 0, m535, NULL},
    {"SteamClient020::ReleaseUser", 0u, 4u, 0, m536, NULL},
    {"SteamClient020::SetLocalIPBinding", 0u, 7u, 0, NULL, "param\u00e8tre ref SteamIPAddress_t unIP"},
    {"SteamClient020::SetWarningMessageHook", 0u, 22u, 0, steam_manual_client_warning_hook, NULL},
    {"SteamClient021::BReleaseSteamPipe", 25u, 1u, 0, m539, NULL},
    {"SteamClient021::BShutdownIfAllPipesClosed", 25u, 23u, 0, m540, NULL},
    {"SteamClient021::ConnectToGlobalUser", 25u, 2u, 0, m541, NULL},
    {"SteamClient021::CreateLocalUser", 25u, 3u, 0, m542, NULL},
    {"SteamClient021::CreateSteamPipe", 25u, 0u, 0, m543, NULL},
    {"SteamClient021::GetIPCCallCount", 25u, 21u, 0, m544, NULL},
    {"SteamClient021::GetISteamApps", 25u, 15u, 0, m545, NULL},
    {"SteamClient021::GetISteamController", 25u, 25u, 0, m546, NULL},
    {"SteamClient021::GetISteamFriends", 25u, 8u, 0, m547, NULL},
    {"SteamClient021::GetISteamGameSearch", 25u, 19u, 0, m548, NULL},
    {"SteamClient021::GetISteamGameServer", 25u, 6u, 0, m549, NULL},
    {"SteamClient021::GetISteamGameServerStats", 25u, 14u, 0, m550, NULL},
    {"SteamClient021::GetISteamGenericInterface", 25u, 12u, 0, m551, NULL},
    {"SteamClient021::GetISteamHTMLSurface", 25u, 29u, 0, m552, NULL},
    {"SteamClient021::GetISteamHTTP", 25u, 24u, 0, m553, NULL},
    {"SteamClient021::GetISteamInput", 25u, 36u, 0, m554, NULL},
    {"SteamClient021::GetISteamInventory", 25u, 33u, 0, m555, NULL},
    {"SteamClient021::GetISteamMatchmaking", 25u, 10u, 0, m556, NULL},
    {"SteamClient021::GetISteamMatchmakingServers", 25u, 11u, 0, m557, NULL},
    {"SteamClient021::GetISteamMusic", 25u, 27u, 0, m558, NULL},
    {"SteamClient021::GetISteamMusicRemote", 25u, 28u, 0, m559, NULL},
    {"SteamClient021::GetISteamNetworking", 25u, 16u, 0, m560, NULL},
    {"SteamClient021::GetISteamParentalSettings", 25u, 35u, 0, m561, NULL},
    {"SteamClient021::GetISteamParties", 25u, 37u, 0, m562, NULL},
    {"SteamClient021::GetISteamRemotePlay", 25u, 38u, 0, m563, NULL},
    {"SteamClient021::GetISteamRemoteStorage", 25u, 17u, 0, m564, NULL},
    {"SteamClient021::GetISteamScreenshots", 25u, 18u, 0, m565, NULL},
    {"SteamClient021::GetISteamUGC", 25u, 26u, 0, m566, NULL},
    {"SteamClient021::GetISteamUser", 25u, 5u, 0, m567, NULL},
    {"SteamClient021::GetISteamUserStats", 25u, 13u, 0, m568, NULL},
    {"SteamClient021::GetISteamUtils", 25u, 9u, 0, m569, NULL},
    {"SteamClient021::GetISteamVideo", 25u, 34u, 0, m570, NULL},
    {"SteamClient021::ReleaseUser", 25u, 4u, 0, m571, NULL},
    {"SteamClient021::SetLocalIPBinding", 25u, 7u, 0, NULL, "param\u00e8tre ref SteamIPAddress_t unIP"},
    {"SteamClient021::SetWarningMessageHook", 25u, 22u, 0, steam_manual_client_warning_hook, NULL},
    {"SteamController008::ActivateActionSet", 13u, 5u, 0, m574, NULL},
    {"SteamController008::ActivateActionSetLayer", 13u, 7u, 0, m575, NULL},
    {"SteamController008::DeactivateActionSetLayer", 13u, 8u, 0, m576, NULL},
    {"SteamController008::DeactivateAllActionSetLayers", 13u, 9u, 0, m577, NULL},
    {"SteamController008::GetActionOriginFromXboxOrigin", 13u, 31u, 0, m578, NULL},
    {"SteamController008::GetActionSetHandle", 13u, 4u, 0, m579, NULL},
    {"SteamController008::GetActiveActionSetLayers", 13u, 10u, 0, m580, NULL},
    {"SteamController008::GetAnalogActionHandle", 13u, 14u, 0, m581, NULL},
    {"SteamController008::GetAnalogActionOrigins", 13u, 16u, 0, m582, NULL},
    {"SteamController008::GetConnectedControllers", 13u, 3u, 0, m583, NULL},
    {"SteamController008::GetControllerBindingRevision", 13u, 33u, 0, m584, NULL},
    {"SteamController008::GetControllerForGamepadIndex", 13u, 27u, 0, m585, NULL},
    {"SteamController008::GetCurrentActionSet", 13u, 6u, 0, m586, NULL},
    {"SteamController008::GetDigitalActionHandle", 13u, 11u, 0, m587, NULL},
    {"SteamController008::GetDigitalActionOrigins", 13u, 13u, 0, m588, NULL},
    {"SteamController008::GetGamepadIndexForController", 13u, 28u, 0, m589, NULL},
    {"SteamController008::GetGlyphForActionOrigin", 13u, 17u, 0, m590, NULL},
    {"SteamController008::GetGlyphForXboxOrigin", 13u, 30u, 0, m591, NULL},
    {"SteamController008::GetInputTypeForHandle", 13u, 26u, 0, m592, NULL},
    {"SteamController008::GetStringForActionOrigin", 13u, 18u, 0, m593, NULL},
    {"SteamController008::GetStringForXboxOrigin", 13u, 29u, 0, m594, NULL},
    {"SteamController008::Init", 13u, 0u, 0, m595, NULL},
    {"SteamController008::RunFrame", 13u, 2u, 0, m596, NULL},
    {"SteamController008::SetLEDColor", 13u, 24u, 0, m597, NULL},
    {"SteamController008::ShowBindingPanel", 13u, 25u, 0, m598, NULL},
    {"SteamController008::Shutdown", 13u, 1u, 0, m599, NULL},
    {"SteamController008::StopAnalogActionMomentum", 13u, 19u, 0, m600, NULL},
    {"SteamController008::TranslateActionOrigin", 13u, 32u, 0, m601, NULL},
    {"SteamController008::TriggerHapticPulse", 13u, 21u, 0, m602, NULL},
    {"SteamController008::TriggerRepeatedHapticPulse", 13u, 22u, 0, m603, NULL},
    {"SteamController008::TriggerVibration", 13u, 23u, 0, m604, NULL},
    {"SteamFriends017::ActivateGameOverlay", 2u, 28u, 0, m605, NULL},
    {"SteamFriends017::ActivateGameOverlayInviteDialog", 2u, 33u, 0, m606, NULL},
    {"SteamFriends017::ActivateGameOverlayInviteDialogConnectString", 2u, 75u, 0, m607, NULL},
    {"SteamFriends017::ActivateGameOverlayRemotePlayTogetherInviteDialog", 2u, 73u, 0, m608, NULL},
    {"SteamFriends017::ActivateGameOverlayToStore", 2u, 31u, 0, m609, NULL},
    {"SteamFriends017::ActivateGameOverlayToUser", 2u, 29u, 0, m610, NULL},
    {"SteamFriends017::ActivateGameOverlayToWebPage", 2u, 30u, 0, m611, NULL},
    {"SteamFriends017::ClearRichPresence", 2u, 44u, 0, m612, NULL},
    {"SteamFriends017::CloseClanChatWindowInSteam", 2u, 63u, 0, m613, NULL},
    {"SteamFriends017::DownloadClanActivityCounts", 2u, 23u, 0, m614, NULL},
    {"SteamFriends017::EnumerateFollowingList", 2u, 69u, 0, m615, NULL},
    {"SteamFriends017::GetChatMemberByIndex", 2u, 57u, 1, m616, NULL},
    {"SteamFriends017::GetClanActivityCounts", 2u, 22u, 0, m617, NULL},
    {"SteamFriends017::GetClanByIndex", 2u, 19u, 1, m618, NULL},
    {"SteamFriends017::GetClanChatMemberCount", 2u, 56u, 0, m619, NULL},
    {"SteamFriends017::GetClanChatMessage", 2u, 59u, 0, m620, NULL},
    {"SteamFriends017::GetClanCount", 2u, 18u, 0, m621, NULL},
    {"SteamFriends017::GetClanName", 2u, 20u, 0, m622, NULL},
    {"SteamFriends017::GetClanOfficerByIndex", 2u, 41u, 1, m623, NULL},
    {"SteamFriends017::GetClanOfficerCount", 2u, 40u, 0, m624, NULL},
    {"SteamFriends017::GetClanOwner", 2u, 39u, 1, m625, NULL},
    {"SteamFriends017::GetClanTag", 2u, 21u, 0, m626, NULL},
    {"SteamFriends017::GetCoplayFriend", 2u, 51u, 1, m627, NULL},
    {"SteamFriends017::GetCoplayFriendCount", 2u, 50u, 0, m628, NULL},
    {"SteamFriends017::GetFollowerCount", 2u, 67u, 0, m629, NULL},
    {"SteamFriends017::GetFriendByIndex", 2u, 4u, 1, m630, NULL},
    {"SteamFriends017::GetFriendCoplayGame", 2u, 53u, 0, m631, NULL},
    {"SteamFriends017::GetFriendCoplayTime", 2u, 52u, 0, m632, NULL},
    {"SteamFriends017::GetFriendCount", 2u, 3u, 0, m633, NULL},
    {"SteamFriends017::GetFriendCountFromSource", 2u, 24u, 0, m634, NULL},
    {"SteamFriends017::GetFriendFromSourceByIndex", 2u, 25u, 1, m635, NULL},
    {"SteamFriends017::GetFriendGamePlayed", 2u, 8u, 0, m636, NULL},
    {"SteamFriends017::GetFriendMessage", 2u, 66u, 0, m637, NULL},
    {"SteamFriends017::GetFriendPersonaName", 2u, 7u, 0, m638, NULL},
    {"SteamFriends017::GetFriendPersonaNameHistory", 2u, 9u, 0, m639, NULL},
    {"SteamFriends017::GetFriendPersonaState", 2u, 6u, 0, m640, NULL},
    {"SteamFriends017::GetFriendRelationship", 2u, 5u, 0, m641, NULL},
    {"SteamFriends017::GetFriendRichPresence", 2u, 45u, 0, m642, NULL},
    {"SteamFriends017::GetFriendRichPresenceKeyByIndex", 2u, 47u, 0, m643, NULL},
    {"SteamFriends017::GetFriendRichPresenceKeyCount", 2u, 46u, 0, m644, NULL},
    {"SteamFriends017::GetFriendSteamLevel", 2u, 10u, 0, m645, NULL},
    {"SteamFriends017::GetFriendsGroupCount", 2u, 12u, 0, m646, NULL},
    {"SteamFriends017::GetFriendsGroupIDByIndex", 2u, 13u, 0, m647, NULL},
    {"SteamFriends017::GetFriendsGroupMembersCount", 2u, 15u, 0, m648, NULL},
    {"SteamFriends017::GetFriendsGroupMembersList", 2u, 16u, 0, m649, NULL},
    {"SteamFriends017::GetFriendsGroupName", 2u, 14u, 0, m650, NULL},
    {"SteamFriends017::GetLargeFriendAvatar", 2u, 36u, 0, m651, NULL},
    {"SteamFriends017::GetMediumFriendAvatar", 2u, 35u, 0, m652, NULL},
    {"SteamFriends017::GetNumChatsWithUnreadPriorityMessages", 2u, 72u, 0, m653, NULL},
    {"SteamFriends017::GetPersonaName", 2u, 0u, 0, m654, NULL},
    {"SteamFriends017::GetPersonaState", 2u, 2u, 0, m655, NULL},
    {"SteamFriends017::GetPlayerNickname", 2u, 11u, 0, m656, NULL},
    {"SteamFriends017::GetSmallFriendAvatar", 2u, 34u, 0, m657, NULL},
    {"SteamFriends017::GetUserRestrictions", 2u, 42u, 0, m658, NULL},
    {"SteamFriends017::HasFriend", 2u, 17u, 0, m659, NULL},
    {"SteamFriends017::InviteUserToGame", 2u, 49u, 0, m660, NULL},
    {"SteamFriends017::IsClanChatAdmin", 2u, 60u, 0, m661, NULL},
    {"SteamFriends017::IsClanChatWindowOpenInSteam", 2u, 61u, 0, m662, NULL},
    {"SteamFriends017::IsClanOfficialGameGroup", 2u, 71u, 0, m663, NULL},
    {"SteamFriends017::IsClanPublic", 2u, 70u, 0, m664, NULL},
    {"SteamFriends017::IsFollowing", 2u, 68u, 0, m665, NULL},
    {"SteamFriends017::IsUserInSource", 2u, 26u, 0, m666, NULL},
    {"SteamFriends017::JoinClanChatRoom", 2u, 54u, 0, m667, NULL},
    {"SteamFriends017::LeaveClanChatRoom", 2u, 55u, 0, m668, NULL},
    {"SteamFriends017::OpenClanChatWindowInSteam", 2u, 62u, 0, m669, NULL},
    {"SteamFriends017::RegisterProtocolInOverlayBrowser", 2u, 74u, 0, m670, NULL},
    {"SteamFriends017::ReplyToFriendMessage", 2u, 65u, 0, m671, NULL},
    {"SteamFriends017::RequestClanOfficerList", 2u, 38u, 0, m672, NULL},
    {"SteamFriends017::RequestFriendRichPresence", 2u, 48u, 0, m673, NULL},
    {"SteamFriends017::RequestUserInformation", 2u, 37u, 0, m674, NULL},
    {"SteamFriends017::SendClanChatMessage", 2u, 58u, 0, m675, NULL},
    {"SteamFriends017::SetInGameVoiceSpeaking", 2u, 27u, 0, m676, NULL},
    {"SteamFriends017::SetListenForFriendsMessages", 2u, 64u, 0, m677, NULL},
    {"SteamFriends017::SetPersonaName", 2u, 1u, 0, m678, NULL},
    {"SteamFriends017::SetPlayedWith", 2u, 32u, 0, m679, NULL},
    {"SteamFriends017::SetRichPresence", 2u, 43u, 0, m680, NULL},
    {"SteamGameServer014::AssociateWithClan", 23u, 36u, 0, m681, NULL},
    {"SteamGameServer014::BLoggedOn", 23u, 8u, 0, m682, NULL},
    {"SteamGameServer014::BSecure", 23u, 9u, 0, m683, NULL},
    {"SteamGameServer014::BUpdateUserData", 23u, 41u, 0, m684, NULL},
    {"SteamGameServer014::BeginAuthSession", 23u, 26u, 0, m685, NULL},
    {"SteamGameServer014::CancelAuthTicket", 23u, 28u, 0, m686, NULL},
    {"SteamGameServer014::ClearAllKeyValues", 23u, 19u, 0, m687, NULL},
    {"SteamGameServer014::ComputeNewPlayerCompatibility", 23u, 37u, 0, m688, NULL},
    {"SteamGameServer014::CreateUnauthenticatedUserConnection", 23u, 39u, 1, m689, NULL},
    {"SteamGameServer014::EndAuthSession", 23u, 27u, 0, m690, NULL},
    {"SteamGameServer014::GetAuthSessionTicket", 23u, 25u, 0, m691, NULL},
    {"SteamGameServer014::GetGameplayStats", 23u, 31u, 0, m692, NULL},
    {"SteamGameServer014::GetNextOutgoingPacket", 23u, 35u, 0, m693, NULL},
    {"SteamGameServer014::GetServerReputation", 23u, 32u, 0, m694, NULL},
    {"SteamGameServer014::GetSteamID", 23u, 10u, 1, m695, NULL},
    {"SteamGameServer014::HandleIncomingPacket", 23u, 34u, 0, m696, NULL},
    {"SteamGameServer014::LogOff", 23u, 7u, 0, m697, NULL},
    {"SteamGameServer014::LogOn", 23u, 5u, 0, m698, NULL},
    {"SteamGameServer014::LogOnAnonymous", 23u, 6u, 0, m699, NULL},
    {"SteamGameServer014::RequestUserGroupStatus", 23u, 30u, 0, m700, NULL},
    {"SteamGameServer014::SendUserConnectAndAuthenticate_DEPRECATED", 23u, 38u, 0, m701, NULL},
    {"SteamGameServer014::SendUserDisconnect_DEPRECATED", 23u, 40u, 0, m702, NULL},
    {"SteamGameServer014::SetAdvertiseServerActive", 23u, 24u, 0, m703, NULL},
    {"SteamGameServer014::SetBotPlayerCount", 23u, 13u, 0, m704, NULL},
    {"SteamGameServer014::SetDedicatedServer", 23u, 4u, 0, m705, NULL},
    {"SteamGameServer014::SetGameData", 23u, 22u, 0, m706, NULL},
    {"SteamGameServer014::SetGameDescription", 23u, 2u, 0, m707, NULL},
    {"SteamGameServer014::SetGameTags", 23u, 21u, 0, m708, NULL},
    {"SteamGameServer014::SetKeyValue", 23u, 20u, 0, m709, NULL},
    {"SteamGameServer014::SetMapName", 23u, 15u, 0, m710, NULL},
    {"SteamGameServer014::SetMaxPlayerCount", 23u, 12u, 0, m711, NULL},
    {"SteamGameServer014::SetModDir", 23u, 3u, 0, m712, NULL},
    {"SteamGameServer014::SetPasswordProtected", 23u, 16u, 0, m713, NULL},
    {"SteamGameServer014::SetProduct", 23u, 1u, 0, m714, NULL},
    {"SteamGameServer014::SetRegion", 23u, 23u, 0, m715, NULL},
    {"SteamGameServer014::SetServerName", 23u, 14u, 0, m716, NULL},
    {"SteamGameServer014::SetSpectatorPort", 23u, 17u, 0, m717, NULL},
    {"SteamGameServer014::SetSpectatorServerName", 23u, 18u, 0, m718, NULL},
    {"SteamGameServer014::UserHasLicenseForApp", 23u, 29u, 0, m719, NULL},
    {"SteamGameServer014::WasRestartRequested", 23u, 11u, 0, m720, NULL},
    {"SteamGameServer015::AssociateWithClan", 29u, 36u, 0, m721, NULL},
    {"SteamGameServer015::BLoggedOn", 29u, 8u, 0, m722, NULL},
    {"SteamGameServer015::BSecure", 29u, 9u, 0, m723, NULL},
    {"SteamGameServer015::BUpdateUserData", 29u, 41u, 0, m724, NULL},
    {"SteamGameServer015::BeginAuthSession", 29u, 26u, 0, m725, NULL},
    {"SteamGameServer015::CancelAuthTicket", 29u, 28u, 0, m726, NULL},
    {"SteamGameServer015::ClearAllKeyValues", 29u, 19u, 0, m727, NULL},
    {"SteamGameServer015::ComputeNewPlayerCompatibility", 29u, 37u, 0, m728, NULL},
    {"SteamGameServer015::CreateUnauthenticatedUserConnection", 29u, 39u, 1, m729, NULL},
    {"SteamGameServer015::EndAuthSession", 29u, 27u, 0, m730, NULL},
    {"SteamGameServer015::GetAuthSessionTicket", 29u, 25u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pSnid"},
    {"SteamGameServer015::GetGameplayStats", 29u, 31u, 0, m732, NULL},
    {"SteamGameServer015::GetNextOutgoingPacket", 29u, 35u, 0, m733, NULL},
    {"SteamGameServer015::GetServerReputation", 29u, 32u, 0, m734, NULL},
    {"SteamGameServer015::GetSteamID", 29u, 10u, 1, m735, NULL},
    {"SteamGameServer015::HandleIncomingPacket", 29u, 34u, 0, m736, NULL},
    {"SteamGameServer015::LogOff", 29u, 7u, 0, m737, NULL},
    {"SteamGameServer015::LogOn", 29u, 5u, 0, m738, NULL},
    {"SteamGameServer015::LogOnAnonymous", 29u, 6u, 0, m739, NULL},
    {"SteamGameServer015::RequestUserGroupStatus", 29u, 30u, 0, m740, NULL},
    {"SteamGameServer015::SendUserConnectAndAuthenticate_DEPRECATED", 29u, 38u, 0, m741, NULL},
    {"SteamGameServer015::SendUserDisconnect_DEPRECATED", 29u, 40u, 0, m742, NULL},
    {"SteamGameServer015::SetAdvertiseServerActive", 29u, 24u, 0, m743, NULL},
    {"SteamGameServer015::SetBotPlayerCount", 29u, 13u, 0, m744, NULL},
    {"SteamGameServer015::SetDedicatedServer", 29u, 4u, 0, m745, NULL},
    {"SteamGameServer015::SetGameData", 29u, 22u, 0, m746, NULL},
    {"SteamGameServer015::SetGameDescription", 29u, 2u, 0, m747, NULL},
    {"SteamGameServer015::SetGameTags", 29u, 21u, 0, m748, NULL},
    {"SteamGameServer015::SetKeyValue", 29u, 20u, 0, m749, NULL},
    {"SteamGameServer015::SetMapName", 29u, 15u, 0, m750, NULL},
    {"SteamGameServer015::SetMaxPlayerCount", 29u, 12u, 0, m751, NULL},
    {"SteamGameServer015::SetModDir", 29u, 3u, 0, m752, NULL},
    {"SteamGameServer015::SetPasswordProtected", 29u, 16u, 0, m753, NULL},
    {"SteamGameServer015::SetProduct", 29u, 1u, 0, m754, NULL},
    {"SteamGameServer015::SetRegion", 29u, 23u, 0, m755, NULL},
    {"SteamGameServer015::SetServerName", 29u, 14u, 0, m756, NULL},
    {"SteamGameServer015::SetSpectatorPort", 29u, 17u, 0, m757, NULL},
    {"SteamGameServer015::SetSpectatorServerName", 29u, 18u, 0, m758, NULL},
    {"SteamGameServer015::UserHasLicenseForApp", 29u, 29u, 0, m759, NULL},
    {"SteamGameServer015::WasRestartRequested", 29u, 11u, 0, m760, NULL},
    {"SteamGameServerStats001::ClearUserAchievement", 24u, 8u, 0, m761, NULL},
    {"SteamGameServerStats001::GetUserAchievement", 24u, 3u, 0, m762, NULL},
    {"SteamGameServerStats001::GetUserStatFloat", 24u, 2u, 0, m763, NULL},
    {"SteamGameServerStats001::GetUserStatInt32", 24u, 1u, 0, m764, NULL},
    {"SteamGameServerStats001::RequestUserStats", 24u, 0u, 0, m765, NULL},
    {"SteamGameServerStats001::SetUserAchievement", 24u, 7u, 0, m766, NULL},
    {"SteamGameServerStats001::SetUserStatFloat", 24u, 5u, 0, m767, NULL},
    {"SteamGameServerStats001::SetUserStatInt32", 24u, 4u, 0, m768, NULL},
    {"SteamGameServerStats001::StoreUserStats", 24u, 9u, 0, m769, NULL},
    {"SteamGameServerStats001::UpdateUserAvgRateStat", 24u, 6u, 0, m770, NULL},
    {"SteamInput006::ActivateActionSet", 22u, 10u, 0, m771, NULL},
    {"SteamInput006::ActivateActionSetLayer", 22u, 12u, 0, m772, NULL},
    {"SteamInput006::BNewDataAvailable", 22u, 5u, 0, m773, NULL},
    {"SteamInput006::BWaitForData", 22u, 4u, 0, m774, NULL},
    {"SteamInput006::DeactivateActionSetLayer", 22u, 13u, 0, m775, NULL},
    {"SteamInput006::DeactivateAllActionSetLayers", 22u, 14u, 0, m776, NULL},
    {"SteamInput006::EnableActionEventCallbacks", 22u, 8u, 0, NULL, "param\u00e8tre SteamInputActionEventCallbackPointer pCallback"},
    {"SteamInput006::EnableDeviceCallbacks", 22u, 7u, 0, m778, NULL},
    {"SteamInput006::GetActionOriginFromXboxOrigin", 22u, 42u, 0, m779, NULL},
    {"SteamInput006::GetActionSetHandle", 22u, 9u, 0, m780, NULL},
    {"SteamInput006::GetActiveActionSetLayers", 22u, 15u, 0, m781, NULL},
    {"SteamInput006::GetAnalogActionHandle", 22u, 20u, 0, m782, NULL},
    {"SteamInput006::GetAnalogActionOrigins", 22u, 22u, 0, m783, NULL},
    {"SteamInput006::GetConnectedControllers", 22u, 6u, 0, m784, NULL},
    {"SteamInput006::GetControllerForGamepadIndex", 22u, 38u, 0, m785, NULL},
    {"SteamInput006::GetCurrentActionSet", 22u, 11u, 0, m786, NULL},
    {"SteamInput006::GetDeviceBindingRevision", 22u, 44u, 0, m787, NULL},
    {"SteamInput006::GetDigitalActionHandle", 22u, 16u, 0, m788, NULL},
    {"SteamInput006::GetDigitalActionOrigins", 22u, 18u, 0, m789, NULL},
    {"SteamInput006::GetGamepadIndexForController", 22u, 39u, 0, m790, NULL},
    {"SteamInput006::GetGlyphForActionOrigin_Legacy", 22u, 25u, 0, m791, NULL},
    {"SteamInput006::GetGlyphForXboxOrigin", 22u, 41u, 0, m792, NULL},
    {"SteamInput006::GetGlyphPNGForActionOrigin", 22u, 23u, 0, m793, NULL},
    {"SteamInput006::GetGlyphSVGForActionOrigin", 22u, 24u, 0, m794, NULL},
    {"SteamInput006::GetInputTypeForHandle", 22u, 37u, 0, m795, NULL},
    {"SteamInput006::GetRemotePlaySessionID", 22u, 45u, 0, m796, NULL},
    {"SteamInput006::GetSessionInputConfigurationSettings", 22u, 46u, 0, m797, NULL},
    {"SteamInput006::GetStringForActionOrigin", 22u, 26u, 0, m798, NULL},
    {"SteamInput006::GetStringForAnalogActionName", 22u, 27u, 0, m799, NULL},
    {"SteamInput006::GetStringForDigitalActionName", 22u, 19u, 0, m800, NULL},
    {"SteamInput006::GetStringForXboxOrigin", 22u, 40u, 0, m801, NULL},
    {"SteamInput006::Init", 22u, 0u, 0, m802, NULL},
    {"SteamInput006::Legacy_TriggerHapticPulse", 22u, 34u, 0, m803, NULL},
    {"SteamInput006::Legacy_TriggerRepeatedHapticPulse", 22u, 35u, 0, m804, NULL},
    {"SteamInput006::RunFrame", 22u, 3u, 0, m805, NULL},
    {"SteamInput006::SetInputActionManifestFilePath", 22u, 2u, 0, m806, NULL},
    {"SteamInput006::SetLEDColor", 22u, 33u, 0, m807, NULL},
    {"SteamInput006::ShowBindingPanel", 22u, 36u, 0, m808, NULL},
    {"SteamInput006::Shutdown", 22u, 1u, 0, m809, NULL},
    {"SteamInput006::StopAnalogActionMomentum", 22u, 28u, 0, m810, NULL},
    {"SteamInput006::TranslateActionOrigin", 22u, 43u, 0, m811, NULL},
    {"SteamInput006::TriggerSimpleHapticEvent", 22u, 32u, 0, NULL, "param\u00e8tre char nGainDB"},
    {"SteamInput006::TriggerVibration", 22u, 30u, 0, m813, NULL},
    {"SteamInput006::TriggerVibrationExtended", 22u, 31u, 0, m814, NULL},
    {"SteamMatchGameSearch001::AcceptGame", 5u, 3u, 0, m815, NULL},
    {"SteamMatchGameSearch001::AddGameSearchParams", 5u, 0u, 0, m816, NULL},
    {"SteamMatchGameSearch001::CancelRequestPlayersForGame", 5u, 11u, 0, m817, NULL},
    {"SteamMatchGameSearch001::DeclineGame", 5u, 4u, 0, m818, NULL},
    {"SteamMatchGameSearch001::EndGame", 5u, 13u, 0, m819, NULL},
    {"SteamMatchGameSearch001::EndGameSearch", 5u, 6u, 0, m820, NULL},
    {"SteamMatchGameSearch001::HostConfirmGameStart", 5u, 10u, 0, m821, NULL},
    {"SteamMatchGameSearch001::RequestPlayersForGame", 5u, 9u, 0, m822, NULL},
    {"SteamMatchGameSearch001::RetrieveConnectionDetails", 5u, 5u, 0, m823, NULL},
    {"SteamMatchGameSearch001::SearchForGameSolo", 5u, 2u, 0, m824, NULL},
    {"SteamMatchGameSearch001::SearchForGameWithLobby", 5u, 1u, 0, m825, NULL},
    {"SteamMatchGameSearch001::SetConnectionDetails", 5u, 8u, 0, m826, NULL},
    {"SteamMatchGameSearch001::SetGameHostParams", 5u, 7u, 0, m827, NULL},
    {"SteamMatchGameSearch001::SubmitPlayerResult", 5u, 12u, 0, m828, NULL},
    {"SteamMatchMaking009::AddFavoriteGame", 4u, 2u, 0, m829, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListCompatibleMembersFilter", 4u, 11u, 0, m830, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListDistanceFilter", 4u, 9u, 0, m831, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListFilterSlotsAvailable", 4u, 8u, 0, m832, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListNearValueFilter", 4u, 7u, 0, m833, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListNumericalFilter", 4u, 6u, 0, m834, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListResultCountFilter", 4u, 10u, 0, m835, NULL},
    {"SteamMatchMaking009::AddRequestLobbyListStringFilter", 4u, 5u, 0, m836, NULL},
    {"SteamMatchMaking009::CreateLobby", 4u, 13u, 0, m837, NULL},
    {"SteamMatchMaking009::DeleteLobbyData", 4u, 23u, 0, m838, NULL},
    {"SteamMatchMaking009::GetFavoriteGame", 4u, 1u, 0, m839, NULL},
    {"SteamMatchMaking009::GetFavoriteGameCount", 4u, 0u, 0, m840, NULL},
    {"SteamMatchMaking009::GetLobbyByIndex", 4u, 12u, 1, m841, NULL},
    {"SteamMatchMaking009::GetLobbyChatEntry", 4u, 27u, 0, m842, NULL},
    {"SteamMatchMaking009::GetLobbyData", 4u, 19u, 0, m843, NULL},
    {"SteamMatchMaking009::GetLobbyDataByIndex", 4u, 22u, 0, m844, NULL},
    {"SteamMatchMaking009::GetLobbyDataCount", 4u, 21u, 0, m845, NULL},
    {"SteamMatchMaking009::GetLobbyGameServer", 4u, 30u, 0, m846, NULL},
    {"SteamMatchMaking009::GetLobbyMemberByIndex", 4u, 18u, 1, m847, NULL},
    {"SteamMatchMaking009::GetLobbyMemberData", 4u, 24u, 0, m848, NULL},
    {"SteamMatchMaking009::GetLobbyMemberLimit", 4u, 32u, 0, m849, NULL},
    {"SteamMatchMaking009::GetLobbyOwner", 4u, 35u, 1, m850, NULL},
    {"SteamMatchMaking009::GetNumLobbyMembers", 4u, 17u, 0, m851, NULL},
    {"SteamMatchMaking009::InviteUserToLobby", 4u, 16u, 0, m852, NULL},
    {"SteamMatchMaking009::JoinLobby", 4u, 14u, 0, m853, NULL},
    {"SteamMatchMaking009::LeaveLobby", 4u, 15u, 0, m854, NULL},
    {"SteamMatchMaking009::RemoveFavoriteGame", 4u, 3u, 0, m855, NULL},
    {"SteamMatchMaking009::RequestLobbyData", 4u, 28u, 0, m856, NULL},
    {"SteamMatchMaking009::RequestLobbyList", 4u, 4u, 0, m857, NULL},
    {"SteamMatchMaking009::SendLobbyChatMsg", 4u, 26u, 0, m858, NULL},
    {"SteamMatchMaking009::SetLinkedLobby", 4u, 37u, 0, m859, NULL},
    {"SteamMatchMaking009::SetLobbyData", 4u, 20u, 0, m860, NULL},
    {"SteamMatchMaking009::SetLobbyGameServer", 4u, 29u, 0, m861, NULL},
    {"SteamMatchMaking009::SetLobbyJoinable", 4u, 34u, 0, m862, NULL},
    {"SteamMatchMaking009::SetLobbyMemberData", 4u, 25u, 0, m863, NULL},
    {"SteamMatchMaking009::SetLobbyMemberLimit", 4u, 31u, 0, m864, NULL},
    {"SteamMatchMaking009::SetLobbyOwner", 4u, 36u, 0, m865, NULL},
    {"SteamMatchMaking009::SetLobbyType", 4u, 33u, 0, m866, NULL},
    {"SteamMatchMakingServers002::CancelQuery", 6u, 8u, 0, NULL, "param\u00e8tre HServerListRequest hRequest"},
    {"SteamMatchMakingServers002::CancelServerQuery", 6u, 16u, 0, m868, NULL},
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
    {"SteamNetworking006::AcceptP2PSessionWithUser", 9u, 3u, 0, m884, NULL},
    {"SteamNetworking006::AllowP2PPacketRelay", 9u, 7u, 0, m885, NULL},
    {"SteamNetworking006::CloseP2PChannelWithUser", 9u, 5u, 0, m886, NULL},
    {"SteamNetworking006::CloseP2PSessionWithUser", 9u, 4u, 0, m887, NULL},
    {"SteamNetworking006::CreateP2PConnectionSocket", 9u, 9u, 0, m888, NULL},
    {"SteamNetworking006::DestroyListenSocket", 9u, 12u, 0, m889, NULL},
    {"SteamNetworking006::DestroySocket", 9u, 11u, 0, m890, NULL},
    {"SteamNetworking006::GetListenSocketInfo", 9u, 19u, 0, NULL, "param\u00e8tre out SteamIPAddress_t pnIP"},
    {"SteamNetworking006::GetMaxPacketSize", 9u, 21u, 0, m892, NULL},
    {"SteamNetworking006::GetP2PSessionState", 9u, 6u, 0, NULL, "param\u00e8tre out P2PSessionState_t pConnectionState"},
    {"SteamNetworking006::GetSocketConnectionType", 9u, 20u, 0, m894, NULL},
    {"SteamNetworking006::GetSocketInfo", 9u, 18u, 0, NULL, "param\u00e8tre out SteamIPAddress_t punIPRemote"},
    {"SteamNetworking006::IsDataAvailable", 9u, 16u, 0, m896, NULL},
    {"SteamNetworking006::IsDataAvailableOnSocket", 9u, 14u, 0, m897, NULL},
    {"SteamNetworking006::IsP2PPacketAvailable", 9u, 1u, 0, m898, NULL},
    {"SteamNetworking006::ReadP2PPacket", 9u, 2u, 0, m899, NULL},
    {"SteamNetworking006::RetrieveData", 9u, 17u, 0, m900, NULL},
    {"SteamNetworking006::RetrieveDataFromSocket", 9u, 15u, 0, m901, NULL},
    {"SteamNetworking006::SendDataOnSocket", 9u, 13u, 0, m902, NULL},
    {"SteamNetworking006::SendP2PPacket", 9u, 0u, 0, m903, NULL},
    {"SteamNetworkingMessages002::AcceptSessionWithUser", 34u, 2u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::CloseChannelWithUser", 34u, 4u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::CloseSessionWithUser", 34u, 3u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::GetSessionConnectionInfo", 34u, 5u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingMessages002::ReceiveMessagesOnChannel", 34u, 1u, 0, NULL, "tableau inout IntPtr[] ppOutMessages"},
    {"SteamNetworkingMessages002::SendMessageToUser", 34u, 0u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingSockets012::AcceptConnection", 33u, 4u, 0, m910, NULL},
    {"SteamNetworkingSockets012::BeginAsyncRequestFakeIP", 33u, 42u, 0, m911, NULL},
    {"SteamNetworkingSockets012::CloseConnection", 33u, 5u, 0, m912, NULL},
    {"SteamNetworkingSockets012::CloseListenSocket", 33u, 6u, 0, m913, NULL},
    {"SteamNetworkingSockets012::ConfigureConnectionLanes", 33u, 20u, 0, NULL, "tampon inout int[] pLanePriorities sans taille"},
    {"SteamNetworkingSockets012::ConnectByIPAddress", 33u, 1u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::ConnectP2P", 33u, 3u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityRemote"},
    {"SteamNetworkingSockets012::ConnectP2PCustomSignaling", 33u, 36u, 0, NULL, "param\u00e8tre out ISteamNetworkingConnectionSignaling pSignaling"},
    {"SteamNetworkingSockets012::ConnectToHostedDedicatedServer", 33u, 30u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityTarget"},
    {"SteamNetworkingSockets012::CreateFakeUDPPort", 33u, 46u, 0, NULL, "retour IntPtr (IntPtr)"},
    {"SteamNetworkingSockets012::CreateHostedDedicatedServerListenSocket", 33u, 34u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreateListenSocketIP", 33u, 0u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreateListenSocketP2P", 33u, 2u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreateListenSocketP2PFakeIP", 33u, 44u, 0, NULL, "tableau inout SteamNetworkingConfigValue_t[] pOptions"},
    {"SteamNetworkingSockets012::CreatePollGroup", 33u, 24u, 0, m924, NULL},
    {"SteamNetworkingSockets012::CreateSocketPair", 33u, 19u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pIdentity1"},
    {"SteamNetworkingSockets012::DestroyPollGroup", 33u, 25u, 0, m926, NULL},
    {"SteamNetworkingSockets012::FindRelayAuthTicketForServer", 33u, 29u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity identityGameServer"},
    {"SteamNetworkingSockets012::FlushMessagesOnConnection", 33u, 13u, 0, m928, NULL},
    {"SteamNetworkingSockets012::GetAuthenticationStatus", 33u, 23u, 0, NULL, "param\u00e8tre out SteamNetAuthenticationStatus_t pDetails"},
    {"SteamNetworkingSockets012::GetCertificateRequest", 33u, 38u, 0, NULL, "tampon IntPtr pBlob sans taille"},
    {"SteamNetworkingSockets012::GetConnectionInfo", 33u, 15u, 0, NULL, "param\u00e8tre out SteamNetConnectionInfo_t pInfo"},
    {"SteamNetworkingSockets012::GetConnectionName", 33u, 10u, 0, m932, NULL},
    {"SteamNetworkingSockets012::GetConnectionRealTimeStatus", 33u, 16u, 0, NULL, "param\u00e8tre ref SteamNetConnectionRealTimeStatus_t pStatus"},
    {"SteamNetworkingSockets012::GetConnectionUserData", 33u, 8u, 0, m934, NULL},
    {"SteamNetworkingSockets012::GetDetailedConnectionStatus", 33u, 17u, 0, m935, NULL},
    {"SteamNetworkingSockets012::GetFakeIP", 33u, 43u, 0, NULL, "param\u00e8tre out SteamNetworkingFakeIPResult_t pInfo"},
    {"SteamNetworkingSockets012::GetGameCoordinatorServerLogin", 33u, 35u, 0, NULL, "tampon IntPtr pLoginInfo sans taille"},
    {"SteamNetworkingSockets012::GetHostedDedicatedServerAddress", 33u, 33u, 0, m938, NULL},
    {"SteamNetworkingSockets012::GetHostedDedicatedServerPOPID", 33u, 32u, 0, m939, NULL},
    {"SteamNetworkingSockets012::GetHostedDedicatedServerPort", 33u, 31u, 0, m940, NULL},
    {"SteamNetworkingSockets012::GetIdentity", 33u, 21u, 0, NULL, "param\u00e8tre out SteamNetworkingIdentity pIdentity"},
    {"SteamNetworkingSockets012::GetListenSocketAddress", 33u, 18u, 0, m942, NULL},
    {"SteamNetworkingSockets012::GetRemoteFakeIPForConnection", 33u, 45u, 0, m943, NULL},
    {"SteamNetworkingSockets012::InitAuthentication", 33u, 22u, 0, m944, NULL},
    {"SteamNetworkingSockets012::ReceiveMessagesOnConnection", 33u, 14u, 0, NULL, "tableau inout IntPtr[] ppOutMessages"},
    {"SteamNetworkingSockets012::ReceiveMessagesOnPollGroup", 33u, 27u, 0, NULL, "tableau inout IntPtr[] ppOutMessages"},
    {"SteamNetworkingSockets012::ReceivedP2PCustomSignal", 33u, 37u, 0, NULL, "param\u00e8tre out ISteamNetworkingSignalingRecvContext pContext"},
    {"SteamNetworkingSockets012::ReceivedRelayAuthTicket", 33u, 28u, 0, NULL, "param\u00e8tre out SteamDatagramRelayAuthTicket pOutParsedTicket"},
    {"SteamNetworkingSockets012::ResetIdentity", 33u, 40u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pIdentity"},
    {"SteamNetworkingSockets012::RunCallbacks", 33u, 41u, 0, m950, NULL},
    {"SteamNetworkingSockets012::SendMessageToConnection", 33u, 11u, 0, m951, NULL},
    {"SteamNetworkingSockets012::SendMessages", 33u, 12u, 0, NULL, "tableau inout IntPtr[] pMessages"},
    {"SteamNetworkingSockets012::SetCertificate", 33u, 39u, 0, NULL, "param\u00e8tre out SteamNetworkingErrMsg errMsg"},
    {"SteamNetworkingSockets012::SetConnectionName", 33u, 9u, 0, m954, NULL},
    {"SteamNetworkingSockets012::SetConnectionPollGroup", 33u, 26u, 0, m955, NULL},
    {"SteamNetworkingSockets012::SetConnectionUserData", 33u, 7u, 0, m956, NULL},
    {"SteamNetworkingUtils004::AllocateMessage", 32u, 0u, 0, NULL, "retour IntPtr (IntPtr)"},
    {"SteamNetworkingUtils004::CheckPingDataUpToDate", 32u, 7u, 0, m958, NULL},
    {"SteamNetworkingUtils004::ConvertPingLocationToString", 32u, 5u, 0, NULL, "param\u00e8tre ref SteamNetworkPingLocation_t location"},
    {"SteamNetworkingUtils004::EstimatePingTimeBetweenTwoLocations", 32u, 3u, 0, NULL, "param\u00e8tre ref SteamNetworkPingLocation_t location1"},
    {"SteamNetworkingUtils004::EstimatePingTimeFromLocalHost", 32u, 4u, 0, NULL, "param\u00e8tre ref SteamNetworkPingLocation_t remoteLocation"},
    {"SteamNetworkingUtils004::GetConfigValue", 32u, 17u, 0, NULL, "tampon IntPtr scopeObj sans taille"},
    {"SteamNetworkingUtils004::GetConfigValueInfo", 32u, 18u, 0, m963, NULL},
    {"SteamNetworkingUtils004::GetDirectPingToPOP", 32u, 9u, 0, m964, NULL},
    {"SteamNetworkingUtils004::GetIPv4FakeIPType", 32u, 14u, 0, m965, NULL},
    {"SteamNetworkingUtils004::GetLocalPingLocation", 32u, 2u, 0, NULL, "param\u00e8tre out SteamNetworkPingLocation_t result"},
    {"SteamNetworkingUtils004::GetLocalTimestamp", 32u, 12u, 0, m967, NULL},
    {"SteamNetworkingUtils004::GetPOPCount", 32u, 10u, 0, m968, NULL},
    {"SteamNetworkingUtils004::GetPOPList", 32u, 11u, 0, m969, NULL},
    {"SteamNetworkingUtils004::GetPingToDataCenter", 32u, 8u, 0, m970, NULL},
    {"SteamNetworkingUtils004::GetRealIdentityForFakeIP", 32u, 15u, 0, NULL, "param\u00e8tre out SteamNetworkingIdentity pOutRealIdentity"},
    {"SteamNetworkingUtils004::GetRelayNetworkStatus", 32u, 1u, 0, NULL, "param\u00e8tre out SteamRelayNetworkStatus_t pDetails"},
    {"SteamNetworkingUtils004::IterateGenericEditableConfigValues", 32u, 19u, 0, m973, NULL},
    {"SteamNetworkingUtils004::ParsePingLocationString", 32u, 6u, 0, NULL, "param\u00e8tre out SteamNetworkPingLocation_t result"},
    {"SteamNetworkingUtils004::SetConfigValue", 32u, 16u, 0, NULL, "tampon IntPtr scopeObj sans taille"},
    {"SteamNetworkingUtils004::SetDebugOutputFunction", 32u, 13u, 0, NULL, "param\u00e8tre FSteamNetworkingSocketsDebugOutput pfnFunc"},
    {"SteamNetworkingUtils004::SteamNetworkingIPAddr_GetFakeIPType", 32u, 22u, 0, m977, NULL},
    {"SteamNetworkingUtils004::SteamNetworkingIPAddr_ParseString", 32u, 21u, 0, m978, NULL},
    {"SteamNetworkingUtils004::SteamNetworkingIdentity_ParseString", 32u, 24u, 0, NULL, "param\u00e8tre out SteamNetworkingIdentity pIdentity"},
    {"SteamParties002::CancelReservation", 30u, 8u, 0, m980, NULL},
    {"SteamParties002::ChangeNumOpenSlots", 30u, 9u, 0, m981, NULL},
    {"SteamParties002::CreateBeacon", 30u, 6u, 0, NULL, "param\u00e8tre ref SteamPartyBeaconLocation_t pBeaconLocation"},
    {"SteamParties002::DestroyBeacon", 30u, 10u, 0, m983, NULL},
    {"SteamParties002::GetAvailableBeaconLocations", 30u, 5u, 0, NULL, "tableau inout SteamPartyBeaconLocation_t[] pLocationList"},
    {"SteamParties002::GetBeaconByIndex", 30u, 1u, 0, m985, NULL},
    {"SteamParties002::GetBeaconDetails", 30u, 2u, 0, NULL, "param\u00e8tre out SteamPartyBeaconLocation_t pLocation"},
    {"SteamParties002::GetNumActiveBeacons", 30u, 0u, 0, m987, NULL},
    {"SteamParties002::GetNumAvailableBeaconLocations", 30u, 4u, 0, m988, NULL},
    {"SteamParties002::JoinParty", 30u, 3u, 0, m989, NULL},
    {"SteamParties002::OnReservationCompleted", 30u, 7u, 0, m990, NULL},
    {"SteamUser021::AdvertiseGame", 1u, 19u, 0, m991, NULL},
    {"SteamUser021::BIsBehindNAT", 1u, 18u, 0, m992, NULL},
    {"SteamUser021::BIsPhoneIdentifying", 1u, 27u, 0, m993, NULL},
    {"SteamUser021::BIsPhoneRequiringVerification", 1u, 28u, 0, m994, NULL},
    {"SteamUser021::BIsPhoneVerified", 1u, 25u, 0, m995, NULL},
    {"SteamUser021::BIsTwoFactorEnabled", 1u, 26u, 0, m996, NULL},
    {"SteamUser021::BLoggedOn", 1u, 1u, 0, m997, NULL},
    {"SteamUser021::BSetDurationControlOnlineState", 1u, 31u, 0, m998, NULL},
    {"SteamUser021::BeginAuthSession", 1u, 14u, 0, m999, NULL},
    {"SteamUser021::CancelAuthTicket", 1u, 16u, 0, m1000, NULL},
    {"SteamUser021::DecompressVoice", 1u, 11u, 0, m1001, NULL},
    {"SteamUser021::EndAuthSession", 1u, 15u, 0, m1002, NULL},
    {"SteamUser021::GetAuthSessionTicket", 1u, 13u, 0, m1003, NULL},
    {"SteamUser021::GetAvailableVoice", 1u, 9u, 0, NULL, "tampon IntPtr pcbUncompressed_Deprecated sans taille"},
    {"SteamUser021::GetDurationControl", 1u, 30u, 0, m1005, NULL},
    {"SteamUser021::GetEncryptedAppTicket", 1u, 21u, 0, m1006, NULL},
    {"SteamUser021::GetGameBadgeLevel", 1u, 22u, 0, m1007, NULL},
    {"SteamUser021::GetHSteamUser", 1u, 0u, 0, m1008, NULL},
    {"SteamUser021::GetMarketEligibility", 1u, 29u, 0, m1009, NULL},
    {"SteamUser021::GetPlayerSteamLevel", 1u, 23u, 0, m1010, NULL},
    {"SteamUser021::GetSteamID", 1u, 2u, 1, m1011, NULL},
    {"SteamUser021::GetUserDataFolder", 1u, 6u, 0, m1012, NULL},
    {"SteamUser021::GetVoiceOptimalSampleRate", 1u, 12u, 0, m1013, NULL},
    {"SteamUser021::InitiateGameConnection_DEPRECATED", 1u, 3u, 0, m1014, NULL},
    {"SteamUser021::RequestEncryptedAppTicket", 1u, 20u, 0, m1015, NULL},
    {"SteamUser021::RequestStoreAuthURL", 1u, 24u, 0, m1016, NULL},
    {"SteamUser021::StartVoiceRecording", 1u, 7u, 0, m1017, NULL},
    {"SteamUser021::StopVoiceRecording", 1u, 8u, 0, m1018, NULL},
    {"SteamUser021::TerminateGameConnection_DEPRECATED", 1u, 4u, 0, m1019, NULL},
    {"SteamUser021::UserHasLicenseForApp", 1u, 17u, 0, m1020, NULL},
    {"SteamUser023::AdvertiseGame", 26u, 20u, 0, m1021, NULL},
    {"SteamUser023::BIsBehindNAT", 26u, 19u, 0, m1022, NULL},
    {"SteamUser023::BIsPhoneIdentifying", 26u, 28u, 0, m1023, NULL},
    {"SteamUser023::BIsPhoneRequiringVerification", 26u, 29u, 0, m1024, NULL},
    {"SteamUser023::BIsPhoneVerified", 26u, 26u, 0, m1025, NULL},
    {"SteamUser023::BIsTwoFactorEnabled", 26u, 27u, 0, m1026, NULL},
    {"SteamUser023::BLoggedOn", 26u, 1u, 0, m1027, NULL},
    {"SteamUser023::BSetDurationControlOnlineState", 26u, 32u, 0, m1028, NULL},
    {"SteamUser023::BeginAuthSession", 26u, 15u, 0, m1029, NULL},
    {"SteamUser023::CancelAuthTicket", 26u, 17u, 0, m1030, NULL},
    {"SteamUser023::DecompressVoice", 26u, 11u, 0, m1031, NULL},
    {"SteamUser023::EndAuthSession", 26u, 16u, 0, m1032, NULL},
    {"SteamUser023::GetAuthSessionTicket", 26u, 13u, 0, NULL, "param\u00e8tre ref SteamNetworkingIdentity pSteamNetworkingIdentity"},
    {"SteamUser023::GetAuthTicketForWebApi", 26u, 14u, 0, m1034, NULL},
    {"SteamUser023::GetAvailableVoice", 26u, 9u, 0, NULL, "tampon IntPtr pcbUncompressed_Deprecated sans taille"},
    {"SteamUser023::GetDurationControl", 26u, 31u, 0, m1036, NULL},
    {"SteamUser023::GetEncryptedAppTicket", 26u, 22u, 0, m1037, NULL},
    {"SteamUser023::GetGameBadgeLevel", 26u, 23u, 0, m1038, NULL},
    {"SteamUser023::GetHSteamUser", 26u, 0u, 0, m1039, NULL},
    {"SteamUser023::GetMarketEligibility", 26u, 30u, 0, m1040, NULL},
    {"SteamUser023::GetPlayerSteamLevel", 26u, 24u, 0, m1041, NULL},
    {"SteamUser023::GetSteamID", 26u, 2u, 1, m1042, NULL},
    {"SteamUser023::GetUserDataFolder", 26u, 6u, 0, m1043, NULL},
    {"SteamUser023::GetVoiceOptimalSampleRate", 26u, 12u, 0, m1044, NULL},
    {"SteamUser023::InitiateGameConnection_DEPRECATED", 26u, 3u, 0, m1045, NULL},
    {"SteamUser023::RequestEncryptedAppTicket", 26u, 21u, 0, m1046, NULL},
    {"SteamUser023::RequestStoreAuthURL", 26u, 25u, 0, m1047, NULL},
    {"SteamUser023::StartVoiceRecording", 26u, 7u, 0, m1048, NULL},
    {"SteamUser023::StopVoiceRecording", 26u, 8u, 0, m1049, NULL},
    {"SteamUser023::TerminateGameConnection_DEPRECATED", 26u, 4u, 0, m1050, NULL},
    {"SteamUser023::UserHasLicenseForApp", 26u, 18u, 0, m1051, NULL},
    {"SteamUtils010::BOverlayNeedsPresent", 3u, 18u, 0, m1052, NULL},
    {"SteamUtils010::CheckFileSignature", 3u, 19u, 0, m1053, NULL},
    {"SteamUtils010::DismissFloatingGamepadTextInput", 3u, 37u, 0, m1054, NULL},
    {"SteamUtils010::FilterText", 3u, 32u, 0, m1055, NULL},
    {"SteamUtils010::GetAPICallFailureReason", 3u, 12u, 0, m1056, NULL},
    {"SteamUtils010::GetAPICallResult", 3u, 13u, 0, m1057, NULL},
    {"SteamUtils010::GetAppID", 3u, 9u, 0, m1058, NULL},
    {"SteamUtils010::GetConnectedUniverse", 3u, 2u, 0, m1059, NULL},
    {"SteamUtils010::GetCurrentBatteryPower", 3u, 8u, 0, m1060, NULL},
    {"SteamUtils010::GetEnteredGamepadTextInput", 3u, 22u, 0, m1061, NULL},
    {"SteamUtils010::GetEnteredGamepadTextLength", 3u, 21u, 0, m1062, NULL},
    {"SteamUtils010::GetIPCCallCount", 3u, 15u, 0, m1063, NULL},
    {"SteamUtils010::GetIPCountry", 3u, 4u, 0, m1064, NULL},
    {"SteamUtils010::GetIPv6ConnectivityState", 3u, 33u, 0, m1065, NULL},
    {"SteamUtils010::GetImageRGBA", 3u, 6u, 0, m1066, NULL},
    {"SteamUtils010::GetImageSize", 3u, 5u, 0, m1067, NULL},
    {"SteamUtils010::GetSecondsSinceAppActive", 3u, 0u, 0, m1068, NULL},
    {"SteamUtils010::GetSecondsSinceComputerActive", 3u, 1u, 0, m1069, NULL},
    {"SteamUtils010::GetServerRealTime", 3u, 3u, 0, m1070, NULL},
    {"SteamUtils010::GetSteamUILanguage", 3u, 23u, 0, m1071, NULL},
    {"SteamUtils010::InitFilterText", 3u, 31u, 0, m1072, NULL},
    {"SteamUtils010::IsAPICallCompleted", 3u, 11u, 0, m1073, NULL},
    {"SteamUtils010::IsOverlayEnabled", 3u, 17u, 0, m1074, NULL},
    {"SteamUtils010::IsSteamChinaLauncher", 3u, 30u, 0, m1075, NULL},
    {"SteamUtils010::IsSteamInBigPictureMode", 3u, 26u, 0, m1076, NULL},
    {"SteamUtils010::IsSteamRunningInVR", 3u, 24u, 0, m1077, NULL},
    {"SteamUtils010::IsSteamRunningOnSteamDeck", 3u, 34u, 0, m1078, NULL},
    {"SteamUtils010::IsVRHeadsetStreamingEnabled", 3u, 28u, 0, m1079, NULL},
    {"SteamUtils010::SetGameLauncherMode", 3u, 36u, 0, m1080, NULL},
    {"SteamUtils010::SetOverlayNotificationInset", 3u, 25u, 0, m1081, NULL},
    {"SteamUtils010::SetOverlayNotificationPosition", 3u, 10u, 0, m1082, NULL},
    {"SteamUtils010::SetVRHeadsetStreamingEnabled", 3u, 29u, 0, m1083, NULL},
    {"SteamUtils010::SetWarningMessageHook", 3u, 16u, 0, steam_manual_utils_warning_hook, NULL},
    {"SteamUtils010::ShowFloatingGamepadTextInput", 3u, 35u, 0, m1085, NULL},
    {"SteamUtils010::ShowGamepadTextInput", 3u, 20u, 0, m1086, NULL},
    {"SteamUtils010::StartVRDashboard", 3u, 27u, 0, m1087, NULL},
};
const size_t steam_method_count = 1088;
