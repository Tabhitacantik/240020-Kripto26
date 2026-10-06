
# Nama    : Kezia Tabhita Smith
# NPM     : 140810240020
# Kelas   : Kriptografi A
# Tanggal : 15 September 2026

import argparse
import math
import os
import struct
import sys

import numpy as np
from PIL import Image

MAGIC = b"LSBS"
TYPE_TEXT, TYPE_FILE = 0, 1
LOSSLESS = (".png", ".bmp")


def load_pixels(path):
    img = Image.open(path).convert("RGB")
    arr = np.array(img, dtype=np.uint8)
    return arr.shape, arr.flatten()


def save_pixels(shape, flat, path):
    if not path.lower().endswith(LOSSLESS):
        sys.exit("Output harus .png atau .bmp (lossless). JPG akan merusak bit LSB.")
    Image.fromarray(flat.reshape(shape), "RGB").save(path)


def positions(n_bytes, n_needed, mode, key):
    """Urutan byte citra yang dipakai untuk menyimpan bit pesan."""
    if mode == "sequential":
        return np.arange(n_needed)
    # mode random: seed PRNG = stego-key
    rng = np.random.default_rng(key)
    return rng.permutation(n_bytes)[:n_needed]


def build_payload(text, file_path):
    if text is not None:
        name, data, tp = b"", text.encode("utf-8"), TYPE_TEXT
    else:
        with open(file_path, "rb") as f:
            data = f.read()
        name, tp = os.path.basename(file_path).encode("utf-8")[:255], TYPE_FILE
    return MAGIC + bytes([tp, len(name)]) + name + struct.pack(">I", len(data)) + data


def psnr(a, b):
    mse = np.mean((a.astype(np.float64) - b.astype(np.float64)) ** 2)
    return float("inf") if mse == 0 else 10 * math.log10(255 ** 2 / mse)


# ------------------------------------------------------------------ encode
def encode(cover, output, text, file_path, m, mode, key):
    shape, flat = load_pixels(cover)
    payload = build_payload(text, file_path)

    bits = np.unpackbits(np.frombuffer(payload, dtype=np.uint8))
    pad = (-len(bits)) % m
    bits = np.concatenate([bits, np.zeros(pad, dtype=np.uint8)])
    groups = bits.reshape(-1, m)
    weights = 1 << np.arange(m - 1, -1, -1)
    values = (groups * weights).sum(axis=1).astype(np.uint8)

    capacity = len(flat) * m // 8
    if len(values) > len(flat):
        sys.exit(f"Pesan terlalu besar! Butuh {len(payload)} byte, "
                 f"kapasitas citra {capacity} byte (dengan {m}-bit LSB).")

    idx = positions(len(flat), len(values), mode, key)
    mask = np.uint8((0xFF << m) & 0xFF)
    stego = flat.copy()
    stego[idx] = (stego[idx] & mask) | values
    save_pixels(shape, stego, output)

    print(f"[+] Berhasil! Stego-image disimpan: {output}")
    print(f"    Ukuran payload : {len(payload)} byte  (kapasitas {capacity} byte)")
    print(f"    Mode / m-bit   : {mode} / {m}")
    print(f"    PSNR           : {psnr(flat, stego):.2f} dB")


# ------------------------------------------------------------------ decode
def decode(stego, outdir, m, mode, key):
    shape, flat = load_pixels(stego)
    idx = positions(len(flat), len(flat), mode, key)
    vals = flat[idx] & np.uint8((1 << m) - 1)
    bits = ((vals[:, None] >> np.arange(m - 1, -1, -1)) & 1).astype(np.uint8).flatten()
    data = np.packbits(bits[: len(bits) // 8 * 8]).tobytes()

    if data[:4] != MAGIC:
        sys.exit("[-] Tidak ditemukan pesan (magic salah). Cek m-bit, mode, dan key.")
    tp, namelen = data[4], data[5]
    name = data[6:6 + namelen].decode("utf-8", errors="replace")
    off = 6 + namelen
    (length,) = struct.unpack(">I", data[off:off + 4])
    secret = data[off + 4: off + 4 + length]
    if len(secret) != length:
        sys.exit("[-] Data korup / parameter salah.")

    if tp == TYPE_TEXT:
        print("[+] Pesan teks tersembunyi:\n")
        print(secret.decode("utf-8", errors="replace"))
    else:
        os.makedirs(outdir, exist_ok=True)
        out = os.path.join(outdir, "extracted_" + os.path.basename(name))
        with open(out, "wb") as f:
            f.write(secret)
        print(f"[+] File tersembunyi diekstrak: {out} ({length} byte)")


# ----------------------------------------------------------------- analyze
def analyze(path, output):
    """Enhanced LSB Attack: LSB 1 -> 255, LSB 0 -> 0 pada tiap channel."""
    shape, flat = load_pixels(path)
    enhanced = ((flat & 1) * 255).astype(np.uint8)
    save_pixels(shape, enhanced, output)
    print(f"[+] Hasil Enhanced LSB Attack disimpan: {output}")
    print("    Area berisi noise acak = indikasi kuat adanya pesan tersembunyi.")


# --------------------------------------------------------------------- CLI
def main():
    p = argparse.ArgumentParser(description="LSB Steganography (encode/decode/analyze)")
    sub = p.add_subparsers(dest="cmd", required=True)

    def common(sp):
        sp.add_argument("-b", "--bits", type=int, default=1, choices=range(1, 9),
                        metavar="[1-8]", help="jumlah bit LSB (default 1)")
        sp.add_argument("--mode", choices=["sequential", "random"], default="sequential")
        sp.add_argument("-k", "--key", type=int, default=0, help="stego-key / seed (mode random)")

    e = sub.add_parser("encode", help="sisipkan pesan/file ke citra")
    e.add_argument("-i", "--input", required=True, help="cover image")
    e.add_argument("-o", "--output", required=True, help="stego image (.png/.bmp)")
    g = e.add_mutually_exclusive_group(required=True)
    g.add_argument("-t", "--text", help="pesan teks rahasia")
    g.add_argument("-f", "--file", help="file rahasia (gambar/dokumen/apa saja)")
    common(e)

    d = sub.add_parser("decode", help="ekstrak pesan/file dari stego image")
    d.add_argument("-i", "--input", required=True, help="stego image")
    d.add_argument("-o", "--outdir", default="output", help="folder hasil ekstraksi file")
    common(d)

    a = sub.add_parser("analyze", help="steganalysis: Enhanced LSB Attack")
    a.add_argument("-i", "--input", required=True)
    a.add_argument("-o", "--output", default="enhanced_lsb.png")

    args = p.parse_args()
    if args.cmd == "encode":
        encode(args.input, args.output, args.text, args.file, args.bits, args.mode, args.key)
    elif args.cmd == "decode":
        decode(args.input, args.outdir, args.bits, args.mode, args.key)
    else:
        analyze(args.input, args.output)


if __name__ == "__main__":
    main()