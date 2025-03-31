#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <bitset>

using namespace std;

// Вывод вектора в hex с разделением
void print_hex(const vector<unsigned char>& data, const string& label = "") {
    if (!label.empty()) cout << label << ": ";
    for (unsigned char c : data) {
        cout << hex << setw(2) << setfill('0') << (int)c << " ";
    }
    cout << dec << endl;
}

// Вывод бинарного представления (для наглядности)
void print_binary(const vector<unsigned char>& data, const string& label = "") {
    if (!label.empty()) cout << label << ":\n";
    for (unsigned char c : data) {
        cout << bitset<8>(c) << " ";
    }
    cout << endl;
}

class FeistelCipher {
public:
    static vector<unsigned char> process(const vector<unsigned char>& block, 
                                       const vector<unsigned char>& keys, 
                                       bool decrypt = false,
                                       bool verbose = true) {
        if (block.size() % 2 != 0) {
            throw runtime_error("Block size must be even");
        }

        size_t half = block.size() / 2;
        vector<unsigned char> left(block.begin(), block.begin() + half);
        vector<unsigned char> right(block.begin() + half, block.end());

        if (verbose) {
            cout << "\n=== Initial Block ===" << endl;
            print_hex(left, "Left ");
            print_hex(right, "Right");
            print_binary(left, "Left (binary)");
            print_binary(right, "Right (binary)");
        }

        for (size_t i = 0; i < keys.size(); i++) {
            size_t key_index = decrypt ? (keys.size() - 1 - i) : i;
            unsigned char key = keys[key_index];
            
            if (verbose) {
                cout << "\n=== Round " << (i+1) << " ===" << endl;
                cout << "Using Key: " << hex << (int)key << dec << endl;
            }

            vector<unsigned char> new_right(half);
            for (size_t j = 0; j < half; j++) {
                unsigned char f_result = round_function(right[j], key);
                new_right[j] = left[j] ^ f_result;
                
                if (verbose && half <= 4) { // Подробный вывод для небольших блоков
                    cout << "Byte " << j << ":\n";
                    cout << "  F(R=" << hex << (int)right[j] << ", K=" << (int)key 
                         << ") = (" << (int)right[j] << " + " << (int)key 
                         << ") % 256 = " << (int)f_result << dec << endl;
                    cout << "  L ^ F = " << (int)left[j] << " XOR " << (int)f_result 
                         << " = " << (int)new_right[j] << endl;
                }
            }

            if (verbose) {
                print_hex(new_right, "New Right");
                print_binary(new_right, "New Right (binary)");
            }

            left = right;
            right = new_right;

            if (verbose) {
                cout << "After swap:\n";
                print_hex(left, "Left ");
                print_hex(right, "Right");
            }
        }
        
        vector<unsigned char> result(right);
        result.insert(result.end(), left.begin(), left.end());
        
        if (verbose) {
            cout << "\n=== Final Result ===" << endl;
            print_hex(result, "Output");
        }
        
        return result;
    }

private:
    static unsigned char round_function(unsigned char data, unsigned char key) {
        return (data + key) % 256;
    }
};

int main() {
    const vector<unsigned char> keys = {0x2A, 0x54, 0x7E, 0xA8}; // 42, 84, 126, 168 в hex
    
    cout << "Feistel Cipher Demo (4 rounds)\n";
    cout << "Round Keys: ";
    for (auto k : keys) cout << hex << setw(2) << setfill('0') << (int)k << " ";
    cout << dec << endl;
    
    while (true) {
        cout << "\nOptions:\n1. Encrypt\n2. Decrypt\n3. Exit\nChoice: ";
        int choice;
        cin >> choice;
        cin.ignore();
        
        if (choice == 3) break;
        
        try {
            if (choice == 1 || choice == 2) {
                cout << "Enter text: ";
                string text;
                getline(cin, text);
                
                vector<unsigned char> data(text.begin(), text.end());
                if (data.size() % 2 != 0) data.push_back(' '); // Padding
                
                bool decrypt = (choice == 2);
                if (decrypt) {
                    // Преобразование hex-строки в байты
                    data.clear();
                    string byteStr;
                    for (char c : text) {
                        if (isxdigit(c)) byteStr += c;
                        if (byteStr.size() == 2) {
                            data.push_back(stoul(byteStr, nullptr, 16));
                            byteStr.clear();
                        }
                    }
                }
                
                cout << "\nProcessing...\n";
                vector<unsigned char> result = FeistelCipher::process(
                    data, keys, decrypt, true);
                
                if (!decrypt) {
                    cout << "\nEncrypted (hex): ";
                    print_hex(result);
                } else {
                    cout << "\nDecrypted text: \"";
                    for (unsigned char c : result) {
                        if (isprint(c)) cout << c;
                        else cout << '.'; // Непечатаемые символы
                    }
                    cout << "\"" << endl;
                }
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
    
    return 0;
}