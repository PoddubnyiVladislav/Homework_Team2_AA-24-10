#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <queue>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct SearchResult {
    string algorithm;
    size_t count;  // изменил int на size_t
    vector<size_t> positions;
    long long duration;
};

// Линейный поиск
SearchResult linearSearch(const string& text, const string& pattern) {
    auto start = high_resolution_clock::now();
    vector<size_t> positions;
    size_t n = text.length();
    size_t m = pattern.length();

    if (m == 0 || m > n) {
        auto end = high_resolution_clock::now();
        return { "Linear Search", 0, positions, duration_cast<microseconds>(end - start).count() };
    }

    for (size_t i = 0; i <= n - m; ++i) {
        bool found = true;
        for (size_t j = 0; j < m; ++j) {
            if (text[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            positions.push_back(i);
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    return { "Linear Search", positions.size(), positions, duration.count() };
}

// Алгоритм Кнута-Морриса-Пратта
vector<int> computeLPS(const string& pattern) {
    int m = static_cast<int>(pattern.length());
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
    return lps;
}

SearchResult kmpSearch(const string& text, const string& pattern) {
    auto start = high_resolution_clock::now();
    vector<size_t> positions;

    if (pattern.empty()) {
        auto end = high_resolution_clock::now();
        return { "KMP", 0, positions, duration_cast<microseconds>(end - start).count() };
    }

    int n = static_cast<int>(text.length());
    int m = static_cast<int>(pattern.length());
    vector<int> lps = computeLPS(pattern);
    int i = 0, j = 0;

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            positions.push_back(static_cast<size_t>(i - j));
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

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    return { "KMP", positions.size(), positions, duration.count() };
}

// Алгоритм Бойера-Мура
vector<int> buildBadCharTable(const string& pattern) {
    const int ALPHABET_SIZE = 256;
    vector<int> badChar(ALPHABET_SIZE, -1);
    for (size_t i = 0; i < pattern.length(); i++) {
        badChar[static_cast<unsigned char>(pattern[i])] = static_cast<int>(i);
    }
    return badChar;
}

SearchResult boyerMooreSearch(const string& text, const string& pattern) {
    auto start = high_resolution_clock::now();
    vector<size_t> positions;

    if (pattern.empty()) {
        auto end = high_resolution_clock::now();
        return { "Boyer-Moore", 0, positions, duration_cast<microseconds>(end - start).count() };
    }

    int n = static_cast<int>(text.length());
    int m = static_cast<int>(pattern.length());
    vector<int> badChar = buildBadCharTable(pattern);
    int s = 0;

    while (s <= n - m) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }
        if (j < 0) {
            positions.push_back(static_cast<size_t>(s));
            s += (s + m < n) ? m - badChar[static_cast<unsigned char>(text[s + m])] : 1;
        }
        else {
            s += max(1, j - badChar[static_cast<unsigned char>(text[s + j])]);
        }
    }


    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    return { "Boyer-Moore", positions.size(), positions, duration.count() };
}

// Алгоритм Рабина-Карпа
SearchResult rabinKarpSearch(const string& text, const string& pattern) {
    auto start = high_resolution_clock::now();
    vector<size_t> positions;

    if (pattern.empty()) {
        auto end = high_resolution_clock::now();
        return { "Rabin-Karp", 0, positions, duration_cast<microseconds>(end - start).count() };
    }

    int n = static_cast<int>(text.length());
    int m = static_cast<int>(pattern.length());
    const int prime = 101;

    long long patternHash = 0;
    long long textHash = 0;
    long long h = 1;

    for (int i = 0; i < m - 1; i++) {
        h = (h * 256) % prime;
    }

    for (int i = 0; i < m; i++) {
        patternHash = (256 * patternHash + pattern[i]) % prime;
        textHash = (256 * textHash + text[i]) % prime;
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
                positions.push_back(static_cast<size_t>(i));
            }
        }

        if (i < n - m) {
            textHash = (256 * (textHash - text[i] * h) + text[i + m]) % prime;
            if (textHash < 0) {
                textHash += prime;
            }
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    return { "Rabin-Karp", positions.size(), positions, duration.count() };
}

// Алгоритм Ахо-Корасик
struct AhoCorasickNode {
    map<char, AhoCorasickNode*> children;
    AhoCorasickNode* suffixLink;
    AhoCorasickNode* outputLink;
    vector<size_t> patternIndexes;  // изменил int на size_t
};

AhoCorasickNode* createNode() {
    AhoCorasickNode* node = new AhoCorasickNode();
    node->suffixLink = nullptr;
    node->outputLink = nullptr;
    return node;
}

void buildTrie(AhoCorasickNode* root, const vector<string>& patterns) {
    for (size_t i = 0; i < patterns.size(); i++) {
        AhoCorasickNode* current = root;
        for (char c : patterns[i]) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = createNode();
            }
            current = current->children[c];
        }
        current->patternIndexes.push_back(i);
    }
}

void buildSuffixAndOutputLinks(AhoCorasickNode* root) {
    queue<AhoCorasickNode*> q;
    for (auto& pair : root->children) {
        pair.second->suffixLink = root;
        q.push(pair.second);
    }

    while (!q.empty()) {
        AhoCorasickNode* current = q.front();
        q.pop();

        for (auto& pair : current->children) {
            char c = pair.first;
            AhoCorasickNode* child = pair.second;
            AhoCorasickNode* temp = current->suffixLink;

            while (temp != nullptr && temp->children.find(c) == temp->children.end()) {
                temp = temp->suffixLink;
            }

            child->suffixLink = (temp != nullptr) ? temp->children[c] : root;
            child->outputLink = (!child->suffixLink->patternIndexes.empty()) ?
                child->suffixLink : child->suffixLink->outputLink;

            q.push(child);
        }
    }
}

SearchResult ahoCorasickSearch(const string& text, const vector<string>& patterns) {
    auto start = high_resolution_clock::now();
    vector<size_t> positions;

    if (patterns.empty() || patterns[0].empty()) {
        auto end = high_resolution_clock::now();
        return { "Aho-Corasick", 0, positions, duration_cast<microseconds>(end - start).count() };
    }

    AhoCorasickNode* root = createNode();
    buildTrie(root, patterns);
    buildSuffixAndOutputLinks(root);


    AhoCorasickNode* current = root;
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        while (current != nullptr && current->children.find(c) == current->children.end()) {
            current = current->suffixLink;
        }

        if (current == nullptr) {
            current = root;
            continue;
        }

        current = current->children[c];

        for (size_t patternIndex : current->patternIndexes) {
            positions.push_back(i - patterns[patternIndex].length() + 1);
        }

        AhoCorasickNode* output = current->outputLink;
        while (output != nullptr) {
            for (size_t patternIndex : output->patternIndexes) {
                positions.push_back(i - patterns[patternIndex].length() + 1);
            }
            output = output->outputLink;
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    // Освобождение памяти
    queue<AhoCorasickNode*> freeQueue;
    freeQueue.push(root);
    while (!freeQueue.empty()) {
        AhoCorasickNode* node = freeQueue.front();
        freeQueue.pop();
        for (auto& pair : node->children) {
            freeQueue.push(pair.second);
        }
        delete node;
    }

    return { "Aho-Corasick", positions.size(), positions, duration.count() };
}

// Вспомогательные функции
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Couldn't open the file");
    }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

void printResults(const vector<SearchResult>& results) {
    vector<SearchResult> sortedResults = results;
    sort(sortedResults.begin(), sortedResults.end(),
        [](const SearchResult& a, const SearchResult& b) {
            return a.duration < b.duration;
        });

    cout << "\nSearch results (sorted by time):\n";
    for (const auto& result : sortedResults) {
        cout << "\n Algorithm: " << result.algorithm << endl;
        cout << "Time: " << result.duration << " microseconds" << endl;
        cout << "Number of occurrences: " << result.count << endl;

        if (result.count > 0) {
            cout << "The first 5 positions: ";
            for (size_t i = 0; i < min(static_cast<size_t>(5), result.count); i++) {
                cout << result.positions[i] << " ";
            }
            if (result.count > 5) cout << "...";
            cout << endl;
        }
    }
}

int main() {
    try {
        string filename;
        cout << "Enter the file name: ";
        cin >> filename;

        string text = readFile(filename);
        cout << "The file was read successfully. Text size: " << text.length() << " characters\n";

        string pattern;
        cout << "Enter the search template: ";
        cin.ignore();
        getline(cin, pattern);

        vector<SearchResult> results;

        // Запуск различных алгоритмов поиска
        results.push_back(linearSearch(text, pattern));
        results.push_back(kmpSearch(text, pattern));
        results.push_back(boyerMooreSearch(text, pattern));
        results.push_back(rabinKarpSearch(text, pattern));
        results.push_back(ahoCorasickSearch(text, { pattern }));

        printResults(results);

    }
    catch (const exception& e) {
        cerr << "Mistake: " << e.what() << endl;
        return 1;
    }

    return 0;
}
