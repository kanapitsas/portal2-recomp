#include "posix.h"
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
const char *rt_ios_case_path(const char *canonical, size_t trusted);
#endif
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

static char game_root[PATH_MAX], linux_root[PATH_MAX], user_root[PATH_MAX];
static _Thread_local char buffers[4][PATH_MAX];
static _Thread_local unsigned next_buffer;
static char *buffer(void) { return buffers[next_buffer++ % 4]; }
static _Atomic unsigned long generation = 1;   // incrémenté à chaque écriture invitée
unsigned long rt_paths_generation(void) { return generation; }
static int insensitive[3];                       // volume de chaque racine insensible à la casse
static int within(const char *path,const char *root) {
    size_t length=strlen(root);
    return length && !strncmp(path,root,length) && (!path[length] || path[length]=='/');
}
static int absolute_path(const char *path,char *output) {
    char joined[PATH_MAX]; int count;
    if (path[0]=='/') count=snprintf(joined,sizeof(joined),"%s",path);
    else {
        char cwd[PATH_MAX]; if (!getcwd(cwd,sizeof(cwd))) return -1;
        count=snprintf(joined,sizeof(joined),"%s/%s",cwd,path);
    }
    if(count<0 || count>=(int)sizeof(joined)){errno=ENAMETOOLONG;return -1;}
    size_t length=1; output[0]='/'; output[1]=0;
    char *save=NULL;
    for(char *part=strtok_r(joined,"/",&save);part;part=strtok_r(NULL,"/",&save)) {
        if(!strcmp(part,"."))continue;
        if(!strcmp(part,"..")){
            if(length>1){while(length>1 && output[length-1]!='/')--length;if(length>1)--length;output[length]=0;}
            continue;
        }
        size_t add=strlen(part);
        if(length+(length>1)+add>=PATH_MAX){errno=ENAMETOOLONG;return -1;}
        if(length>1)output[length++]='/';
        memcpy(output+length,part,add+1); length+=add;
    }
    // Sous une racine déjà canonique (jeu, dépôt Linux, données utilisateur) :
    // pas de realpath, qui coûte un getattrlist par composant à chaque accès
    // (20 % du temps CPU du jeu). Les liens créés par l'invité sont résolus
    // à part (overlay_link).
    if(within(output,game_root)||within(output,linux_root)||within(output,user_root))return 0;
    // Volume insensible à la casse (APFS par défaut) : le correcteur de casse
    // de Source essaie des variantes (« /volumes/crucial p3/... ») ; elles
    // désignent la même racine, réécrite dans sa casse canonique.
    const char *roots[3]={game_root,linux_root,user_root};
    for(int i=0;i<3;++i){
        size_t length=strlen(roots[i]);
        if(insensitive[i] && length && !strncasecmp(output,roots[i],length) && (!output[length]||output[length]=='/')){
            memcpy(output,roots[i],length); return 0;
        }
    }
    // Parent d'une racine (« …/common », « /volumes ») : le correcteur de casse
    // remonte jusque-là ; ces dossiers existent, avec la casse de la racine.
    size_t out_length=strlen(output);
    for(int i=0;i<3;++i){
        size_t length=strlen(roots[i]);
        int same=insensitive[i]?!strncasecmp(output,roots[i],out_length):!strncmp(output,roots[i],out_length);
        if(out_length>1 && out_length<length && same && roots[i][out_length]=='/'){
            memcpy(output,roots[i],out_length); return 0;
        }
    }
    // Resolve existing prefixes too: /tmp is /private/tmp on macOS, including
    // when the requested file has not been created yet.
    char prefix[PATH_MAX], resolved[PATH_MAX]; strcpy(prefix,output);
    size_t cut=strlen(prefix);
    while(!realpath(prefix,resolved)) {
        if(cut<=1) return -1;
        while(cut>1 && prefix[cut-1]!='/')--cut;
        if(cut>1)--cut;
        prefix[cut]=0;
    }
    const char *suffix=output+cut;
    char combined[PATH_MAX];
    int total=snprintf(combined,sizeof(combined),"%s%s",resolved,suffix);
    if(total<0 || total>=PATH_MAX){errno=ENAMETOOLONG;return -1;}
    strcpy(output,combined);
    return 0;
}
int rt_paths_init(const char *game,const char *linux,const char *user) {
    ++generation;   // nouvelles racines : caches invalides
    if(!realpath(game,game_root) || !realpath(linux,linux_root))return -1;
    if(absolute_path(user,user_root))return -1;
    if(mkdir(user_root,0755) && errno!=EEXIST)return -1;
    const char *roots[3]={game_root,linux_root,user_root};
    for(int i=0;i<3;++i)insensitive[i]=pathconf(roots[i],_PC_CASE_SENSITIVE)==0;
#if TARGET_OS_IPHONE
    // iOS compare la casse même sur un volume insensible (bac à sable) : les
    // variantes du correcteur de casse de Source sont résolues ici et par
    // rt_ios_case_path (runtime/ios/case_path.c).
    for(int i=0;i<3;++i)insensitive[i]=1;
#endif
    return 0;
}
// Le répertoire des marques de suppression n'existe pas dans la vue invitée.
static int reserved(const char *relative) {
    size_t length=strlen(RT_WHITEOUTS);
    return !strncmp(relative,RT_WHITEOUTS,length) && (!relative[length] || relative[length]=='/');
}
static int overlay_link(const char *result,const char *relative,int preserve_existing,int follow_final,const char **out);
// Cache des résolutions de lecture sous la racine du jeu : le menu sonde en
// boucle des fichiers absents (cartes du workshop...) et chaque résolution
// coûte cinq à six appels système. Une entrée vaut pour une génération : toute
// écriture invitée (write_path, marques de suppression) ou changement de
// racines l'invalide. Les changements faits hors du jeu ne sont pas vus.
#define READ_CACHE 4096u
#define ACCESS_UNKNOWN (-1)
// f_ok/r_ok : résultat mémorisé de access(F_OK/R_OK) sur le chemin résolu
// (0, un errno, ou ACCESS_UNKNOWN) ; même invalidation que la résolution.
typedef struct { unsigned long generation; int error, f_ok, r_ok; char canonical[PATH_MAX]; char result[PATH_MAX]; } ReadEntry;
static ReadEntry *read_cache;
static pthread_mutex_t read_lock=PTHREAD_MUTEX_INITIALIZER;
static unsigned read_slot(const char *canonical) {
    unsigned h=2166136261u; for(const char *p=canonical;*p;++p)h=(h^(unsigned char)*p)*16777619u;
    return h&(READ_CACHE-1);
}
static const char *read_path_uncached(const char *path);
const char *rt_read_path(const char *path) {
    if(rt_is_proc_path(path))return rt_proc_path(path);   // fichiers /proc générés (procfs.c)
    if(!game_root[0])return path;
    char canonical[PATH_MAX]; if(absolute_path(path,canonical))return NULL;
    if(!within(canonical,game_root)) {
#if TARGET_OS_IPHONE
        // iOS compare la casse : chemin résolu composant par composant (case_path.c).
        if(within(canonical,linux_root))return rt_ios_case_path(canonical,strlen(linux_root));
        if(within(canonical,user_root))return rt_ios_case_path(canonical,strlen(user_root));
#endif
        return path;
    }
    unsigned long now=generation; unsigned slot=read_slot(canonical);
    pthread_mutex_lock(&read_lock);
    if(!read_cache)read_cache=calloc(READ_CACHE,sizeof(*read_cache));
    ReadEntry *e=read_cache?&read_cache[slot]:NULL;
    if(e && e->generation==now && !strcmp(e->canonical,canonical)){
        int error=e->error; char *result=error?NULL:buffer();
        if(result)strcpy(result,e->result);
        pthread_mutex_unlock(&read_lock);
        if(error){errno=error;return NULL;}
        return result;
    }
    pthread_mutex_unlock(&read_lock);
    const char *result=read_path_uncached(path); int error=result?0:errno;
    if(!result && error!=ENOENT)return NULL;   // erreurs inattendues : non mémorisées
    pthread_mutex_lock(&read_lock);
    if(e && strlen(canonical)<PATH_MAX && (!result || strlen(result)<PATH_MAX)){
        e->generation=now; e->error=error; e->f_ok=e->r_ok=ACCESS_UNKNOWN; strcpy(e->canonical,canonical);
        if(result)strcpy(e->result,result);
    }
    pthread_mutex_unlock(&read_lock);
    if(!result)errno=error;
    return result;
}
// access(F_OK|R_OK) mémorisé : filesystem_stdio teste l'existence des mêmes
// fichiers en continu, même en jeu (27 % du thread principal). Renvoie 0, -1
// (errno positionné), ou -2 si le cas n'est pas mémorisable (autre mode, hors
// du jeu) : l'appelant fait alors l'appel système.
int rt_access_cached(const char *path,int mode) {
    if((mode!=F_OK && mode!=R_OK) || !game_root[0] || rt_is_proc_path(path))return -2;
    char canonical[PATH_MAX]; if(absolute_path(path,canonical))return -2;
    // Parent d'une racine (le correcteur de casse remonte jusqu'à « /volumes »
    // à chaque fichier absent, 1,7 million d'appels en 150 s) : c'est un dossier
    // existant et lisible, sans appel système.
    const char *roots[3]={game_root,linux_root,user_root};
    for(int i=0;i<3;++i){
        size_t length=strlen(canonical);
        if(length && !strncmp(roots[i],canonical,length) && (roots[i][length]=='/' || (length==1 && canonical[0]=='/')))return 0;
    }
    if(!within(canonical,game_root))return -2;
    for(int attempt=0;attempt<2;++attempt){
        unsigned long now=generation; unsigned slot=read_slot(canonical);
        pthread_mutex_lock(&read_lock);
        ReadEntry *e=read_cache?&read_cache[slot]:NULL;
        if(e && e->generation==now && !strcmp(e->canonical,canonical)){
            int *field=mode==F_OK?&e->f_ok:&e->r_ok, outcome;
            if(e->error)outcome=e->error;
            else if(*field!=ACCESS_UNKNOWN)outcome=*field;
            else { outcome=access(e->result,mode)?errno:0; *field=outcome; }
            pthread_mutex_unlock(&read_lock);
            if(outcome){errno=outcome;return -1;}
            return 0;
        }
        pthread_mutex_unlock(&read_lock);
        if(!rt_read_path(path))return -1;   // remplit le cache, puis seconde passe
    }
    return -2;
}
static const char *read_path_uncached(const char *path) {
    char canonical[PATH_MAX]; if(absolute_path(path,canonical))return NULL;
    const char *relative=canonical+strlen(game_root); while(*relative=='/')++relative;
    if(reserved(relative)){errno=ENOENT;return NULL;}
    char *result=buffer();
    int count=snprintf(result,PATH_MAX,"%s/%s",user_root,relative);
    if(count<0 || count>=PATH_MAX){errno=ENAMETOOLONG;return NULL;}
    const char *linked;
    if(*relative && overlay_link(result,relative,-1,1,&linked))return linked;
#if TARGET_OS_IPHONE
    // Chaque couche est sondée dans sa casse réelle, pas dans celle demandée.
    {const char *c=rt_ios_case_path(result,strlen(user_root)); if(c && c!=result)strcpy(result,c);}
#endif
    struct stat info;
    if(!stat(result,&info) && (!S_ISDIR(info.st_mode) || access(canonical,F_OK)))return result;
    // Supprimé par l'invité : la copie utilisateur absente donne ENOENT.
    if(lstat(result,&info) && rt_path_is_whiteout(relative)==1)return result;
    count=snprintf(result,PATH_MAX,"%s/%s",linux_root,relative);
    if(count<0 || count>=PATH_MAX){errno=ENAMETOOLONG;return NULL;}
#if TARGET_OS_IPHONE
    {const char *c=rt_ios_case_path(result,strlen(linux_root)); if(c && c!=result)strcpy(result,c);}
#endif
    if(!stat(result,&info) && (!S_ISDIR(info.st_mode) || access(canonical,F_OK)))return result;
    const char *base=strrchr(relative,'/');
    if(base && strstr(relative,"bin/")) {
        const char *directory=!strncmp(relative,"portal2/",8)?"portal2/bin/linux32":"bin/linux32";
        count=snprintf(result,PATH_MAX,"%s/%s/%s",linux_root,directory,base+1);
        if(count>=0 && count<PATH_MAX && !stat(result,&info))return result;
    }
    if(rt_path_windows_binary(relative)){errno=ENOENT;return NULL;}   // binaire Windows : absent sous Linux
#if TARGET_OS_IPHONE
    return rt_ios_case_path(canonical,strlen(game_root));
#else
    return path;
#endif
}
#define WHITEOUTS RT_WHITEOUTS
static _Thread_local char parent_buffer[PATH_MAX];
static const char *relative_parent(const char *relative,size_t length) {
    if(length>=PATH_MAX)return NULL;
    memcpy(parent_buffer,relative,length);parent_buffer[length]=0;return parent_buffer;
}
// Répertoire présent dans une source originale et non supprimé par l'invité.
static int lower_directory(const char *relative) {
    if(rt_path_is_whiteout(relative)==1)return 0;
    char lower[2][PATH_MAX];int count=rt_path_lower(relative,lower);
    struct stat info;
    for(int i=0;i<count;++i){
        const char *candidate=lower[i];
#if TARGET_OS_IPHONE
        // iOS compare la casse : « portal2/save » désigne le dossier « SAVE » du jeu.
        size_t trusted=within(candidate,game_root)?strlen(game_root):within(candidate,linux_root)?strlen(linux_root):0;
        const char *real=rt_ios_case_path(candidate,trusted); if(real)candidate=real;
#endif
        if(!stat(candidate,&info) && S_ISDIR(info.st_mode))return 1;
    }
    return 0;
}
int rt_path_relative(const char *path,char relative[PATH_MAX]) {
    if(!game_root[0])return 0;
    char canonical[PATH_MAX]; if(absolute_path(path,canonical))return -1;
    if(!within(canonical,game_root))return 0;
    const char *r=canonical+strlen(game_root); while(*r=='/')++r;
    strcpy(relative,r); return 1;
}
static int join(char out[PATH_MAX],const char *root,const char *relative) {
    int count=*relative?snprintf(out,PATH_MAX,"%s/%s",root,relative):snprintf(out,PATH_MAX,"%s",root);
    if(count<0 || count>=PATH_MAX){errno=ENAMETOOLONG;return -1;}
    return 0;
}
int rt_path_overlay(const char *relative,char out[PATH_MAX]) {
    if(join(out,user_root,relative))return -1;
#if TARGET_OS_IPHONE
    // Casse réelle des composants existants (suppressions, renommages : posix.c).
    {const char *c=rt_ios_case_path(out,strlen(user_root)); if(c && c!=out)strcpy(out,c);}
#endif
    return 0;
}
int rt_path_absolute(const char *path,char out[PATH_MAX]) { return absolute_path(path,out); }
static int join(char out[PATH_MAX],const char *root,const char *relative);
// Greffon Miles de bin/linux32 traduit d'avance (pe.c) : visible seulement si
// le fichier de la copie du jeu est exactement celui qui a été traduit.
static int translated_plugin(const char *relative) {
    static const char prefix[]="bin/linux32/";
    if(strncasecmp(relative,prefix,sizeof(prefix)-1))return 0;
    const char *name=relative+sizeof(prefix)-1;
    if(!*name || strchr(name,'/'))return 0;
    const RtPeImage *image=rt_pe_image_named(name);
    char host[PATH_MAX];
    if(!image || !game_root[0] || join(host,game_root,relative))return 0;
    return rt_pe_file_matches(image,host);
}
// La copie du jeu est l'installation Windows : ses binaires (DLL, EXE, greffons
// Miles .asi/.flt) n'existent pas dans une installation Linux et ne doivent pas
// être vus par le jeu Linux (Miles chargeait mssmp3.asi, un greffon PE, et
// sautait dans son code non traduit). Exception : les greffons de bin/linux32
// traduits d'avance, que Miles charge comme sous Linux. `relative` est relatif
// à la racine du jeu.
int rt_path_windows_binary(const char *relative) {
    const char *base=strrchr(relative,'/'); base=base?base+1:relative;
    const char *dot=strrchr(base,'.');
    if(!dot)return 0;
    static const char *const ext[]={".dll",".exe",".asi",".flt",".pdb",".sys"};
    for(size_t i=0;i<sizeof(ext)/sizeof(*ext);++i)
        if(!strcasecmp(dot,ext[i]))return !translated_plugin(relative);
    return 0;
}
int rt_path_is_game_source(const char *host) { return game_root[0] && within(host,game_root); }
int rt_path_lower(const char *relative,char out[2][PATH_MAX]) {
    int count=0; struct stat info;
    if(!join(out[count],linux_root,relative) && !lstat(out[count],&info))++count;
    if(!rt_path_windows_binary(relative) && !join(out[count],game_root,relative) && !lstat(out[count],&info))++count;
    return count;
}
static int whiteout_marker(const char *relative,char out[PATH_MAX]) {
    int count=*relative?snprintf(out,PATH_MAX,"%s/" WHITEOUTS "/%s/.wh",user_root,relative):-1;
    if(count<0 || count>=PATH_MAX){errno=*relative?ENAMETOOLONG:EINVAL;return -1;}
    return 0;
}
int rt_path_is_whiteout(const char *relative) {
    char marker[PATH_MAX]; if(whiteout_marker(relative,marker))return -1;
    return !access(marker,F_OK);
}
int rt_path_whiteout(const char *relative) {
    ++generation;
    char marker[PATH_MAX]; if(whiteout_marker(relative,marker))return -1;
    for(char *p=marker+strlen(user_root)+1;*p;++p)if(*p=='/'){
        *p=0; int error=mkdir(marker,0755); *p='/'; if(error && errno!=EEXIST)return -1;
    }
    FILE *file=fopen(marker,"wb"); if(!file)return -1;
    return fclose(file)?-1:0;
}
int rt_path_clear_whiteout(const char *relative) {
    ++generation;
    char marker[PATH_MAX]; if(whiteout_marker(relative,marker))return -1;
    return unlink(marker) && errno!=ENOENT?-1:0;
}
int rt_path_hidden_name(const char *relative_dir,const char *name) {
    if(!*relative_dir && !strcmp(name,WHITEOUTS))return 1;
    char relative[PATH_MAX];
    int count=*relative_dir?snprintf(relative,PATH_MAX,"%s/%s",relative_dir,name):snprintf(relative,PATH_MAX,"%s",name);
    if(count<0 || count>=PATH_MAX)return 0;
    return rt_path_is_whiteout(relative)==1;
}
int rt_guest_realpath(const char *path,char out[PATH_MAX]) {
    if(!*path){errno=ENOENT;return -1;}
    if(absolute_path(path,out))return -1;
    const char *resolved=rt_read_path(out); struct stat info;
    if(!resolved || stat(resolved,&info))return -1;
    return 0;
}
static _Thread_local unsigned link_depth;
static const char *write_path(const char *path,int preserve_existing,int follow_final);
// Un lien symbolique créé par l'invité dans la copie utilisateur est résolu
// dans la vue invitée : suivre le lien sur l'hôte pourrait écrire dans la copie
// originale du jeu, et une lecture doit retrouver la cible dans les autres
// sources. preserve_existing < 0 : résolution en lecture. Renvoie 1 si un lien
// a été remplacé dans *out.
static int overlay_link(const char *result,const char *relative,int preserve_existing,int follow_final,const char **out) {
    size_t root=strlen(user_root)+1;
    char prefix[PATH_MAX]; strcpy(prefix,result);
    for(size_t i=root;;++i){
        char saved=prefix[i];
        if(saved!='/' && saved)continue;
        if(!saved && !follow_final)return 0;
        prefix[i]=0;
        struct stat info;
        if(lstat(prefix,&info))return 0;
        if(S_ISLNK(info.st_mode)){
            char target[PATH_MAX]; ssize_t length=readlink(prefix,target,sizeof(target)-1);
            if(length<0){*out=NULL;return 1;}
            target[length]=0;
            size_t component=i-root; // longueur de la partie relative jusqu'au lien
            const char *slash=NULL; for(size_t j=0;j<component;++j)if(relative[j]=='/')slash=relative+j;
            char next[PATH_MAX]; int count;
            if(target[0]=='/')count=snprintf(next,PATH_MAX,"%s%s",target,result+i);
            else count=snprintf(next,PATH_MAX,"%s/%.*s%s%s%s",game_root,slash?(int)(slash-relative):0,relative,
                slash?"/":"",target,result+i);
            if(count<0 || count>=PATH_MAX){errno=ENAMETOOLONG;*out=NULL;return 1;}
            if(++link_depth>40){link_depth=0;errno=ELOOP;*out=NULL;return 1;}
            *out=preserve_existing<0?rt_read_path(next):write_path(next,preserve_existing,follow_final);
            link_depth=0;
            return 1;
        }
        if(!saved)return 0;
        prefix[i]=saved;
    }
}
// Génération incrémentée aussi en sortie : write_path résout l'original en
// lecture (copie préalable), ce qui pourrait sinon rester en cache alors que la
// copie utilisateur vient d'être créée. Le fichier lui-même est créé ensuite
// par le même shim, verrou d'exécution invité tenu.
const char *rt_write_path(const char *path,int preserve_existing) {
    const char *result=write_path(path,preserve_existing,1); ++generation; return result;
}
const char *rt_write_path_nofollow(const char *path) { const char *result=write_path(path,0,0); ++generation; return result; }
static const char *write_path(const char *path,int preserve_existing,int follow_final) {
    ++generation;   // toute écriture invalide les caches de lecture (listes de dossiers)
    if(!game_root[0])return path;
    char canonical[PATH_MAX]; if(absolute_path(path,canonical))return NULL;
    if(!within(canonical,game_root)) {
#if TARGET_OS_IPHONE
        if(within(canonical,linux_root))return rt_ios_case_path(canonical,strlen(linux_root));
        if(within(canonical,user_root))return rt_ios_case_path(canonical,strlen(user_root));
#endif
        return path;
    }
    const char *relative=canonical+strlen(game_root); while(*relative=='/')++relative;
    if(reserved(relative)){errno=ENOENT;return NULL;}
    char *result=buffer(); int count=snprintf(result,PATH_MAX,"%s/%s",user_root,relative);
    if(count<0 || count>=PATH_MAX){errno=ENAMETOOLONG;return NULL;}
#if TARGET_OS_IPHONE
    // Le moteur écrit en minuscules (« portal2/save/… ») : composants existants
    // de la copie utilisateur repris dans leur casse réelle (« SAVE »), sinon
    // ils seraient recréés ou introuvables sur un système sensible à la casse.
    {const char *c=rt_ios_case_path(result,strlen(user_root)); if(c && c!=result)strcpy(result,c);}
#endif
    const char *linked;
    if(*relative && overlay_link(result,relative,preserve_existing,follow_final,&linked))return linked;
    // Ne recrée dans la copie utilisateur que les répertoires parents visibles
    // dans la vue fusionnée : sinon ENOENT, comme sous Linux.
    for(char *p=result+strlen(user_root)+1;*p;++p)if(*p=='/'){
        *p=0;
        struct stat info;
        if(stat(result,&info)) {
            const char *parent=relative_parent(relative,(size_t)(p-(result+strlen(user_root)+1)));
            if(!parent || !lower_directory(parent)){*p='/';errno=ENOENT;return NULL;}
            if(mkdir(result,0755) && errno!=EEXIST){*p='/';return NULL;}
        } else if(!S_ISDIR(info.st_mode)){*p='/';errno=ENOTDIR;return NULL;}
        *p='/';
    }
    if(preserve_existing && access(result,F_OK)){
        const char *existing=rt_read_path(canonical);
        FILE *input=existing?fopen(existing,"rb"):NULL;
        if(input){
            FILE *output=fopen(result,"wb");
            if(!output){fclose(input);return NULL;}
            char block[65536];size_t size;int failed=0;
            while((size=fread(block,1,sizeof(block),input)))if(fwrite(block,1,size,output)!=size){failed=1;break;}
            if(ferror(input))failed=1;
            fclose(input);if(fclose(output))failed=1;
            if(failed)return NULL;
        }
    }
    return result;
}
const char *rt_program_path(void) {
    char *result=buffer();snprintf(result,PATH_MAX,"%s/portal2_linux",game_root);return result;
}
