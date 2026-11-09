// libm i386 : `float` occupe 4 octets sur la pile, `double` 8 (alignement 4),
// le résultat revient toujours dans ST(0).
//
// Précision : glibc i386 calcule souvent en x87 80 bits et laisse le résultat
// étendu dans ST(0) ; le runtime modélise ST en double. Les fonctions double
// sont donc arrondies au double (comme si l'appelant stockait le résultat), et
// les valeurs peuvent différer de la glibc d'un ulp (libm Darwin). Les variantes
// float renvoient le float hôte élargi, ce qui est exactement la valeur float.
//
// errno : glibc positionne errno (math_errhandling & MATH_ERRNO), Darwin non.
// Règles reproduites : NaN produit à partir d'arguments non NaN → EDOM ;
// infini produit à partir d'arguments finis (pôle ou dépassement) → ERANGE ;
// sous-dépassement vers 0 pour exp/exp2/pow/ldexp → ERANGE. __pow_finite est la
// version sans enveloppe : jamais d'errno.
// SHIMS: acos acosf asin asinf atan atanf atan2 atan2f cbrtf ceil ceilf cos cosf cosh coshf
// SHIMS: sqrt exp2f fmaxf fminf
// SHIMS: exp exp2 expf floor floorf fmod fmodf frexp ldexp log log10 log10f logf modf
// SHIMS: pow powf __pow_finite sin sinf sinh sinhf sincos sincosf sqrtf tan tanf tanh tanhf trunc
#include "rt.h"
#include <errno.h>
#include <math.h>


enum { UFLOW = 1 };
static void check(CPU *c, double result, double x, double y, int arity, int flags) {
    int nan_in = isnan(x) || (arity > 1 && isnan(y));
    int finite_in = isfinite(x) && (arity < 2 || isfinite(y));
    if (isnan(result) && !nan_in) rt_set_errno(c, EDOM);
    else if (isinf(result) && finite_in) rt_set_errno(c, ERANGE);
    else if ((flags & UFLOW) && result == 0 && finite_in && x != 0) rt_set_errno(c, ERANGE);
}

typedef double (*unary_d)(double);
typedef float (*unary_f)(float);
static const struct { const char *name; unary_d fd; unary_f ff; int flags; } unary[] = {
    {"acos", acos, 0, 0}, {"acosf", 0, acosf, 0}, {"asin", asin, 0, 0}, {"asinf", 0, asinf, 0},
    {"atan", atan, 0, 0}, {"atanf", 0, atanf, 0}, {"cbrtf", 0, cbrtf, 0},
    {"ceil", ceil, 0, 0}, {"ceilf", 0, ceilf, 0}, {"cos", cos, 0, 0}, {"cosf", 0, cosf, 0},
    {"cosh", cosh, 0, 0}, {"coshf", 0, coshf, 0}, {"exp", exp, 0, UFLOW}, {"exp2", exp2, 0, UFLOW},
    {"expf", 0, expf, UFLOW}, {"floor", floor, 0, 0}, {"floorf", 0, floorf, 0},
    {"log", log, 0, 0}, {"log10", log10, 0, 0}, {"log10f", 0, log10f, 0}, {"logf", 0, logf, 0},
    {"sin", sin, 0, 0}, {"sinf", 0, sinf, 0}, {"sinh", sinh, 0, 0}, {"sinhf", 0, sinhf, 0},
    {"sqrtf", 0, sqrtf, 0}, {"sqrt", sqrt, 0, 0}, {"exp2f", 0, exp2f, UFLOW}, {"tan", tan, 0, 0}, {"tanf", 0, tanf, 0},
    {"tanh", tanh, 0, 0}, {"tanhf", 0, tanhf, 0}, {"trunc", trunc, 0, 0},
};

static void run_unary(CPU *c, size_t i) {
    double x, result;
    if (unary[i].fd) { x = rt_arg_f64(c, 0); result = unary[i].fd(x); }
    else { float xf = rt_arg_f32(c, 0); x = xf; result = unary[i].ff(xf); }
    // Pour exp*, -inf → 0 n'est pas un sous-dépassement (argument non fini).
    check(c, result, x, 0, 1, unary[i].flags);
    rt_return_st0(c, result);
}

static int sincosf_handler(CPU *c, const char *name, void *data);
static int rt_math_shim(CPU *c, const char *name) {
    for (size_t i = 0; i < sizeof(unary) / sizeof(*unary); ++i) {
        if (strcmp(name, unary[i].name)) continue;
        run_unary(c, i);
        return 1;
    }
    double x, y, result;
    if (!strcmp(name, "atan2")) { x = rt_arg_f64(c, 0); y = rt_arg_f64(c, 8); result = atan2(x, y); check(c, result, x, y, 2, 0); }
    else if (!strcmp(name, "atan2f")) { x = rt_arg_f32(c, 0); y = rt_arg_f32(c, 4); result = atan2f((float)x, (float)y); check(c, result, x, y, 2, 0); }
    else if (!strcmp(name, "fmod")) { x = rt_arg_f64(c, 0); y = rt_arg_f64(c, 8); result = fmod(x, y); check(c, result, x, y, 2, 0); }
    else if (!strcmp(name, "fmodf")) { x = rt_arg_f32(c, 0); y = rt_arg_f32(c, 4); result = fmodf((float)x, (float)y); check(c, result, x, y, 2, 0); }
    else if (!strcmp(name, "pow")) { x = rt_arg_f64(c, 0); y = rt_arg_f64(c, 8); result = pow(x, y); check(c, result, x, y, 2, UFLOW); }
    else if (!strcmp(name, "powf")) { x = rt_arg_f32(c, 0); y = rt_arg_f32(c, 4); result = powf((float)x, (float)y); check(c, result, x, y, 2, UFLOW); }
    else if (!strcmp(name, "__pow_finite")) result = pow(rt_arg_f64(c, 0), rt_arg_f64(c, 8));
    else if (!strcmp(name, "fmaxf")) result = fmaxf(rt_arg_f32(c, 0), rt_arg_f32(c, 4));   // sans errno
    else if (!strcmp(name, "fminf")) result = fminf(rt_arg_f32(c, 0), rt_arg_f32(c, 4));
    else if (!strcmp(name, "ldexp")) {
        x = rt_arg_f64(c, 0); int exponent = (int32_t)rt_arg32(c, 2);
        result = ldexp(x, exponent); check(c, result, x, 0, 1, UFLOW);
    } else if (!strcmp(name, "frexp")) {
        x = rt_arg_f64(c, 0); uint32_t out = rt_arg32(c, 2);
        int exponent = 0; result = frexp(x, &exponent);
        W32(out, (uint32_t)exponent);
    } else if (!strcmp(name, "modf")) {
        x = rt_arg_f64(c, 0); uint32_t out = rt_arg32(c, 2);
        double whole; result = modf(x, &whole);
        WD(out, whole);
    } else if (!strcmp(name, "sincos")) {
        x = rt_arg_f64(c, 0); uint32_t s = rt_arg32(c, 2), co = rt_arg32(c, 3);
        double sv = sin(x), cv = cos(x);
        check(c, sv, x, 0, 1, 0);
        WD(s, sv); WD(co, cv);
        return 1;  // void : rien dans ST(0)
    } else if (!strcmp(name, "sincosf")) return sincosf_handler(c, name, NULL);
    else return 0;
    rt_return_st0(c, result);
    return 1;
}
// Route directe (sans recherche par nom) : fonctions unaires par leur entrée
// de table, sincosf par un handler dédié (appelée des dizaines de milliers de
// fois par seconde en jeu) ; les autres gardent le fournisseur.
static int unary_handler(CPU *c, const char *name, void *data) {
    (void)name; run_unary(c, (size_t)(uintptr_t)data); return 1;
}
static int sincosf_handler(CPU *c, const char *name, void *data) {
    (void)name; (void)data;
    float xf = rt_arg_f32(c, 0); uint32_t s = rt_arg32(c, 1), co = rt_arg32(c, 2);
    float sv = sinf(xf), cv = cosf(xf);
    check(c, sv, xf, 0, 1, 0);
    WF(s, sv); WF(co, cv);
    return 1;
}
static int fminmaxf_handler(CPU *c, const char *name, void *data) {
    (void)name;
    float x = rt_arg_f32(c, 0), y = rt_arg_f32(c, 4);
    rt_return_st0(c, data ? fmaxf(x, y) : fminf(x, y));   // sans errno, comme le fournisseur
    return 1;
}
static int math_resolve(const char *name, RtShimRoute *route) {
    if (!strcmp(name, "fmaxf")) { route->handler = fminmaxf_handler; route->data = (void *)1; return 1; }
    if (!strcmp(name, "fminf")) { route->handler = fminmaxf_handler; route->data = NULL; return 1; }
    for (size_t i = 0; i < sizeof(unary) / sizeof(*unary); ++i)
        if (!strcmp(name, unary[i].name)) { route->handler = unary_handler; route->data = (void *)(uintptr_t)i; return 1; }
    if (!strcmp(name, "sincosf")) { route->handler = sincosf_handler; return 1; }
    return 0;
}
RT_SHIM_PROVIDER_RESOLVER(rt_math_shim, math_resolve)
