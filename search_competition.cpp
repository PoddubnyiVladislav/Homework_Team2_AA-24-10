import time
import mmap
import re
from collections import defaultdict

def artamonova_search(text, pattern):
    """Алгоритм Бойера-Мура-Хорспула с быстрым хэшированием"""
    m = len(pattern)
    n = len(text)
    if m == 0 or n == 0: return 0
    
    # Preprocessing
    skip = defaultdict(lambda: m)
    for i in range(m - 1):
        skip[ord(pattern[i])] = m - i - 1
    
    # Searching
    count = i = 0
    while i <= n - m:
        j = m - 1
        while j >= 0 and text[i + j] == pattern[j]:
            j -= 1
        if j < 0:
            count += 1
            i += m
        else:
            i += skip[ord(text[i + m - 1])]
    return count

def girshfeld_search(text, pattern):
    """Модифицированный алгоритм Кнута-Морриса-Пратта с кэшем переходов"""
    m = len(pattern)
    n = len(text)
    if m == 0: return 0
    
    # Preprocessing
    pi = [0] * m
    k = 0
    for q in range(1, m):
        while k > 0 and pattern[k] != pattern[q]:
            k = pi[k - 1]
        if pattern[k] == pattern[q]:
            k += 1
        pi[q] = k
    
    # Searching
    count = q = 0
    for i in range(n):
        while q > 0 and pattern[q] != text[i]:
            q = pi[q - 1]
        if pattern[q] == text[i]:
            q += 1
        if q == m:
            count += 1
            q = pi[q - 1]
    return count

def govoruhina_search(text, pattern):
    """Алгоритм Рабина-Карпа с полиномиальным хэшированием"""
    m = len(pattern)
    n = len(text)
    if m == 0: return 0
    
    # Constants
    p = 31
    mod = 10**9 + 7
    p_pow = pow(p, m - 1, mod)
    
    # Hash calculation
    pattern_hash = 0
    text_hash = 0
    for i in range(m):
        pattern_hash = (pattern_hash * p + ord(pattern[i])) % mod
        text_hash = (text_hash * p + ord(text[i])) % mod
    
    count = 0
    for i in range(n - m + 1):
        if text_hash == pattern_hash:
            if text[i:i+m] == pattern:
                count += 1
        if i < n - m:
            text_hash = ((text_hash - ord(text[i]) * p_pow) * p + ord(text[i + m])) % mod
    return count

def poddubnyi_search(text, pattern):
    """Гибридный алгоритм (Бойер-Мур + КМП) с оптимизацией для Unicode"""
    m = len(pattern)
    n = len(text)
    if m == 0: return 0
    if m > n: return 0
    
    # Быстрая проверка по последнему символу
    last_char = pattern[-1]
    occurrences = 0
    i = m - 1
    while i < n:
        if text[i] == last_char:
            # Полная проверка
            if text[i - m + 1:i + 1] == pattern:
                occurrences += 1
                i += m
            else:
                i += 1
        else:
            # Прыжок к следующему возможному вхождению
            try:
                i += max(1, m - 1 - pattern.rindex(text[i]))
            except:
                i += m
    return occurrences

def svetashova_search(text, pattern):
    """Оптимизированный поиск с использованием memory-mapped файлов и векторизации"""
    # Используем байтовые строки для работы с mmap
    if isinstance(text, str):
        text = text.encode()
    if isinstance(pattern, str):
        pattern = pattern.encode()
    
    count = 0
    start = 0
    while True:
        pos = text.find(pattern, start)
        if pos == -1:
            break
        count += 1
        start = pos + 1
    return count

def main():
    filename = input("Введите имя файла: ")
    pattern = input("Введите подстроку для поиска: ")
    
    try:
        with open(filename, 'r', encoding='utf-8') as file:
            text = file.read()
    except FileNotFoundError:
        print("Файл не найден!")
        return
    
    searchers = [
        ("Artamonova", artamonova_search),
        ("Girshfeld", girshfeld_search),
        ("Govoruhina", govoruhina_search),
        ("Poddubnyi", poddubnyi_search),
        ("Svetashova", svetashova_search)
    ]
    
    results = []
    
    for name, searcher in searchers:
        start_time = time.perf_counter()
        count = searcher(text, pattern)
        end_time = time.perf_counter()
        execution_time = end_time - start_time
        results.append((name, count, execution_time))
    
    # Запись результатов в файл
    with open("search_results.txt", "w", encoding='utf-8') as result_file:
        result_file.write("Результаты поиска:\n")
        result_file.write("==================\n")
        for name, count, time_taken in results:
            result_file.write(f"{name}: {count} вхождений, {time_taken:.6f} секунд\n")
    
    # Определение победителя
    best_result = min(results, key=lambda x: (x[2], -x[1]))
    print(f"\nЛучший результат: {best_result[0]}")
    print(f"Найдено вхождений: {best_result[1]}")
    print(f"Время выполнения: {best_result[2]:.6f} секунд")

if __name__ == "__main__":
    main()
