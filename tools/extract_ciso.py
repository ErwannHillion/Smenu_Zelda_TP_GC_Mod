#!/usr/bin/env python3
"""
Extract a GameCube CISO to a Dolphin-bootable directory (streaming, low memory).
Usage: python3 tools/extract_ciso.py <input.ciso> <output_dir>
"""

import struct
import sys
import os


class CISOReader:
    """Read from a CISO as if it were a regular ISO, with block-level seeking."""

    def __init__(self, path):
        self.f = open(path, "rb")
        magic = self.f.read(4)
        if magic != b"CISO":
            raise ValueError("Not a CISO file")

        bs_raw = self.f.read(4)
        self.block_size = struct.unpack("<I", bs_raw)[0]
        # Verify with file size
        self.block_map = self.f.read(0x8000 - 8)
        self.total_blocks = len(self.block_map)

        # Build lookup: iso_block_idx -> physical_block_idx
        self.block_phys = {}
        phys = 0
        for i in range(self.total_blocks):
            if self.block_map[i] == 1:
                self.block_phys[i] = phys
                phys += 1

        self.used_blocks = phys
        print(f"CISO: block_size=0x{self.block_size:X} used={self.used_blocks}/{self.total_blocks}")

    def read_at(self, iso_offset, size):
        """Read `size` bytes from ISO offset, handling block boundaries."""
        result = bytearray()
        remaining = size

        while remaining > 0:
            block_idx = iso_offset // self.block_size
            offset_in_block = iso_offset % self.block_size
            chunk_size = min(remaining, self.block_size - offset_in_block)

            if block_idx in self.block_phys:
                phys_offset = 0x8000 + self.block_phys[block_idx] * self.block_size + offset_in_block
                self.f.seek(phys_offset)
                data = self.f.read(chunk_size)
                result.extend(data)
            else:
                # Block not in CISO = zeros
                result.extend(b"\x00" * chunk_size)

            iso_offset += chunk_size
            remaining -= chunk_size

        return bytes(result)

    def close(self):
        self.f.close()


def get_dol_size_from_bytes(header):
    max_end = 0x100
    for i in range(7):
        off = struct.unpack(">I", header[i * 4: 4 + i * 4])[0]
        size = struct.unpack(">I", header[0x90 + i * 4: 0x94 + i * 4])[0]
        if off + size > max_end:
            max_end = off + size
    for i in range(11):
        off = struct.unpack(">I", header[0x1C + i * 4: 0x20 + i * 4])[0]
        size = struct.unpack(">I", header[0xAC + i * 4: 0xB0 + i * 4])[0]
        if off + size > max_end:
            max_end = off + size
    return max_end


def write_file(path, data):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "wb") as f:
        f.write(data)
    return len(data)


def extract(ciso_path, outdir):
    reader = CISOReader(ciso_path)

    # Read disc header
    disc_header = reader.read_at(0, 0x460)
    game_code = disc_header[0:4].decode("ascii")
    game_name = disc_header[0x20:0x400].split(b"\x00")[0].decode("ascii")
    dol_offset = struct.unpack(">I", disc_header[0x420:0x424])[0]
    fst_offset = struct.unpack(">I", disc_header[0x424:0x428])[0]
    fst_size = struct.unpack(">I", disc_header[0x428:0x42C])[0]

    print(f"Game: {game_name} ({game_code})")
    print(f"DOL: 0x{dol_offset:X}, FST: 0x{fst_offset:X} ({fst_size} bytes)")

    # Apploader
    app_header = reader.read_at(0x2440, 0x20)
    app_size = struct.unpack(">I", app_header[0x14:0x18])[0]
    app_trailer = struct.unpack(">I", app_header[0x18:0x1C])[0]
    app_total = 0x20 + app_size + app_trailer

    # DOL
    dol_header = reader.read_at(dol_offset, 0x100)
    dol_size = get_dol_size_from_bytes(dol_header)

    # Extract sys/
    sys_dir = os.path.join(outdir, "sys")
    files_dir = os.path.join(outdir, "files")
    os.makedirs(sys_dir, exist_ok=True)
    os.makedirs(files_dir, exist_ok=True)

    print("\n--- Extracting sys/ ---")
    write_file(os.path.join(sys_dir, "boot.bin"), disc_header[:0x440])
    print("  boot.bin")
    write_file(os.path.join(sys_dir, "bi2.bin"), reader.read_at(0x440, 0x2000))
    print("  bi2.bin")
    write_file(os.path.join(sys_dir, "apploader.img"), reader.read_at(0x2440, app_total))
    print(f"  apploader.img ({app_total} bytes)")
    write_file(os.path.join(sys_dir, "main.dol"), reader.read_at(dol_offset, dol_size))
    print(f"  main.dol ({dol_size} bytes)")

    # Extract files/ from FST
    print("\n--- Extracting files/ ---")
    fst_data = reader.read_at(fst_offset, fst_size)
    num_entries = struct.unpack(">I", fst_data[8:12])[0]
    str_table = num_entries * 12
    print(f"FST: {num_entries} entries")

    # Track directory stack
    dir_stack = [("", num_entries)]  # (path, end_index)
    file_count = 0
    warn_count = 0

    for i in range(1, num_entries):
        # Pop finished directories
        while dir_stack and i >= dir_stack[-1][1]:
            dir_stack.pop()

        entry = fst_data[i * 12: (i + 1) * 12]
        is_dir = entry[0] == 1
        name_off = struct.unpack(">I", entry[0:4])[0] & 0x00FFFFFF
        name_end = fst_data.index(0, str_table + name_off)
        name = fst_data[str_table + name_off: name_end].decode("ascii", errors="replace")

        parent_path = dir_stack[-1][0] if dir_stack else ""

        if is_dir:
            next_idx = struct.unpack(">I", entry[8:12])[0]
            dir_path = os.path.join(parent_path, name)
            os.makedirs(os.path.join(files_dir, dir_path), exist_ok=True)
            dir_stack.append((dir_path, next_idx))
        else:
            file_off = struct.unpack(">I", entry[4:8])[0]
            file_sz = struct.unpack(">I", entry[8:12])[0]

            rel_path = os.path.join(parent_path, name)
            full_path = os.path.join(files_dir, rel_path)
            os.makedirs(os.path.dirname(full_path), exist_ok=True)

            data = reader.read_at(file_off, file_sz)
            write_file(full_path, data)
            file_count += 1

            if file_count % 100 == 0:
                print(f"  {file_count} files extracted...")

    print(f"  Total: {file_count} files")

    reader.close()
    print(f"\nDone! Dolphin-bootable directory: {outdir}/")
    print(f"Open Dolphin > File > Open > select {outdir}/sys/main.dol")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.ciso> <output_dir>")
        sys.exit(1)
    extract(sys.argv[1], sys.argv[2])
