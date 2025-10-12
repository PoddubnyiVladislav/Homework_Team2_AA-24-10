#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

// Прототипы функций
int artamonova_search(const string& text, const string& pattern);
int girshfeld_search(const string& text, const string& pattern);
int govoruhina_search(const string& text, const string& pattern);
int poddubnyi_search(const string& text, const string& pattern);
int svetashova_search(const string& text, const string& pattern);

struct SearchResult {
    string name;
    int count;
    double time_ms;
};

// Заглушки функций (каждый участник заменит своей реализацией)

int artamonova_search(const string& text, const string& pattern) {
    // Артамонова: Алгоритм Бойера-Мура
    // TODO: Заменить на оптимизированную реализацию
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(pattern, pos)) != string::npos) {
        count++;
        pos += pattern.length();
    }
    return count;
}

int girshfeld_search(const string& text, const string& pattern) {
    // Гиршфельд: Алгоритм Кнута-Морриса-Пратта
    // TODO: Заменить на оптимизированную реализацию
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(pattern, pos)) != string::npos) {
        count++;
        pos += 1; // Для перекрывающихся вхождений
    }
    return count;
}

int govoruhina_search(const string& text, const string& pattern) {
    // Говорухина: Алгоритм Рабина-Карпа
    // TODO: Заменить на оптимизированную реализацию
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(pattern, pos)) != string::npos) {
        count++;
        pos += pattern.length();
    }
    return count;
}

int poddubnyi_search(const string& text, const string& pattern) {
    // Поддубный: Гибридный алгоритм (Бойер-Мур + КМП)
    // TODO: Заменить на оптимизированную реализацию
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(pattern, pos)) != string::npos) {
        count++;
        pos += pattern.length();
    }
    return count;
}

int svetashova_search(const string& text, const string& pattern) {
    // Светашова: Оптимизированный поиск с memory mapping
    // TODO: Заменить на оптимизированную реализацию
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(pattern, pos)) != string::npos) {
        count++;
        pos += 1;
    }
    return count;
}

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    
    string content((istreambuf_iterator<char>(file)),
                   istreambuf_iterator<char>());
    file.close();
    return content;
}

void writeResultsToFile(const vector<SearchResult>& results, const string& pattern) {
    ofstream outFile("search_results.txt");
    if (!outFile.is_open()) {
        cerr << "Не удалось создать файл результатов!" << endl;
        return;
    }
    
    outFile << "Результаты поиска подстроки: \"" << pattern << "\"" << endl;
    outFile << "==========================================" << endl;
    
    for (const auto& result : results) {
        outFile << result.name << ": " << result.count 
                << " вхождений, " << result.time_ms << " мс" << endl;
    }
    outFile.close();
}

int main() {
    string filename, pattern;
    
    cout << "Введите имя файла: ";
    cin >> filename;
    cout << "Введите подстроку для поиска: ";
    cin >> pattern;
    
    try {
        string text = readFile(filename);
        cout << "Файл загружен. Размер: " << text.length() << " символов" << endl;
        
        vector<pair<string, function<int(const string&, const string&)>>> searchers = {
            {"Artamonova", artamonova_search},
            {"Girshfeld", girshfeld_search},
            {"Govoruhina", govoruhina_search},
            {"Poddubnyi", poddubnyi_search},
            {"Svetashova", svetashova_search}
        };
        
        vector<SearchResult> results;
        
        for (const auto& searcher : searchers) {
            auto start = high_resolution_clock::now();
            int count = searcher.second(text, pattern);
            auto end = high_resolution_clock::now();
            
            double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
            
            results.push_back({searcher.first, count, time_ms});
            
            cout << searcher.first << ": " << count << " вхождений, " 
                 << time_ms << " мс" << endl;
        }
        
        // Запись результатов в файл
        writeResultsToFile(results, pattern);
        
        // Определение победителя
        auto best = min_element(results.begin(), results.end(),
            [](const SearchResult& a, const SearchResult& b) {
                return a.time_ms < b.time_ms;
            });
        
        cout << "\nЛучший результат: " << best->name << endl;
        cout << "Время: " << best->time_ms << " мс" << endl;
        cout << "Найдено вхождений: " << best->count << endl;
        cout << "\nПолные результаты сохранены в search_results.txt" << endl;
        
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
