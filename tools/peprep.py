#!/usr/bin/env python3
"""PE32 images (Miles .asi plugins): headers, sections, exports, imports and
base relocations, for ahead-of-time translation.

On Linux, libMiles loads the Windows plugins of bin/linux32 with its own PE
loader: it picks the address at run time, copies the sections to their RVA and
applies the .reloc table. The translated code is therefore relocatable: every
word covered by a relocation is `file_value + delta` at run time (delta =
actual base - ImageBase), and functions are indexed by RVA (see
lift.generate_pe, runtime/pe.c).
"""
from pathlib import Path
import struct

IMAGE_SCN_CNT_CODE = 0x20
IMAGE_SCN_MEM_EXECUTE = 0x20000000
IMAGE_REL_BASED_ABSOLUTE, IMAGE_REL_BASED_HIGHLOW = 0, 3


class PeError(ValueError):
    pass


def fnv1a64(data):
    """File hash, recomputed by runtime/pe.c (rt_pe_hash)."""
    value = 0xcbf29ce484222325
    for byte in data:
        value = ((value ^ byte) * 0x100000001b3) & 0xffffffffffffffff
    return value


class PeImage:
    def __init__(self, path=None, data=None, name=None):
        if data is None:
            data = Path(path).read_bytes()
        self.path, self.data = path, bytes(data)
        self.name = name or (str(path).rsplit('/', 1)[-1] if path else 'image')
        d = self.data
        if d[:2] != b'MZ' or len(d) < 0x40:
            raise PeError('MZ header expected')
        pe = struct.unpack_from('<I', d, 0x3c)[0]
        if d[pe:pe + 4] != b'PE\0\0':
            raise PeError('PE signature expected')
        machine, section_count, _, _, _, optional_size, characteristics = struct.unpack_from('<HHIIIHH', d, pe + 4)
        if machine != 0x14c:
            raise PeError(f'i386 image expected, machine {machine:#x}')
        if not characteristics & 0x2000:
            raise PeError('DLL expected')
        opt = pe + 24
        if struct.unpack_from('<H', d, opt)[0] != 0x10b:
            raise PeError('PE32 optional header expected')
        self.entry_rva = struct.unpack_from('<I', d, opt + 16)[0]
        self.image_base, self.section_alignment, self.file_alignment = struct.unpack_from('<III', d, opt + 28)
        self.image_size, self.headers_size = struct.unpack_from('<II', d, opt + 56)
        directory_count = struct.unpack_from('<I', d, opt + 92)[0]
        self.directories = [struct.unpack_from('<II', d, opt + 96 + 8 * i) for i in range(min(directory_count, 16))]
        self.sections = []
        for i in range(section_count):
            o = opt + optional_size + 40 * i
            name = d[o:o + 8].rstrip(b'\0').decode('latin-1')
            virtual_size, rva, raw_size, raw_offset = struct.unpack_from('<IIII', d, o + 8)
            flags = struct.unpack_from('<I', d, o + 36)[0]
            self.sections.append(dict(name=name, rva=rva, size=virtual_size or raw_size,   # VirtualSize is 0 in some images
                                      raw_offset=raw_offset, raw_size=raw_size, flags=flags,
                                      executable=bool(flags & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE))))
        # Image telle qu'un chargeur la projette (sections à leur RVA, reste nul).
        self.mapped = bytearray(self.image_size)
        self.mapped[:min(self.headers_size, len(d))] = d[:self.headers_size]
        for s in self.sections:
            count = min(s['size'], s['raw_size'])
            if s['rva'] + s['size'] > self.image_size or s['raw_offset'] + count > len(d):
                raise PeError(f'section {s["name"]} outside the image')
            self.mapped[s['rva']:s['rva'] + count] = d[s['raw_offset']:s['raw_offset'] + count]
        self.relocs = self._relocations()
        self.exports = self._exports()
        self.imports = self._imports()

    def directory(self, index):
        return self.directories[index] if index < len(self.directories) else (0, 0)

    def u32(self, rva):
        return struct.unpack_from('<I', self.mapped, rva)[0]

    def u16(self, rva):
        return struct.unpack_from('<H', self.mapped, rva)[0]

    def cstr(self, rva):
        end = self.mapped.index(b'\0', rva)
        return self.mapped[rva:end].decode('latin-1')

    def _relocations(self):
        rva, size = self.directory(5)
        out, o = [], rva
        while o + 8 <= rva + size:
            page, block = struct.unpack_from('<II', self.mapped, o)
            if block < 8:
                raise PeError(f'invalid relocation block at {o:#x}')
            for i in range((block - 8) // 2):
                entry = self.u16(o + 8 + 2 * i)
                kind, offset = entry >> 12, entry & 0xfff
                if kind == IMAGE_REL_BASED_HIGHLOW:
                    out.append(page + offset)
                elif kind != IMAGE_REL_BASED_ABSOLUTE:
                    raise PeError(f'relocation type {kind} at {page + offset:#x} is not supported')
            o += block
        if any(r + 4 > self.image_size for r in out):
            raise PeError('relocation outside the image')
        return sorted(set(out))

    def _exports(self):
        rva, size = self.directory(0)
        if not rva:
            return {}
        base, function_count, name_count, functions, names, ordinals = struct.unpack_from('<IIIIII', self.mapped, rva + 16)
        exports = {}
        named = set()
        for i in range(name_count):
            name = self.cstr(self.u32(names + 4 * i))
            index = self.u16(ordinals + 2 * i)
            target = self.u32(functions + 4 * index)
            named.add(index)
            if not rva <= target < rva + size:   # pas un renvoi vers une autre DLL
                exports[name] = target
        for index in range(function_count):
            target = self.u32(functions + 4 * index)
            if index not in named and target and not rva <= target < rva + size:
                exports[f'#{base + index}'] = target
        return exports

    def _imports(self):
        """[(dll, name or #ordinal, RVA of the IAT entry)]: filled in by the Miles loader."""
        rva, _ = self.directory(1)
        out = []
        while rva:
            lookup, _, _, name, iat = struct.unpack_from('<IIIII', self.mapped, rva)
            if not (lookup or name or iat):
                break
            dll = self.cstr(name)
            table, i = lookup or iat, 0
            while self.u32(table + 4 * i):
                value = self.u32(table + 4 * i)
                symbol = f'#{value & 0xffff}' if value & 0x80000000 else self.cstr(value + 2)
                out.append((dll, symbol, iat + 4 * i))
                i += 1
            rva += 20
        return out

    def executable_sections(self):
        return [s for s in self.sections if s['executable']]

    def executable(self, address):
        """`address` is an address at the preferred ImageBase."""
        rva = address - self.image_base
        return any(s['rva'] <= rva < s['rva'] + s['size'] for s in self.executable_sections())

    def code_target(self, value):
        """Value (at the preferred ImageBase) that libMiles relocates to the
        executable view: RAD_dll_fixup_base_address only considers the section
        named .text; everything else points into the writable view."""
        rva = value - self.image_base
        return any(s['name'] == '.text' and s['rva'] <= rva < s['rva'] + s['size'] for s in self.sections)

    def code_pointers(self):
        """Relocated values (addresses at the preferred ImageBase) that point to
        code, with the relocation's location: {(location RVA, value)}."""
        out = []
        for r in self.relocs:
            value = self.u32(r)
            if self.executable(value):
                out.append((r, value))
        return out

    def summary(self):
        return dict(name=self.name, image_base=self.image_base, image_size=self.image_size,
                    entry_rva=self.entry_rva, section_alignment=self.section_alignment,
                    sections=[{k: s[k] for k in ('name', 'rva', 'size', 'flags', 'executable')} for s in self.sections],
                    exports=self.exports, imports=self.imports, relocations=len(self.relocs),
                    file_size=len(self.data), file_hash=fnv1a64(self.data))


if __name__ == '__main__':
    import json
    import sys
    for argument in sys.argv[1:]:
        print(json.dumps(PeImage(argument).summary(), indent=1))
