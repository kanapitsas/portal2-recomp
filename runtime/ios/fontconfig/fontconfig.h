// Sous-ensemble de l'API publique Fontconfig utilisé par runtime/fonts.c,
// implémenté par runtime/ios/fontconfig_min.c (iOS n'a pas Fontconfig).
// Types et valeurs identiques à Fontconfig 2.x pour les parties reprises.
#pragma once
#include <stddef.h>

typedef unsigned char FcChar8;
typedef int FcBool;
#define FcFalse 0
#define FcTrue 1

typedef enum { FcTypeUnknown = -1, FcTypeVoid, FcTypeInteger, FcTypeDouble, FcTypeString, FcTypeBool,
               FcTypeMatrix, FcTypeCharSet, FcTypeFTFace, FcTypeLangSet, FcTypeRange } FcType;
typedef struct { double xx, xy, yx, yy; } FcMatrix;
typedef struct {
    FcType type;
    union { const FcChar8 *s; int i; FcBool b; double d; const FcMatrix *m; const void *f; } u;
} FcValue;
typedef enum { FcResultMatch, FcResultNoMatch, FcResultTypeMismatch, FcResultNoId, FcResultOutOfMemory } FcResult;
typedef enum { FcMatchPattern, FcMatchFont, FcMatchScan } FcMatchKind;

typedef struct _FcPattern FcPattern;
typedef struct _FcConfig FcConfig;
typedef struct _FcObjectSet FcObjectSet;
typedef struct { int nfont; int sfont; FcPattern **fonts; } FcFontSet;

#define FC_FAMILY "family"
#define FC_STYLE "style"
#define FC_SLANT "slant"
#define FC_WEIGHT "weight"
#define FC_FILE "file"
#define FC_INDEX "index"
#define FC_SCALABLE "scalable"
#define FC_WEIGHT_REGULAR 80
#define FC_WEIGHT_BOLD 200
#define FC_SLANT_ROMAN 0
#define FC_SLANT_ITALIC 100

FcBool FcInit(void);
FcConfig *FcConfigGetCurrent(void);
FcConfig *FcConfigReference(FcConfig *config);
FcConfig *FcInitLoadConfigAndFonts(void);
FcBool FcConfigSetCurrent(FcConfig *config);
FcBool FcConfigAppFontAddFile(FcConfig *config, const FcChar8 *file);
FcBool FcConfigSubstitute(FcConfig *config, FcPattern *p, FcMatchKind kind);
void FcDefaultSubstitute(FcPattern *pattern);
FcPattern *FcPatternCreate(void);
void FcPatternDestroy(FcPattern *p);
FcBool FcPatternAdd(FcPattern *p, const char *object, FcValue value, FcBool append);
FcResult FcPatternGetString(const FcPattern *p, const char *object, int n, FcChar8 **s);
FcResult FcPatternGetBool(const FcPattern *p, const char *object, int n, FcBool *b);
FcPattern *FcFontMatch(FcConfig *config, FcPattern *p, FcResult *result);
FcObjectSet *FcObjectSetCreate(void);
FcBool FcObjectSetAdd(FcObjectSet *os, const char *object);
void FcObjectSetDestroy(FcObjectSet *os);
FcFontSet *FcFontList(FcConfig *config, FcPattern *p, FcObjectSet *os);
void FcFontSetDestroy(FcFontSet *s);
