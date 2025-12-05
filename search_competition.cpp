#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <climits>

using namespace std;
using namespace std::chrono;

// Вспомогательная функция для чтения файла
string read_file(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filepath);
    }
    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return text;
}

// Алгоритм Бойера-Мура
pair<bool, vector<int>> boyer_moore_search(const string& filepath, const string& pattern, bool output = false) {
    vector<int> indices;

    if (pattern.empty()) {
        return make_pair(false, indices);
    }

    string text = read_file(filepath);
    if (output) cout << "Boyer-Moore: text length " << text.length() << ", pattern length " << pattern.length() << endl;

    int n = (int)text.length();
    int m = (int)pattern.length();

    if (n < m) {
        return make_pair(false, indices);
    }

    // Таблица плохого символа
    unordered_map<char, int> badChar;
    for (int i = 0; i < m; i++) {
        badChar[pattern[i]] = i;
    }

    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;

        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }

        if (j < 0) {
            indices.push_back(s);
            if (s + m < n && badChar.find(text[s + m]) != badChar.end()) {
                s += m - badChar[text[s + m]];
            }
            else {
                s += 1;
            }
        }
        else {
            if (badChar.find(text[s + j]) != badChar.end()) {
                s += max(1, j - badChar[text[s + j]]);
            }
            else {
                s += j + 1;
            }
        }
    }

    return make_pair(!indices.empty(), indices);
}

// Алгоритм Кнута-Морриса-Пратта
pair<bool, vector<int>> kmp_search(const string& filepath, const string& pattern, bool output = false) {
    vector<int> indices;

    if (pattern.empty()) {
        return make_pair(false, indices);
    }

    string text = read_file(filepath);
    if (output) cout << "KMP: text length " << text.length() << ", pattern length " << pattern.length() << endl;

    int n = (int)text.length();
    int m = (int)pattern.length();

    // Построение префикс-функции
    vector<int> lps(m, 0);
    int len = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    // Поиск
    i = 0;
    int j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }

        if (j == m) {
            indices.push_back(i - j);
            j = lps[j - 1];
        }
        else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }

    return make_pair(!indices.empty(), indices);
}

// Алгоритм Рабина-Карпа
pair<bool, vector<int>> rabin_karp_search(const string& filepath, const string& pattern, bool output = false) {
    vector<int> indices;

    if (pattern.empty()) {
        return make_pair(false, indices);
    }

    string text = read_file(filepath);
    if (output) cout << "Rabin-Karp: text length " << text.length() << ", pattern length " << pattern.length() << endl;

    int n = (int)text.length();
    int m = (int)pattern.length();

    if (n < m) {
        return make_pair(false, indices);
    }

    const int prime = 101;
    const int d = 256;

    int patternHash = 0;
    int textHash = 0;
    int h = 1;

    for (int i = 0; i < m - 1; i++) {
        h = (h * d) % prime;
    }


    for (int i = 0; i < m; i++) {
        patternHash = (d * patternHash + pattern[i]) % prime;
        textHash = (d * textHash + text[i]) % prime;
    }

    for (int i = 0; i <= n - m; i++) {
        if (patternHash == textHash) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                indices.push_back(i);
            }
        }

        if (i < n - m) {
            textHash = (d * (textHash - text[i] * h) + text[i + m]) % prime;
            if (textHash < 0) {
                textHash += prime;
            }
        }
    }

    return make_pair(!indices.empty(), indices);
}

// Гибридный алгоритм (Бойер-Мур + КМП)
pair<bool, vector<int>> hybrid_search(const string& filepath, const string& pattern, bool output = false) {
    vector<int> indices;

    if (pattern.empty()) {
        return make_pair(false, indices);
    }

    string text = read_file(filepath);
    if (output) cout << "Hybrid: text length " << text.length() << ", pattern length " << pattern.length() << endl;

    int n = (int)text.length();
    int m = (int)pattern.length();

    if (n < m) {
        return make_pair(false, indices);
    }

    // Таблица плохого символа (Бойер-Мур)
    unordered_map<char, int> badChar;
    for (int i = 0; i < m; i++) {
        badChar[pattern[i]] = i;
    }

    // Префикс-функция (КМП)
    vector<int> lps(m, 0);
    int len = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else {
            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    // Гибридный поиск
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;

        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }

        if (j < 0) {
            indices.push_back(s);
            // Используем больший сдвиг из двух алгоритмов
            int boyer_shift = 1;
            if (s + m < n && badChar.find(text[s + m]) != badChar.end()) {
                boyer_shift = m - badChar[text[s + m]];
            }
            int kmp_shift = (m > 0) ? m - lps[m - 1] : 1;
            s += max(boyer_shift, kmp_shift);
        }
        else {
            // Используем больший сдвиг из двух алгоритмов
            int boyer_shift = 1;
            if (badChar.find(text[s + j]) != badChar.end()) {
                boyer_shift = max(1, j - badChar[text[s + j]]);
            }
            int kmp_shift = (j > 0) ? j - lps[j - 1] : 1;
            s += max(boyer_shift, kmp_shift);
        }
    }

    return make_pair(!indices.empty(), indices);
}

// Структура для хранения результатов алгоритма
struct AlgorithmResult {
    string name;
    long long time_microseconds;
    int occurrences;
    vector<int> positions;

    // Для сортировки по времени
    bool operator<(const AlgorithmResult& other) const {
        return time_microseconds < other.time_microseconds;
    }
};

int main() {
    string filename, search_term;

    cout << "=== ADVANCED STRING SEARCH ALGORITHMS COMPARISON ===" << endl;
    cout << "Enter text file path: ";
    cin >> filename;
    cout << "Enter text/numbers to search: ";
    cin.ignore();
    getline(cin, search_term);

    cout << "\n=== SEARCHING FOR: '" << search_term << "' ===" << endl;
    cout << "File: " << filename << endl;
    cout << "====================================================\n" << endl;


    // Список алгоритмов - используем простой подход без сложных шаблонов
    vector<pair<string, function<pair<bool, vector<int>>(const string&, const string&, bool)>>> algorithms;
    algorithms.push_back(make_pair("Boyer-Moore", boyer_moore_search));
    algorithms.push_back(make_pair("Knuth-Morris-Pratt", kmp_search));
    algorithms.push_back(make_pair("Rabin-Karp", rabin_karp_search));
    algorithms.push_back(make_pair("Hybrid (Boyer-Moore + KMP)", hybrid_search));

    vector<AlgorithmResult> results;

    // Тестирование каждого алгоритма
    for (size_t idx = 0; idx < algorithms.size(); ++idx) {
        string name = algorithms[idx].first;
        auto search_func = algorithms[idx].second;
        
        try {
            cout << "--- " << name << " ---" << endl;

            auto start_time = high_resolution_clock::now();
            pair<bool, vector<int>> result_pair = search_func(filename, search_term, false);
            auto end_time = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end_time - start_time);

            AlgorithmResult result;
            result.name = name;
            result.time_microseconds = duration.count();
            result.occurrences = (int)result_pair.second.size();
            result.positions = result_pair.second;

            results.push_back(result);

            // Вывод детальной информации
            cout << "Found: " << (result_pair.first ? "YES" : "NO") << endl;
            cout << "Occurrences: " << result_pair.second.size() << endl;
            cout << "Positions: ";
            if (result_pair.second.empty()) {
                cout << "[]";
            }
            else {
                cout << "[";
                for (size_t i = 0; i < result_pair.second.size() && i < 10; ++i) {
                    cout << result_pair.second[i];
                    if (i < result_pair.second.size() - 1 && i < 9) cout << ", ";
                }
                if (result_pair.second.size() > 10) {
                    cout << ", ... (+" << result_pair.second.size() - 10 << " more)";
                }
                cout << "]";
            }
            cout << "\nTime: " << duration.count() << " microseconds" << endl;
            cout << endl;

        }
        catch (const exception& e) {
            cerr << "ERROR in " << name << ": " << e.what() << endl;

            // Добавляем результат с ошибкой
            AlgorithmResult error_result;
            error_result.name = name;
            error_result.time_microseconds = LLONG_MAX;
            error_result.occurrences = 0;
            results.push_back(error_result);

            cout << endl;
        }
    }

    // Сортировка результатов по времени (от быстрейшего к медленному)
    sort(results.begin(), results.end());

    // Вывод итоговой таблицы
    cout << "=== FINAL RESULTS (FASTEST TO SLOWEST) ===" << endl;
    cout << "===========================================" << endl;
    cout << "Rank | Algorithm               | Time (us) | Occurrences" << endl;
    cout << "-----|-------------------------|-----------|------------" << endl;

    for (size_t i = 0; i < results.size(); i++) {
        const AlgorithmResult& result = results[i];

        string rank;
        if (i == 0) {
            rank = "1st ";
        }
        else if (i == 1) {
            rank = "2nd ";
        }
        else if (i == 2) {
            rank = "3rd ";
        }
        else {
            rank = to_string(i + 1) + "th ";
        }


        cout << rank << " | ";
        // Используем простой вывод вместо printf
        cout << result.name;
        // Добавляем пробелы для выравнивания
        for (int j = (int)result.name.length(); j < 23; j++) cout << " ";
        cout << " | ";
        
        if (result.time_microseconds == LLONG_MAX) {
            cout << "    ERROR";
        }
        else {
            // Выравниваем время
            string time_str = to_string(result.time_microseconds);
            for (int j = (int)time_str.length(); j < 9; j++) cout << " ";
            cout << time_str;
        }
        
        cout << " | ";
        
        // Выравниваем occurrences
        string occ_str = to_string(result.occurrences);
        for (int j = (int)occ_str.length(); j < 10; j++) cout << " ";
        cout << occ_str << endl;
    }

    // Дополнительная информация о лучшем алгоритме
    if (!results.empty() && results[0].time_microseconds != LLONG_MAX) {
        cout << "\nBEST PERFORMANCE: " << results[0].name << endl;
        cout << "Time: " << results[0].time_microseconds << " microseconds" << endl;
        cout << "Occurrences found: " << results[0].occurrences << endl;

        if (results[0].occurrences > 0) {
            cout << "First 5 positions: [";
            size_t count = min(results[0].positions.size(), size_t(5));
            for (size_t i = 0; i < count; ++i) {
                cout << results[0].positions[i];
                if (i < count - 1) cout << ", ";
            }
            cout << "]" << endl;
        }
    }

    // Анализ производительности
    cout << "\n=== PERFORMANCE ANALYSIS ===" << endl;
    if (results.size() >= 2 && results[0].time_microseconds != LLONG_MAX) {
        long long fastest_time = results[0].time_microseconds;

        for (size_t i = 1; i < results.size(); i++) {
            if (results[i].time_microseconds != LLONG_MAX) {
                double ratio = static_cast<double>(results[i].time_microseconds) / fastest_time;
                cout << results[i].name << " is " << ratio << "x slower than " << results[0].name << endl;
            }
        }
    }

    cout << "\nPress Enter to exit...";
    cin.get();
    return 0;
}
