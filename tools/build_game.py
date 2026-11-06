#!/usr/bin/env python3
"""Build the real Linux game as one native ARM64 executable.

By default every known function of every prepared module is a root (symbols,
exports, init/fini arrays, code pointers in data, see lift.module_roots), in
addition to `main`, the foundation constructors and config/game-roots.json.
The C is generated in files grouped by guest address (lift.generate_sharded),
compiled in parallel, and each object is cached by the hash of its source,
included headers, compiler and flags: an unchanged file is never recompiled.
Generated code is AOT C, with traps for instructions/imports not yet ported.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time
import lift
import localconfig
import peprep

# Identique à HOST_CFLAGS/HOST_LIBS du Makefile (tools/bench.py les importe d'ici) ;
# préfixe Homebrew ou bundle précompilé, voir localconfig.host_prefix.
HOST_CFLAGS, HOST_LIBS = localconfig.host_flags()
WARNINGS = ['-Wall', '-Wextra', '-Werror']
DEBUG = {'full': '-g', 'lines': '-gline-tables-only', 'none': '-g0'}
# Scripts whose content enters the generation keys (a change regenerates the C).
GEN_TOOLS = ['tools/lift.py', 'tools/float_lift.py', 'tools/registry.py']
PE_TOOLS = GEN_TOOLS + ['tools/peprep.py']
# Greffons Windows que libMiles charge lui-même depuis bin/linux32 de la copie
# du jeu (runtime/pe.c) : mssmp3 décode les voix MP3, mssvoice le chat vocal,
# binkawin l'audio des vidéos Bink.
PE_PLUGINS = ('mssmp3.asi', 'mssvoice.asi', 'binkawin.asi')
KEEP_CONFIGURATIONS = 2   # objets conservés pour les deux derniers jeux d'options


def roots(layout):
    result = ['portal2_linux:symbol:main']
    for module in layout['modules']:
        if module['name'] in ('libtier0.so', 'libvstdlib.so', 'portal2_linux'):
            if module['init']: result.append(f'{module["name"]}:init:dt')
            result += [f'{module["name"]}:init:{i}' for i in range(len(module['init_array']))]
    extra = Path('config/game-roots.json')
    if extra.exists(): result += json.loads(extra.read_text())
    return list(dict.fromkeys(result))


def digest(*parts):
    h = hashlib.sha256()
    for part in parts:
        h.update(part if isinstance(part, bytes) else str(part).encode())
        h.update(b'\0')
    return h.hexdigest()


def generation_key(layout_path, layout, specs, whole_game):
    images = [(m['image'], Path(m['image']).stat().st_size, Path(m['image']).stat().st_mtime_ns)
              for m in layout['modules']]
    objdump = shutil.which('objdump') or 'objdump'
    return digest(*(Path(t).read_bytes() for t in GEN_TOOLS), layout_path.read_bytes(), images, specs,
                  whole_game, objdump, Path(objdump).stat().st_mtime_ns if Path(objdump).exists() else 0,
                  lift.SHARD_BITS, 'v1')


def generate(args, layout, specs):
    out = Path('build/game/gen')
    manifest_path = out / 'manifest.json'
    key = generation_key(args.layout, layout, specs, args.all)
    manifest = read_manifest(manifest_path)
    sources = out / 'src'
    if manifest.get('key') == key and all((sources / n).exists() for n in manifest['shards']) \
            and (sources / 'dispatch.c').exists():
        print(f'generated C up to date: {manifest["functions"]} functions, '
              f'{manifest["instructions"]} instructions, {len(manifest["shards"])} files')
        return manifest, 0.0
    started = time.time()
    manifest = lift.generate_sharded(layout, specs, args.all, 'objdump', Path('build/disasm-cache'), out, args.jobs)
    manifest['key'] = key
    lift._write_atomic(manifest_path, json.dumps(manifest).encode())   # never a truncated manifest
    return manifest, time.time() - started


def read_manifest(path):
    """The previous generation's manifest; {} when absent or unreadable (interrupted write)."""
    try:
        manifest = json.loads(path.read_text())
    except (OSError, ValueError):
        return {}
    return manifest if isinstance(manifest, dict) and isinstance(manifest.get('shards'), dict) else {}


def generate_plugins(config_path, log=print):
    """Relocatable C of the PE plugins in bin/linux32 (lift.generate_pe), in
    build/game/gen/pe: one file per plugin, rewritten only when it changes."""
    out = Path('build/game/gen/pe')
    out.mkdir(parents=True, exist_ok=True)
    config = json.loads(Path(config_path).read_text()) if Path(config_path).exists() else {}
    root = Path(config.get('game_root', '')) / 'bin/linux32'
    sources, stats = [], []
    for name in PE_PLUGINS:
        path = root / name
        if not config.get('game_root') or not path.is_file():
            log(f'PE plugin {name} not found under {root}: Miles will not see it')
            continue
        tag = name.split('.')[0].lower()
        target = out / f'pe_{tag}.c'
        key = digest(*(Path(t).read_bytes() for t in PE_TOOLS), path.read_bytes(), 'pe-v1')
        stamp = out / f'pe_{tag}.key'
        if not (target.exists() and stamp.exists() and stamp.read_text() == key):
            source, info = lift.generate_pe(peprep.PeImage(path), tag, log=log)
            target.write_text(source)
            stamp.write_text(key)
            (out / f'pe_{tag}.json').write_text(json.dumps(info, indent=1) + '\n')
        stats.append(json.loads((out / f'pe_{tag}.json').read_text()))
        sources.append(target)
    for stale in out.glob('pe_*.c'):
        if stale not in sources:
            stale.unlink()
    return sources, stats


INCLUDE = re.compile(rb'^\s*#\s*include\s+"([^"]+)"', re.M)


class SourceIndex:
    """Content digest and quoted includes of each file, recomputed only when
    its size or modification time changes (rereading ~1 GB of generated C at
    every build would cost seconds)."""

    def __init__(self, path):
        self.path = path
        self.known = json.loads(path.read_text()) if path.exists() else {}
        self.current = {}

    def entry(self, path):
        key = str(path)
        if key not in self.current:
            info = Path(path).stat()
            stamp = f'{info.st_size}:{info.st_mtime_ns}'
            cached = self.known.get(key)
            if not cached or cached[0] != stamp:
                data = Path(path).read_bytes()
                cached = [stamp, digest(data), [n.decode() for n in INCLUDE.findall(data)]]
            self.current[key] = cached
        return self.current[key]

    def closure(self, source, directories):
        """Digest of the source and of every quoted include reachable from it."""
        seen, pending = {}, [Path(source)]
        while pending:
            path = pending.pop()
            _, value, includes = self.entry(path)
            seen[str(path)] = value
            for name in includes:
                for directory in [path.parent, *directories]:
                    candidate = (directory / name).resolve()
                    if candidate.exists():
                        if str(candidate) not in seen:
                            pending.append(candidate)
                        break
        return [seen.pop(str(Path(source)))] + [f'{p}:{v}' for p, v in sorted(seen.items())]

    def save(self):
        self.known.update(self.current)
        self.path.write_text(json.dumps(self.known))


def compile_units(units, cache, jobs, compiler_identity):
    """units: [(source, flags)] -> ([object], compiled count, cpu seconds)."""
    cache.mkdir(parents=True, exist_ok=True)
    index = SourceIndex(cache / 'source-digests.json')
    planned, objects = [], []
    for source, flags in units:
        key = digest(compiler_identity, flags, *index.closure(source, [Path('runtime')]))
        obj = cache / f'{key}.o'
        objects.append(obj)
        if not obj.exists():
            planned.append((source, flags, obj))
    index.save()
    # Les plus gros d'abord : le pool reste occupé jusqu'au bout.
    planned.sort(key=lambda item: -Path(item[0]).stat().st_size)

    def run(item):
        source, flags, obj = item
        temporary = obj.with_name(f'.{obj.name}.{os.getpid()}.tmp')
        started = time.time()
        result = subprocess.run(['clang', '-c', *flags, str(source), '-o', str(temporary)],
                                capture_output=True, text=True)
        if result.returncode:
            temporary.unlink(missing_ok=True)
            return source, result.stderr, time.time() - started
        os.replace(temporary, obj)
        return source, None, time.time() - started

    seconds, failures = 0.0, []
    with ThreadPoolExecutor(max_workers=jobs) as pool:
        for source, error, elapsed in pool.map(run, planned):
            seconds += elapsed
            if error:
                failures.append(f'{source}:\n{error[:4000]}')
    if failures:
        raise SystemExit('compilation failed:\n' + '\n'.join(failures[:5]))
    return objects, len(planned), seconds


def collect_garbage(cache, configuration, objects):
    """Keep the objects of the most recent option sets only (disk space)."""
    manifests = cache / 'configurations.json'
    known = json.loads(manifests.read_text()) if manifests.exists() else []
    known = [k for k in known if k['configuration'] != configuration]
    known.append(dict(configuration=configuration, objects=sorted(o.name for o in objects)))
    known = known[-KEEP_CONFIGURATIONS:]
    manifests.write_text(json.dumps(known))
    live = {name for k in known for name in k['objects']}
    removed = 0
    for obj in cache.glob('*.o'):
        if obj.name not in live:
            removed += obj.stat().st_size
            obj.unlink()
    return removed


def disk_usage(path):
    return sum(p.stat().st_size for p in Path(path).rglob('*') if p.is_file())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--layout', type=Path, default=Path('build/game/prep/layout.json'))
    parser.add_argument('--out', type=Path, default=Path('build/portal2-native'))
    parser.add_argument('--opt', default='1', choices=['0', '1', '2', '3'], help='generated code')
    parser.add_argument('--runtime-opt', choices=['0', '1', '2', '3'], help='runtime and main (default: --opt)')
    parser.add_argument('--debug', default='lines', choices=sorted(DEBUG),
                        help='debug info of generated code (runtime is always -g)')
    scope = parser.add_mutually_exclusive_group()
    scope.add_argument('--all', dest='all', action='store_true', default=True,
                       help='every known function of every module is a root (default)')
    scope.add_argument('--roots-only', dest='all', action='store_false',
                       help='only main, foundation constructors and config/game-roots.json')
    parser.add_argument('--fast-memory', action='store_true',
                        help='no software check of guest memory accesses (RT_FAST_MEMORY): faster, but only '
                             'the 16 KiB host pages protect, see rt.h; by default, checks at the 4 KiB '
                             'Linux page grain')
    parser.add_argument('--traced-calls', '--debug-traces', dest='traced_calls', action='store_true',
                        help='every guest call through rt_call: full guest stack in diagnostics and '
                             'P2_WATCH_CALL (default: direct calls, see rt.h)')
    parser.add_argument('--release', action='store_true',
                        help='build to play: --opt 2 --fast-memory (measured ~200 fps in game against '
                             '~117 checked at -O1); keep the checked build for diagnosis')
    pgo = parser.add_mutually_exclusive_group()
    pgo.add_argument('--pgo-generate', action='store_true',
                     help='instrumented build (LLVM profile written every 20 s to LLVM_PROFILE_FILE), '
                          'without debug info to keep the objects small')
    pgo.add_argument('--pgo-use', type=Path, metavar='PROFDATA',
                     help='profile-guided build from a merged profile (llvm-profdata merge)')
    parser.add_argument('--order-file', type=Path,
                        help='linker order file (tools/hot_order.py): hot code grouped together')
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    parser.add_argument('--stats', type=Path, default=Path('build/game/build-stats.json'))
    parser.add_argument('--config', type=Path, default=Path('config/local.json'),
                        help='game_root: game copy holding bin/linux32/*.asi (PE plugins)')
    args = parser.parse_args()
    if args.release:
        args.opt, args.fast_memory = '2', True
    sys.stdout.reconfigure(line_buffering=True)
    started = time.time()
    layout = json.loads(args.layout.read_text())
    try:
        manifest, generation_time = generate(args, layout, roots(layout))
    except lift.Unsupported as exc:
        parser.exit(1, f'game lift: {exc}\n')
    plugin_sources, plugin_stats = generate_plugins(args.config)
    imports = lift.import_map(layout)
    unimplemented = sorted({imports[a] for a in manifest['used_imports']} - lift.SHIMS)
    if unimplemented:
        print('Runtime traps for unimplemented imports: ' + ', '.join(unimplemented))
    identity = subprocess.run(['clang', '--version'], capture_output=True, text=True, check=True).stdout
    memory = (['-DRT_FAST_MEMORY'] if args.fast_memory else []) + (['-DRT_TRACED_CALLS'] if args.traced_calls else [])
    profile = []
    if args.pgo_generate:
        profile = ['-fprofile-instr-generate', '-DRT_PGO_GENERATE']
        args.debug = 'none'
    elif args.pgo_use:
        # Le contenu du profil entre dans la clé des objets (même chemin, autre profil).
        identity += digest(args.pgo_use.read_bytes())
        profile = [f'-fprofile-instr-use={args.pgo_use.resolve()}', '-Wno-profile-instr-unprofiled',
                   '-Wno-profile-instr-out-of-date', '-Wno-profile-instr-missing']
    runtime_flags = [f'-O{args.runtime_opt or args.opt}', '-g', *WARNINGS, *memory, *profile, '-Iruntime', *HOST_CFLAGS]
    generated_flags = [f'-O{args.opt}', DEBUG[args.debug], *WARNINGS, *memory, *profile, '-Iruntime']
    sources = Path('build/game/gen/src')
    units = [(str(p), runtime_flags) for p in sorted(Path('runtime').glob('*.c'))]
    units.append(('src/main.c', runtime_flags))
    units += [(str(sources / name), generated_flags) for name in sorted(manifest['shards'])]
    units.append((str(sources / 'dispatch.c'), generated_flags))
    units += [(str(p), generated_flags) for p in plugin_sources]
    # Objets instrumentés à part : supprimables d'un bloc une fois le profil extrait.
    cache = Path('build/game/obj-pgo-generate' if args.pgo_generate else 'build/game/obj')
    compile_started = time.time()
    objects, compiled, cpu = compile_units(units, cache, args.jobs, identity)
    compile_time = time.time() - compile_started
    link_started = time.time()
    response = Path('build/game/link.rsp')
    response.write_text('\n'.join(str(o) for o in objects) + '\n')
    temporary = args.out.with_name(f'.{args.out.name}.tmp')
    link_flags = (['-fprofile-instr-generate'] if args.pgo_generate else []) + \
        ([f'-Wl,-order_file,{args.order_file}'] if args.order_file else [])
    subprocess.run(['clang', f'@{response}', *HOST_LIBS, *link_flags, '-o', str(temporary)], check=True)
    os.replace(temporary, args.out)
    link_time = time.time() - link_started
    removed = collect_garbage(cache, digest(runtime_flags, generated_flags), objects)
    stats = dict(functions=manifest['functions'], instructions=manifest['instructions'],
                 roots=manifest['roots'], files=len(manifest['shards']) + 1, c_bytes=manifest['c_bytes'],
                 generation_seconds=round(generation_time, 1), compiled_units=compiled, total_units=len(units),
                 compile_seconds=round(compile_time, 1), compile_cpu_seconds=round(cpu, 1),
                 link_seconds=round(link_time, 1), total_seconds=round(time.time() - started, 1),
                 binary_bytes=args.out.stat().st_size, object_cache_bytes=disk_usage(cache),
                 generated_source_bytes=disk_usage(sources), removed_object_bytes=removed,
                 opt=args.opt, runtime_opt=args.runtime_opt or args.opt, debug=args.debug, whole_game=args.all,
                 fast_memory=args.fast_memory, traced_calls=args.traced_calls,
                 pe_plugins=[{k: v for k, v in item.items() if k != 'not_compiled'} for item in plugin_stats])
    args.stats.write_text(json.dumps(stats, indent=1) + '\n')
    print(f'{compiled}/{len(units)} files compiled in {compile_time:.0f} s ({cpu:.0f} s CPU), '
          f'link {link_time:.0f} s, total {stats["total_seconds"]:.0f} s; binary '
          f'{stats["binary_bytes"] / 1e6:.0f} MB, objects {stats["object_cache_bytes"] / 1e9:.2f} GB, '
          f'C {stats["generated_source_bytes"] / 1e9:.2f} GB')


if __name__ == '__main__':
    main()
