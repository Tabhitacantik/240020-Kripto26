Nama    : Kezia Tabhita Smith
NPM     : 140810240020
Kelas   : Kriptografi A
Tanggal : 15 September 2026

# Hill-Cipher

Program C++ sederhana untuk melakukan **enkripsi**, **dekripsi**, dan **pencarian kunci** (known-plaintext attack) menggunakan algoritma **Hill Cipher** dengan matriks kunci 2x2.

## Struktur Folder

```
Hill-Cipher/
├── hillcipher.cpp
└── README.md
```

## Fitur

1. **Enkripsi** — mengubah plaintext menjadi ciphertext menggunakan matriks kunci yang diberikan.
2. **Dekripsi** — mengembalikan ciphertext menjadi plaintext menggunakan invers matriks kunci.
3. **Cari Kunci** — menemukan matriks kunci dari pasangan plaintext & ciphertext yang diketahui (minimal 4 huruf masing-masing).
4. Menampilkan **determinan kunci (mod 26)** setiap kali kunci dimasukkan, sekaligus validasi apakah kunci tersebut layak dipakai (determinan harus coprime dengan 26).

## Cara Compile & Menjalankan

```bash
g++ -o hillcipher hillcipher.cpp
./hillcipher
```

Di Windows (contoh dengan g++/MinGW):

```bash
g++ -o hillcipher.exe hillcipher.cpp
hillcipher.exe
```

Di macOS (menggunakan Terminal, compiler bawaan Xcode Command Line Tools):

```bash
g++ -o hillcipher hillcipher.cpp
./hillcipher
```

> Jika belum ada compiler, install dulu Xcode Command Line Tools dengan menjalankan:
> ```bash
> xcode-select --install
> ```
> Setelah itu perintah `g++` (yang sebenarnya adalah `clang++`) akan tersedia di Terminal.

## Cara Pakai

Setelah dijalankan, akan muncul menu:

```
=== HILL CIPHER (2x2) ===
1. Enkripsi
2. Dekripsi
3. Cari Kunci (Known-Plaintext Attack)
4. Keluar
```

### 1. Enkripsi
- Masukkan 4 angka matriks kunci (baris per baris), contoh: `7 6 2 5` untuk matriks
  ```
  7 6
  2 5
  ```
- Program menampilkan determinan kunci dan validitasnya.
- Masukkan plaintext, contoh: `MAGANG`
- Hasil ciphertext: `GYQMXE`

### 2. Dekripsi
- Masukkan matriks kunci yang sama seperti saat enkripsi.
- Masukkan ciphertext, contoh: `GYQMXE`
- Hasil plaintext: `MAGANG`

### 3. Cari Kunci
- Masukkan minimal 4 huruf plaintext yang diketahui, contoh: `HELP`
- Masukkan 4 huruf ciphertext yang bersesuaian, contoh: `VILT`
- Program akan menghitung dan menampilkan matriks kunci beserta determinannya.
- Catatan: pasangan plaintext yang dipilih harus membentuk matriks yang *invertible* mod 26 (tidak semua pasangan huruf bisa dipakai).

## Contoh Uji

| Aksi     | Kunci      | Input     | Output    |
|----------|------------|-----------|-----------|
| Enkripsi | `7 6 2 5`  | `MAGANG`  | `GYQMXE`  |
| Dekripsi | `7 6 2 5`  | `GYQMXE`  | `MAGANG`  |
| Enkripsi | `7 6 2 5`  | `HELP`    | `VILT`    |
| Cari Kunci | -        | `HELP` / `VILT` | `7 6 2 5` |

## Catatan Teknis

- Aritmatika dilakukan dalam modulo 26 (alfabet A–Z).
- Karakter non-huruf pada input akan diabaikan, dan huruf otomatis dikapitalkan.
- Jika panjang teks ganjil saat enkripsi, akan ditambahkan padding huruf `X`.
- Matriks kunci hanya valid jika determinannya (mod 26) memiliki invers, yaitu coprime dengan 26 (tidak habis dibagi 2 atau 13).
