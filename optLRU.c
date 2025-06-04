#include <stdio.h>
#include <stdlib.h> // Untuk malloc dan free
#include <stdbool.h> // Untuk tipe data bool

// Definisi nilai maksimum yang cukup besar untuk "jarak terjauh"
// Meskipun INT_MAX lebih akurat, kita akan menggunakan nilai yang cukup besar
// sesuai dengan semangat tidak menggunakan <limits.h> langsung.
#define MY_LARGE_VALUE 1000000 

// --- Fungsi Pendukung Umum ---

// Fungsi ini memeriksa apakah item stream saat ini (key) ada di salah satu frame atau tidak
bool search(int key, int frame_items[], int frame_occupied) {
    for (int i = 0; i < frame_occupied; i++)
        if (frame_items[i] == key)
            return true; // Ditemukan
    return false; // Tidak ditemukan
}

// --- Implementasi Optimal Page Replacement ---

// Fungsi ini membantu dalam menemukan frame yang tidak akan digunakan
// untuk periode waktu terlama di masa depan dalam ref_str[0 ... refStrLen - 1]
int predict(int ref_str[], int frame_items[], int refStrLen, int index, int frame_occupied) {
    int result = -1, farthest = index;
    for (int i = 0; i < frame_occupied; i++) {
        int j;
        // Mencari kemunculan halaman frame_items[i] di string referensi mulai dari 'index'
        for (j = index; j < refStrLen; j++) {
            if (frame_items[i] == ref_str[j]) {
                // Jika ditemukan dan lebih jauh dari yang sudah ditemukan
                if (j > farthest) {
                    farthest = j;
                    result = i; // Simpan indeks frame yang akan diganti
                }
                break; // Hentikan pencarian untuk frame ini, karena sudah ditemukan kemunculan pertamanya
            }
        }

        // Jika kita menemukan halaman yang tidak pernah direferensikan di masa depan (j == refStrLen),
        // kembalikan indeks frame tersebut segera karena itu adalah pilihan terbaik untuk diganti
        if (j == refStrLen)
            return i;
    }

    // Jika semua item frame akan digunakan lagi, kembalikan yang paling jauh.
    // Jika tidak ada yang ditemukan (result masih -1), kembalikan indeks 0 sebagai fallback.
    return (result == -1) ? 0 : result;
}

// Fungsi utama untuk Algoritma Penggantian Halaman Optimal
int optimalPageReplacement(int ref_str[], int refStrLen, int max_frames) {
    int frame_items[max_frames]; // Array untuk menyimpan item di frame
    int frame_occupied = 0;     // Jumlah frame yang sudah terisi
    int hits = 0;               // Penghitung hit

    // Inisialisasi semua frame menjadi -1 (kosong)
    for (int i = 0; i < max_frames; i++) {
        frame_items[i] = -1;
    }

    // Melintasi string referensi
    for (int i = 0; i < refStrLen; i++) {
        // Jika halaman sudah ada di frame: HIT
        if (search(ref_str[i], frame_items, frame_occupied)) {
            hits++;
            continue; // Lanjutkan ke halaman berikutnya
        }

        // Jika halaman tidak ditemukan di frame: MISS

        // Jika masih ada frame kosong
        if (frame_occupied < max_frames) {
            frame_items[frame_occupied] = ref_str[i]; // Masukkan halaman ke frame kosong
            frame_occupied++;
        }
        // Jika semua frame sudah penuh, lakukan penggantian Optimal
        else {
            // Dapatkan indeks frame yang akan diganti menggunakan fungsi predict
            int pos = predict(ref_str, frame_items, refStrLen, i + 1, frame_occupied);
            frame_items[pos] = ref_str[i]; // Ganti halaman
        }
    }
    // Jumlah page fault adalah total halaman dikurangi jumlah hit
    return refStrLen - hits;
}

// --- Implementasi LRU Page Replacement ---

// Fungsi utama untuk Algoritma Penggantian Halaman LRU
int lruPageReplacement(int pages[], int total_pages, int total_frames) {
    int frames[total_frames]; // Array untuk menyimpan item di frame
    // Array untuk menyimpan waktu terakhir kali setiap halaman di frame digunakan
    int last_used_time[total_frames]; 
    int page_fault = 0;
    int current_time = 0; // Penanda waktu global untuk melacak LRU

    // Inisialisasi semua frame menjadi -1 (kosong) dan waktu penggunaan menjadi 0
    for (int m = 0; m < total_frames; m++) {
        frames[m] = -1;
        last_used_time[m] = 0;
    }

    // Iterasi melalui setiap halaman dalam string referensi
    for (int n = 0; n < total_pages; n++) {
        current_time++; // Tingkatkan waktu setiap kali halaman diakses
        int current_page = pages[n];
        bool found = false;

        // Periksa apakah halaman sudah ada di frame (HIT)
        for (int m = 0; m < total_frames; m++) {
            if (frames[m] == current_page) {
                found = true;
                last_used_time[m] = current_time; // Perbarui waktu terakhir digunakan
                break;
            }
        }

        // Jika halaman tidak ditemukan (MISS)
        if (!found) {
            page_fault++; // Tambah jumlah page fault

            int empty_frame_idx = -1;
            // Cari frame kosong terlebih dahulu
            for (int m = 0; m < total_frames; m++) {
                if (frames[m] == -1) {
                    empty_frame_idx = m;
                    break;
                }
            }

            if (empty_frame_idx != -1) {
                // Jika ada frame kosong, masukkan halaman ke dalamnya
                frames[empty_frame_idx] = current_page;
                last_used_time[empty_frame_idx] = current_time;
            } else {
                // Jika tidak ada frame kosong, cari halaman LRU untuk diganti
                int lru_idx = -1;
                int min_last_used_time = MY_LARGE_VALUE; // Cari waktu terkecil (paling tidak baru digunakan)

                for (int m = 0; m < total_frames; m++) {
                    if (last_used_time[m] < min_last_used_time) {
                        min_last_used_time = last_used_time[m];
                        lru_idx = m;
                    }
                }
                // Ganti halaman LRU dengan halaman baru
                frames[lru_idx] = current_page;
                last_used_time[lru_idx] = current_time;
            }
        }
    }
    return page_fault; // Kembalikan total page fault
}

// --- Fungsi Utama (Driver Function) ---
int main() {
    int num_frames;
    printf("Masukkan jumlah frame: ");
    scanf("%d", &num_frames);

    printf("Masukkan page reference string (akhiri dengan -1):\n");
    int pages_input[200]; // Asumsi maksimal 200 halaman dalam string referensi
    int num_pages = 0;
    int page_val;

    // Membaca input page reference string hingga -1
    while (scanf("%d", &page_val) == 1 && page_val != -1) {
        if (num_pages < 200) {
            pages_input[num_pages++] = page_val;
        } else {
            printf("Batas maksimum page reference string tercapai (%d). Menghentikan input.\n", 200);
            break;
        }
    }

    // Mengalokasikan memori secara dinamis untuk string referensi aktual
    // Ini adalah praktik yang lebih baik dan lebih aman daripada array statis besar.
    int *actual_pages = (int *)malloc(num_pages * sizeof(int));
    if (actual_pages == NULL) {
        printf("Gagal mengalokasikan memori.\n");
        return 1; // Keluar dengan kode error
    }
    // Menyalin data dari array input sementara ke array dinamis
    for (int i = 0; i < num_pages; i++) {
        actual_pages[i] = pages_input[i];
    }

    // Hitung page fault untuk LRU
    int lru_faults = lruPageReplacement(actual_pages, num_pages, num_frames);
    printf("Jumlah page fault (LRU): %d\n", lru_faults);

    // Hitung page fault untuk Optimal
    int optimal_faults = optimalPageReplacement(actual_pages, num_pages, num_frames);
    printf("Jumlah page fault (Optimal): %d\n", optimal_faults);

    free(actual_pages); // Bebaskan memori yang dialokasikan secara dinamis

    return 0; // Program berakhir dengan sukses
}