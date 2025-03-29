#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Простая раундовая функция
unsigned char round_function(unsigned char data, unsigned char key) {
    return (data + key) % 256;
}

// Генерация ключей для заданного количества раундов
vector<unsigned char> generate_keys(int rounds) {
    vector<unsigned char> keys;
    for (int i = 1; i <= rounds; i++) {
        keys.push_back((i * 42) % 256);
    }
    return keys;
}

// Функция обработки данных (используется для шифрования и дешифрования)
vector<unsigned char> feistel_process(const vector<unsigned char>& block,
    const vector<unsigned char>& keys,
    bool reverse_keys = false) {
    if (block.size() % 2 != 0) {
        throw runtime_error("Block size must be even");
    }

    size_t half = block.size() / 2;
    vector<unsigned char> left(block.begin(), block.begin() + half);
    vector<unsigned char> right(block.begin() + half, block.end());

    size_t num_rounds = keys.size();

    for (size_t i = 0; i < num_rounds; i++) {
        size_t key_index = reverse_keys ? (num_rounds - 1 - i) : i;
        unsigned char key = keys[key_index];

        vector<unsigned char> new_right(half);
        for (size_t j = 0; j < half; j++) {
            new_right[j] = left[j] ^ round_function(right[j], key);
        }

        left = right;
        right = new_right;
    }

    // В конце соединяем right + left (без последнего обмена)
    vector<unsigned char> result(right);
    result.insert(result.end(), left.begin(), left.end());

    return result;
}

// Функция шифрования
vector<unsigned char> feistel_encrypt(const vector<unsigned char>& block, int rounds = 4) {
    vector<unsigned char> keys = generate_keys(rounds);
    return feistel_process(block, keys);
}

// Функция дешифрования
vector<unsigned char> feistel_decrypt(const vector<unsigned char>& block, int rounds = 4) {
    vector<unsigned char> keys = generate_keys(rounds);
    return feistel_process(block, keys, true);
}

// Вспомогательная функция для вывода вектора
void print_vector(const vector<unsigned char>& v) {
    for (unsigned char c : v) {
        cout << (isprint(c) ? c : '.');
    }
    cout << " [";
    for (unsigned char c : v) {
        cout << (int)c << " ";
    }
    cout << "]" << endl;
}

int main() {
    // Исходное сообщение (должно иметь четную длину)
    string message = "Secret!!";
    vector<unsigned char> block(message.begin(), message.end());

    cout << "Original:  ";
    print_vector(block);

    // Шифрование
    vector<unsigned char> encrypted = feistel_encrypt(block);
    cout << "Encrypted: ";
    print_vector(encrypted);

    // Дешифрование
    vector<unsigned char> decrypted = feistel_decrypt(encrypted);
    cout << "Decrypted: ";
    print_vector(decrypted);

    // Проверка
    cout << "Success: " << (block == decrypted ? "YES" : "NO") << endl;

    return 0;
}