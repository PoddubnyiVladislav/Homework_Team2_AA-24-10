#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

using namespace std;
using namespace std::chrono;

// Заглушки функций участников (4 участника)
pair<bool, vector<int>> participant1(const string& filepath, const string& word, bool output = false) {
    // TODO: Участник 1 реализует здесь свой алгоритм поиска
    vector<int> dummy_indices;
    if (output) cout << "Participant1: stub function" << endl;
    return {false, dummy_indices};
}

pair<bool, vector<int>> participant2(const string& filepath, const string& word, bool output = false) {
    // TODO: Участник 2 реализует здесь свой алгоритм поиска
    vector<int> dummy_indices;
    if (output) cout << "Participant2: stub function" << endl;
    return {false, dummy_indices};
}

pair<bool, vector<int>> participant3(const string& filepath, const string& word, bool output = false) {
    // TODO: Участник 3 реализует здесь свой алгоритм поиска
    vector<int> dummy_indices;
    if (output) cout << "Participant3: stub function" << endl;
    return {false, dummy_indices};
}

pair<bool, vector<int>> participant4(const string& filepath, const string& word, bool output = false) {
    // TODO: Участник 4 реализует здесь свой алгоритм поиска
    vector<int> dummy_indices;
    if (output) cout << "Participant4: stub function" << endl;
    return {false, dummy_indices};
}

int main() {
    string filename, word;
    
    // Ввод данных
    cout << "Enter file path: ";
    cin >> filename;
    cout << "Enter word/phrase to search: ";
    cin.ignore(); // очищаем буфер
    getline(cin, word);
    
    cout << "\n=== SEARCH RESULTS ===" << endl;
    cout << "File: " << filename << endl;
    cout << "Search term: " << word << endl;
    cout << "========================\n" << endl;

    // Список участников и их функций
    vector<pair<string, function<pair<bool, vector<int>>(const string&, const string&, bool)>>> participants = {
        {"Algorithm 1 (Participant 1)", participant1},
        {"Algorithm 2 (Participant 2)", participant2},
        {"Algorithm 3 (Participant 3)", participant3},
        {"Algorithm 4 (Participant 4)", participant4}
    };

    // Тестирование каждой функции
    for (auto& [name, search_func] : participants) {
        try {
            cout << "--- " << name << " ---" << endl;
            
            auto start_time = high_resolution_clock::now();
            auto [found, indices] = search_func(filename, word, false);
            auto end_time = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end_time - start_time);
            
            // Вывод результатов
            cout << "Found: " << (found ? "TRUE" : "FALSE") << endl;
            cout << "Indices: ";
            if (indices.empty()) {
                cout << "[]";
            } else {
                cout << "[";
                for (size_t i = 0; i < indices.size(); ++i) {
                    cout << indices[i];
                    if (i < indices.size() - 1) cout << ", ";
                }
                cout << "]";
            }
            cout << "\nTime: " << duration.count() << " microseconds" << endl;
            cout << endl;
            
        } catch (const exception& e) {
            cerr << "ERROR in " << name << ": " << e.what() << endl;
            cout << endl;
        }
    }

    return 0;
}
