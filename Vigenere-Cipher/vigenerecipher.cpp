/*
Nama    : Kezia Tabhita Smith
NPM     : 140810240020
Kelas   : Kriptografi A
Tanggal : 22 September 2026
*/

#include <iostream>
#include <string>
#include <cctype>
using namespace std;

// ================================
// FUNGSI ENKRIPSI VIGENERE
// ================================
string enkripsi(string plaintext, string key) {
    string ciphertext = "";
    int keyIndex = 0;

    for (char c : plaintext) {

        if (isalpha(c)) {
            char p = toupper(c);
            char k = toupper(key[keyIndex % key.length()]);

            // C = (P + K) mod 26
            char cipher = ((p - 'A') + (k - 'A')) % 26 + 'A';

            ciphertext += cipher;
            keyIndex++;
        }
        else {
            // Spasi / karakter lain tetap
            ciphertext += c;
        }
    }

    return ciphertext;
}


// ================================
// FUNGSI DEKRIPSI VIGENERE
// ================================
string dekripsi(string ciphertext, string key) {
    string plaintext = "";
    int keyIndex = 0;

    for (char c : ciphertext) {

        if (isalpha(c)) {
            char cipher = toupper(c);
            char k = toupper(key[keyIndex % key.length()]);

            // P = (C - K + 26) mod 26
            char plain = ((cipher - 'A') - (k - 'A') + 26) % 26 + 'A';

            plaintext += plain;
            keyIndex++;
        }
        else {
            plaintext += c;
        }
    }

    return plaintext;
}


// ================================
// FUNGSI CEK KEY
// ================================
bool cekKey(string key) {

    for (char c : key) {

        // Key hanya boleh berisi huruf
        if (!isalpha(c)) {
            return false;
        }
    }

    return true;
}


// ================================
// MAIN PROGRAM
// ================================
int main() {

    string plaintext;
    string ciphertext;
    string key;

    int pilihan;

    do {

        cout << "\n====================================\n";
        cout << "        VIGENERE CIPHER\n";
        cout << "====================================\n";
        cout << "1. Enkripsi\n";
        cout << "2. Dekripsi\n";
        cout << "3. Keluar\n";
        cout << "------------------------------------\n";
        cout << "Pilih menu : ";
        cin >> pilihan;

        cin.ignore();

        // ============================
        // MENU ENKRIPSI
        // ============================
        if (pilihan == 1) {

            cout << "\n========== ENKRIPSI ==========\n";

            cout << "Masukkan Plaintext : ";
            getline(cin, plaintext);

            cout << "Masukkan Key       : ";
            getline(cin, key);

            if (key.empty()) {
                cout << "\nKey tidak boleh kosong!\n";
                continue;
            }

            if (!cekKey(key)) {
                cout << "\nKey hanya boleh berisi huruf!\n";
                continue;
            }

            ciphertext = enkripsi(plaintext, key);

            cout << "\n---------- HASIL ----------\n";
            cout << "Plaintext  : " << plaintext << endl;
            cout << "Key        : " << key << endl;
            cout << "Ciphertext : " << ciphertext << endl;
            cout << "---------------------------\n";
        }


        // ============================
        // MENU DEKRIPSI
        // ============================
        else if (pilihan == 2) {

            cout << "\n========== DEKRIPSI ==========\n";

            cout << "Masukkan Ciphertext : ";
            getline(cin, ciphertext);

            cout << "Masukkan Key        : ";
            getline(cin, key);

            if (key.empty()) {
                cout << "\nKey tidak boleh kosong!\n";
                continue;
            }

            if (!cekKey(key)) {
                cout << "\nKey hanya boleh berisi huruf!\n";
                continue;
            }

            plaintext = dekripsi(ciphertext, key);

            cout << "\n---------- HASIL ----------\n";
            cout << "Ciphertext : " << ciphertext << endl;
            cout << "Key        : " << key << endl;
            cout << "Plaintext  : " << plaintext << endl;
            cout << "---------------------------\n";
        }

        // ============================
        // MENU KELUAR
        // ============================
        else if (pilihan == 3) {

            cout << "\nProgram selesai.\n";
        }


        else {
            cout << "\nPilihan tidak valid!\n";
        }

    } while (pilihan != 3);


    return 0;
}