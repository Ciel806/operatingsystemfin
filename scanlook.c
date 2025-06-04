#include <stdio.h>
#include <stdlib.h> // Diperlukan untuk abs()

#define MAX_REQUESTS 100 // Batasan jumlah permintaan I/O

// --- Fungsi Pembantu ---

// Fungsi untuk mengurutkan array permintaan I/O (menggunakan Bubble Sort)
void sort_requests(int requests[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (requests[j] > requests[j + 1]) {
                int temp = requests[j];
                requests[j] = requests[j + 1];
                requests[j + 1] = temp;
            }
        }
    }
}

// Fungsi untuk menemukan indeks permintaan pertama yang lebih besar dari posisi head saat ini
int find_index(int requests[], int n, int head) {
    for (int i = 0; i < n; i++) {
        if (requests[i] >= head) { // Gunakan '>=' agar head juga dipertimbangkan jika ada di posisi yang sama
            return i;
        }
    }
    return n; // Jika semua permintaan lebih kecil dari head, kembalikan n
}

// --- Algoritma Penjadwalan Disk ---

// Implementasi algoritma C-SCAN
void c_scan(int head, int requests_arr[], int n, int disk_min, int disk_max) {
    int total_movement = 0;
    int current_pos = head;
    int step = 1;

    // Buat salinan permintaan agar tidak mengubah array asli
    int temp_requests[MAX_REQUESTS + 2]; // +2 untuk menampung disk_min dan disk_max jika perlu
    for (int i = 0; i < n; i++) {
        temp_requests[i] = requests_arr[i];
    }
    int current_n = n;

    // Tambahkan disk_min dan disk_max ke permintaan agar head mencapai ujung disk
    int found_min = 0, found_max = 0;
    for (int i = 0; i < n; i++) {
        if (requests_arr[i] == disk_min) found_min = 1;
        if (requests_arr[i] == disk_max) found_max = 1;
    }
    if (!found_min) temp_requests[current_n++] = disk_min;
    if (!found_max) temp_requests[current_n++] = disk_max;

    sort_requests(temp_requests, current_n); // Urutkan semua permintaan termasuk batas disk

    printf("\nAlgoritma: C-SCAN\n");
    printf("STEP %d: Head di posisi %d - Total movement: %d\n", step++, current_pos, total_movement);

    int index = find_index(temp_requests, current_n, head);

    // Layani permintaan dari posisi head saat ini menuju disk_max
    for (int i = index; i < current_n; i++) {
        // Hanya layani permintaan yang sebenarnya ada di antrian (bukan disk_min/disk_max tambahan kecuali disk_max)
        if (temp_requests[i] >= head || temp_requests[i] == disk_max) {
            total_movement += abs(current_pos - temp_requests[i]);
            current_pos = temp_requests[i];
            printf("STEP %d: Head pindah ke %d - Total movement: %d\n", step++, current_pos, total_movement);
        }
    }

    // Pergerakan dari disk_max ke disk_min (wrap-around)
    // C-SCAN akan selalu melewati disk_max lalu melompat ke disk_min
    if (current_pos != disk_max) { // Pastikan head sudah di disk_max sebelum wrap-around
        total_movement += abs(current_pos - disk_max);
        current_pos = disk_max;
        printf("STEP %d: Head pindah ke %d (menuju akhir disk) - Total movement: %d\n", step++, current_pos, total_movement);
    }
    
    total_movement += abs(current_pos - disk_min); // Pergerakan wrap-around
    current_pos = disk_min;
    printf("STEP %d: Head pindah ke %d (wrap-around ke awal disk) - Total movement: %d\n", step++, current_pos, total_movement);

    // Layani permintaan dari disk_min hingga sebelum titik awal (index)
    for (int i = 0; i < index; i++) {
        // Hanya layani permintaan yang sebenarnya ada di antrian dan belum dilayani
        if (temp_requests[i] >= disk_min && temp_requests[i] < head) {
            total_movement += abs(current_pos - temp_requests[i]);
            current_pos = temp_requests[i];
            printf("STEP %d: Head pindah ke %d - Total movement: %d\n", step++, current_pos, total_movement);
        }
    }
    
    printf("\nTotal pergerakan head: %d\n", total_movement);
}

// Implementasi algoritma C-LOOK
void c_look(int head, int requests_arr[], int n, int disk_min, int disk_max) {
    int total_movement = 0;
    int current_pos = head;
    int step = 1;

    // Buat salinan permintaan dan urutkan
    int temp_requests[MAX_REQUESTS];
    for (int i = 0; i < n; i++) {
        temp_requests[i] = requests_arr[i];
    }
    sort_requests(temp_requests, n);

    printf("\nAlgoritma: C-LOOK\n");
    printf("STEP %d: Head di posisi %d - Total movement: %d\n", step++, current_pos, total_movement);
    
    int index = find_index(temp_requests, n, head);

    // Layani permintaan dari posisi head saat ini menuju permintaan terbesar
    for (int i = index; i < n; i++) {
        total_movement += abs(current_pos - temp_requests[i]);
        current_pos = temp_requests[i];
        printf("STEP %d: Head pindah ke %d - Total movement: %d\n", step++, current_pos, total_movement);
    }

    // Jika ada permintaan di sisi kiri posisi head awal,
    // head akan melompat ke permintaan terkecil dan melanjutkan
    if (index > 0) {
        total_movement += abs(current_pos - temp_requests[0]); // Lompat ke permintaan terkecil
        current_pos = temp_requests[0];
        printf("STEP %d: Head pindah ke %d (kembali ke permintaan terkecil) - Total movement: %d\n", step++, current_pos, total_movement);

        for (int i = 0; i < index; i++) { // Lanjutkan melayani permintaan yang tersisa
            total_movement += abs(current_pos - temp_requests[i]);
            current_pos = temp_requests[i];
            printf("STEP %d: Head pindah ke %d - Total movement: %d\n", step++, current_pos, total_movement);
        }
    }
    
    printf("\nTotal pergerakan head: %d\n", total_movement);
}

// --- Fungsi Utama ---

int main() {
    int head, disk_max;
    int requests[MAX_REQUESTS];
    int n = 0;
    int choice;
    int disk_min = 0; // Disk selalu dimulai dari 0

    printf("Masukkan posisi awal head: ");
    scanf("%d", &head);
    
    printf("Masukkan panjang disk (misalnya 199): ");
    scanf("%d", &disk_max);
    
    printf("Masukkan jumlah permintaan I/O: ");
    scanf("%d", &n);
    
    if (n > MAX_REQUESTS) {
        printf("Error: Jumlah permintaan melebihi batas (%d).\n", MAX_REQUESTS);
        return 1;
    }
    
    printf("Masukkan daftar permintaan (dipisah spasi): ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &requests[i]);
    }
    
    printf("Pilih algoritma yang akan dijalankan (1 = C-SCAN, 2 = C-LOOK): ");
    scanf("%d", &choice);
    
    // Panggil algoritma yang dipilih
    switch (choice) {
        case 1:
            c_scan(head, requests, n, disk_min, disk_max);
            break;
        case 2:
            c_look(head, requests, n, disk_min, disk_max);
            break;
        default:
            printf("Pilihan tidak valid.\n");
            return 1;
    }
    
    return 0;
}