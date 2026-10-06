1. **Nama    : Kezia Tabhita Smith**
2. **NPM     : 140810240020**
3. **Kelas   : Kriptografi A**
4. **Tanggal : 15 September 2026**

# LSB Steganography

Tugas Praktikum Kriptografi – Pertemuan 5 (Steganografi, LSB, Steganalysis).

Program Python untuk menyembunyikan **teks** atau **file apa pun (termasuk gambar)** ke dalam citra
menggunakan metode **Least Significant Bit (LSB)**, dilengkapi fitur steganalysis (Enhanced LSB Attack).

## Fitur

| Fitur | Keterangan |
|-------|------------|
| Encode / Decode | Menyisipkan dan mengekstrak pesan teks atau file |
| Mode sequential | Bit pesan disimpan di byte citra secara berurutan |
| Mode random | Posisi byte dipilih acak oleh PRNG, seed = stego-key |
| m-bit LSB | Menggunakan 1–8 bit LSB per byte (trade-off kapasitas vs kualitas) |
| Validasi kapasitas | Menolak pesan yang melebihi kapasitas citra |
| PSNR | Menampilkan kualitas stego-image setelah encode |
| Analyze | Enhanced LSB Attack untuk mendeteksi pesan secara visual |

## Struktur Folder

```
Steganography/
├── LSB_steg.py
├── README.md
├── cover.png
└── screenshot/
```

## Instalasi

Membutuhkan Python 3 dengan library Pillow dan NumPy.

```bash
pip3 install pillow numpy
```

## Cara Penggunaan

Program harus dijalankan dengan salah satu perintah: `encode`, `decode`, atau `analyze`.
Untuk melihat semua opsi: `python3 LSB_steg.py encode -h`.

### 1. Sembunyikan dan ekstrak teks (1-bit LSB, sequential)

```bash
python3 LSB_steg.py encode -i cover.png -o stego.png -t "Halo, ini pesan rahasia"
python3 LSB_steg.py decode -i stego.png
```

### 2. Sembunyikan dan ekstrak file gambar (2-bit LSB, random, key 1234)

Siapkan dulu file rahasia berupa gambar kecil:

```bash
python3 -c "from PIL import Image; im=Image.open('cover.png'); im.thumbnail((60,60)); im.save('secret.png')"
```

Lalu:

```bash
python3 LSB_steg.py encode -i cover.png -o stego2.png -f secret.png -b 2 --mode random -k 1234
python3 LSB_steg.py decode -i stego2.png -b 2 --mode random -k 1234 -o hasil
```

File hasil ekstraksi tersimpan di `hasil/extracted_secret.png`.

### 3. Decode dengan key salah

```bash
python3 LSB_steg.py decode -i stego2.png -b 2 --mode random -k 999
```

Hasilnya: `[-] Tidak ditemukan pesan`.

### 4. Steganalysis (Enhanced LSB Attack)

```bash
python3 LSB_steg.py analyze -i stego.png -o enhanced.png
python3 LSB_steg.py analyze -i stego2.png -o enhanced2.png
```

### Opsi

| Opsi | Fungsi | Default |
|------|--------|---------|
| `-i`, `--input` | Gambar input (cover atau stego) | wajib |
| `-o`, `--output` | Gambar output (encode/analyze) atau folder hasil (decode) | wajib / `output` |
| `-t`, `--text` | Pesan teks rahasia | – |
| `-f`, `--file` | File rahasia (gambar, dokumen, dll.) | – |
| `-b`, `--bits` | Jumlah bit LSB (1–8) | 1 |
| `--mode` | `sequential` atau `random` | sequential |
| `-k`, `--key` | Stego-key / seed untuk mode random | 0 |

> Output stego-image **wajib `.png` atau `.bmp`** (lossless). Format JPG memakai kompresi lossy
> sehingga bit LSB akan rusak. Saat decode, nilai `-b`, `--mode`, dan `-k` harus sama
> dengan saat encode.

## Cara Kerja

### 1. Format payload

Pesan dibungkus header agar bisa diekstrak tanpa mengetahui panjangnya terlebih dahulu:

```
MAGIC "LSBS" (4 byte) | TYPE (1) | NAMELEN (1) | NAME | DATALEN (4 byte) | DATA
```

- `MAGIC` untuk mengecek apakah ada pesan dan parameter (bit/mode/key) sudah benar.
- `TYPE` membedakan teks (0) dan file (1).
- `NAME` menyimpan nama file asli (kosong untuk teks).
- `DATALEN` adalah panjang data dalam byte.

### 2. Encode

1. Citra dibuka sebagai RGB lalu diratakan menjadi array byte (1 pixel = 3 byte: R, G, B).
2. Payload diubah menjadi deretan bit, lalu dipotong per `m` bit.
3. Posisi byte ditentukan:
   - **sequential**: byte ke-0, 1, 2, dan seterusnya.
   - **random**: permutasi indeks byte citra memakai PRNG dengan seed = stego-key.
4. Pada tiap byte terpilih, `m` bit terakhir dikosongkan lalu diisi bit pesan:
   `byte_baru = (byte & ~mask) | bit_pesan`.
5. Hasil disimpan sebagai PNG/BMP.

Contoh (1-bit LSB): byte `10000010` (130) disisipi bit `1` menjadi `10000011` (131).
Perubahan hanya ±1 sehingga tidak terlihat oleh mata manusia.

### 3. Decode

Program membaca `m` bit terakhir dari byte citra dengan urutan posisi yang sama, menyusunnya
kembali menjadi byte, memeriksa `MAGIC`, membaca header, lalu menampilkan teks atau
menyimpan file hasil ekstraksi. Jika key atau parameter salah, `MAGIC` tidak cocok dan pesan
tidak dapat dibaca.

### 4. Analyze (Enhanced LSB Attack)

Setiap channel warna diubah menjadi `255` jika LSB-nya 1 dan `0` jika LSB-nya 0. Area yang berisi
pesan tampak sebagai noise acak, sedangkan citra asli cenderung memperlihatkan pola objek.
Metode ini termasuk visual attack pada steganalysis.

## Screenshot

Tambahkan screenshot hasil running program di folder `screenshot/`.

| Langkah | Screenshot |
|---------|------------|
| Encode dan decode teks | `screenshot/teks.png` |
| Encode dan decode file gambar | `screenshot/file.png` |
| Decode dengan key salah | `screenshot/key_salah.png` |
| Enhanced LSB Attack | `screenshot/analyze.png` |

## Catatan Keamanan

Mode random hanya menyembunyikan posisi bit, sedangkan pesan tidak dienkripsi. Untuk keamanan
lebih baik, enkripsi pesan terlebih dahulu (misalnya dengan AES) sebelum disisipkan.