/*
Nama    : Kezia Tabhita Smith
NPM     : 140810240020
Kelas   : Kriptografi A
Tanggal : 15 September 2026
*/

#include <iostream>
#include <string>
#include <cctype>
using namespace std;

// Membuat angka selalu positif dalam mod 26
int mod26(int x) {
    return ((x % 26) + 26) % 26;
}

// Mencari invers modular dari a terhadap mod 26 (brute force, cepat karena mod kecil)
int modInverse(int a) {
    a = mod26(a);
    for (int x = 1; x < 26; x++)
        if (mod26(a * x) == 1) return x;
    return -1; // tidak ada invers -> a tidak coprime dengan 26
}

// Menghitung determinan matriks 2x2 (mod 26)
int determinan(int m[2][2]) {
    return mod26(m[0][0]*m[1][1] - m[0][1]*m[1][0]);
}

// Mencari invers matriks 2x2 mod 26, return false jika matriks tidak valid sebagai kunci
bool inverseMatrix(int m[2][2], int inv[2][2]) {
    int det = determinan(m);
    int detInv = modInverse(det);
    if (detInv == -1) return false;
    inv[0][0] = mod26( m[1][1] * detInv);
    inv[0][1] = mod26(-m[0][1] * detInv);
    inv[1][0] = mod26(-m[1][0] * detInv);
    inv[1][1] = mod26( m[0][0] * detInv);
    return true;
}

// Mengalikan matriks kunci 2x2 dengan sepasang huruf (vektor kolom), hasil dikembalikan lewat p,q
void kaliVektor(int m[2][2], int p, int q, int &r0, int &r1) {
    r0 = mod26(m[0][0]*p + m[0][1]*q);
    r1 = mod26(m[1][0]*p + m[1][1]*q);
}

// Proses umum enkripsi/dekripsi: teks diproses per 2 huruf memakai matriks yang diberikan
string prosesTeks(string teks, int m[2][2]) {
    // bersihkan teks: huruf besar saja
    string bersih;
    for (char c : teks)
        if (isalpha(c)) bersih += toupper(c);
    // padding dengan 'X' jika ganjil
    if (bersih.size() % 2 != 0) bersih += 'X';

    string hasil;
    for (size_t i = 0; i < bersih.size(); i += 2) {
        int p = bersih[i] - 'A';
        int q = bersih[i+1] - 'A';
        int r0, r1;
        kaliVektor(m, p, q, r0, r1);
        hasil += char('A' + r0);
        hasil += char('A' + r1);
    }
    return hasil;
}

// Meminta user memasukkan matriks kunci 2x2 dan menampilkan determinannya
bool inputKunci(int m[2][2]) {
    cout << "Masukkan matriks kunci 2x2 (4 angka, baris per baris):\n";
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            cin >> m[i][j];

    int det = determinan(m);
    cout << "Determinan kunci (mod 26) = " << det << "\n";

    if (modInverse(det) == -1) {
        cout << "Kunci TIDAK valid! Determinan tidak coprime dengan 26.\n";
        return false;
    }
    cout << "Kunci valid.\n";
    return true;
}

// Menu 1: Enkripsi plaintext memakai matriks kunci
void menuEnkripsi() {
    int key[2][2];
    if (!inputKunci(key)) return;
    cin.ignore();
    cout << "Masukkan plaintext: ";
    string teks; getline(cin, teks);
    cout << "Ciphertext: " << prosesTeks(teks, key) << "\n";
}

// Menu 2: Dekripsi ciphertext memakai invers matriks kunci
void menuDekripsi() {
    int key[2][2], invKey[2][2];
    if (!inputKunci(key)) return;
    inverseMatrix(key, invKey); // pasti berhasil karena sudah divalidasi
    cin.ignore();
    cout << "Masukkan ciphertext: ";
    string teks; getline(cin, teks);
    cout << "Plaintext: " << prosesTeks(teks, invKey) << "\n";
}

// Menu 3: Mencari kunci dari pasangan plaintext & ciphertext yang diketahui (known-plaintext attack)
// Butuh minimal 4 huruf plaintext & 4 huruf ciphertext yang bersesuaian
void menuCariKunci() {
    cin.ignore();
    cout << "Masukkan 4 huruf plaintext yang diketahui: ";
    string plain; getline(cin, plain);
    cout << "Masukkan 4 huruf ciphertext yang bersesuaian: ";
    string cipher; getline(cin, cipher);

    string p, c;
    for (char x : plain)  if (isalpha(x)) p += toupper(x);
    for (char x : cipher) if (isalpha(x)) c += toupper(x);

    if (p.size() < 4 || c.size() < 4) {
        cout << "Data kurang! Perlu minimal 4 huruf plaintext dan ciphertext.\n";
        return;
    }

    // Susun matriks P (plaintext, per kolom) dan C (ciphertext, per kolom)
    int P[2][2] = {{p[0]-'A', p[2]-'A'}, {p[1]-'A', p[3]-'A'}};
    int C[2][2] = {{c[0]-'A', c[2]-'A'}, {c[1]-'A', c[3]-'A'}};

    int Pinv[2][2];
    if (!inverseMatrix(P, Pinv)) {
        cout << "Pasangan plaintext ini tidak bisa dipakai (matriks tidak invertible).\n";
        return;
    }

    // Kunci K = C * P^-1 (mod 26)
    int K[2][2];
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            K[i][j] = mod26(C[i][0]*Pinv[0][j] + C[i][1]*Pinv[1][j]);

    cout << "Kunci ditemukan:\n";
    cout << K[0][0] << " " << K[0][1] << "\n";
    cout << K[1][0] << " " << K[1][1] << "\n";
    cout << "Determinan kunci = " << determinan(K) << "\n";
}

int main() {
    int pilihan;
    do {
        cout << "\n=== HILL CIPHER (2x2) ===\n";
        cout << "1. Enkripsi\n";
        cout << "2. Dekripsi\n";
        cout << "3. Cari Kunci (Known-Plaintext Attack)\n";
        cout << "4. Keluar\n";
        cout << "Pilih menu: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1: menuEnkripsi(); break;
            case 2: menuDekripsi(); break;
            case 3: menuCariKunci(); break;
            case 4: cout << "Selesai.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
    } while (pilihan != 4);

    return 0;
}