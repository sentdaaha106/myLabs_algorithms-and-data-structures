#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <queue>
#include <fstream>
#include <string>
#include <chrono>

// Очередь с приоритетами на основе отсортированного массива
template<typename T>
class PriorityQueueSorted {
private:
    std::deque<T> data;  // deque удобен для вставки и удаления с концов

public:
    // Добавление элемента с сохранением порядка
    void push(const T& value) {
        // Ищем позицию для вставки (бинарный поиск)
        auto it = std::lower_bound(data.begin(), data.end(), value);
        data.insert(it, value);
    }

    // Удаление максимального элемента (в конце для std::less)
    void pop() {
        if (!empty()) data.pop_back();
    }

    // Получение максимального элемента
    T top() const {
        return data.back();
    }

    // Создание очереди из массива (просто сортируем)
    void build_from_array(const std::vector<T>& arr) {
        data = std::deque<T>(arr.begin(), arr.end());
        std::sort(data.begin(), data.end());
    }

    // Слияние двух очередей (объединение отсортированных массивов)
    void merge(PriorityQueueSorted& other) {
        std::deque<T> merged;
        merged.resize(data.size() + other.data.size());
        std::merge(data.begin(), data.end(),
            other.data.begin(), other.data.end(),
            merged.begin());
        data = merged;
        other.data.clear();
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }

    // Оператор вывода
    friend std::ostream& operator<<(std::ostream& os, const PriorityQueueSorted& pq) {
        for (const auto& x : pq.data) os << x << " ";
        return os;
    }
};

// Очередь с приоритетами на основе бинарной кучи
template<typename T>
class PriorityQueueHeap {
private:
    std::deque<T> heap;

    // Просеивание вверх (восстановление кучи после добавления)
    void sift_up(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            // Если текущий элемент больше родительского, меняем
            if (heap[parent] < heap[i]) {
                std::swap(heap[parent], heap[i]);
                i = parent;
            }
            else break;
        }
    }

    // Просеивание вниз (восстановление кучи после удаления)
    void sift_down(int i) {
        int n = heap.size();
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            // Ищем максимальный среди i, left, right
            if (left < n && heap[left] > heap[largest]) largest = left;
            if (right < n && heap[right] > heap[largest]) largest = right;

            // Если максимум не в корне, меняем и продолжаем
            if (largest != i) {
                std::swap(heap[i], heap[largest]);
                i = largest;
            }
            else break;
        }
    }

public:
    // Добавление элемента в кучу
    void push(const T& value) {
        heap.push_back(value);      // добавляем в конец
        sift_up(heap.size() - 1);   // восстанавливаем свойства кучи
    }

    // Удаление максимального элемента
    void pop() {
        if (empty()) return;
        std::swap(heap[0], heap.back());  // меняем корень с последним
        heap.pop_back();                   // удаляем последний
        if (!empty()) sift_down(0);        // восстанавливаем кучу
    }

    // Получение максимального элемента
    T top() const {
        return heap[0];  // корень кучи - максимальный
    }

    // Создание кучи из массива (эффективное построение за O(n))
    void build_from_array(const std::vector<T>& arr) {
        heap = std::deque<T>(arr.begin(), arr.end());
        // Начинаем с последнего нелистового узла
        for (int i = heap.size() / 2 - 1; i >= 0; --i) {
            sift_down(i);
        }
    }

    // Слияние двух куч (просто добавляем все элементы)
    void merge(PriorityQueueHeap& other) {
        for (const auto& x : other.heap) {
            push(x);
        }
        other.heap.clear();
    }

    bool empty() const { return heap.empty(); }
    size_t size() const { return heap.size(); }

    // Оператор вывода (показывает кучу, не отсортированную)
    friend std::ostream& operator<<(std::ostream& os, const PriorityQueueHeap& pq) {
        for (const auto& x : pq.heap) os << x << " ";
        return os;
    }
};

// Функция извлечения слов из текстового файла
std::vector<std::string> get_words(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> words;
    std::string word;
    char ch;

    // Читаем файл посимвольно
    while (file.get(ch)) {
        if (isalpha(ch)) {  // если буква - добавляем к текущему слову
            word += tolower(ch);
        }
        else if (!word.empty()) {  // если не буква и слово не пустое - сохраняем
            words.push_back(word);
            word.clear();
        }
    }
    return words;
}

// Поиск N самых длинных слов с помощью очереди с приоритетами
template<typename PQ>
std::vector<std::string> top_longest(const std::vector<std::string>& words, int n) {
    PQ pq;

    // Используем очередь как фильтр: храним только n самых длинных слов
    for (const auto& w : words) {
        if (pq.size() < n) {
            // Если еще не набрали n слов, просто добавляем
            pq.push(w);
        }
        else if (w.length() > pq.top().length()) {
            // Если текущее слово длиннее минимального в очереди, заменяем
            pq.pop();
            pq.push(w);
        }
    }

    // Извлекаем результат
    std::vector<std::string> result;
    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }
    return result;
}

// Функция для измерения времени выполнения
template<typename Func>
long long time_ms(Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Демонстрация работы очереди на отсортированном массиве
    PriorityQueueSorted<int> pq1;
    pq1.push(5);
    pq1.push(2);
    pq1.push(8);
    pq1.push(1);
    std::cout << "Отсортированная очередь: " << pq1 << std::endl;
    std::cout << "Максимум: " << pq1.top() << std::endl;
    pq1.pop();
    std::cout << "После удаления: " << pq1 << std::endl;

    // Демонстрация работы очереди на куче
    PriorityQueueHeap<int> pq2;
    pq2.push(5);
    pq2.push(2);
    pq2.push(8);
    pq2.push(1);
    std::cout << "\nКуча (не отсортирована): " << pq2 << std::endl;
    std::cout << "Максимум: " << pq2.top() << std::endl;

    // ТЕСТ ПРОИЗВОДИТЕЛЬНОСТИ - уменьшим количество элементов
    std::cout << "\nТЕСТ ПРОИЗВОДИТЕЛЬНОСТИ\n";
    const int N = 10000;  // Уменьшил с 100000 до 10000 для быстрого теста
    std::vector<int> test(N);
    for (int i = 0; i < N; i++) test[i] = rand() % N;

    std::cout << "Тест вставки " << N << " элементов...\n";

    // Измеряем время вставки для разных реализаций
    auto t1 = time_ms([&]() {
        PriorityQueueSorted<int> q;
        for (int x : test) q.push(x);
        });

    auto t2 = time_ms([&]() {
        PriorityQueueHeap<int> q;
        for (int x : test) q.push(x);
        });

    auto t3 = time_ms([&]() {
        std::priority_queue<int> q;
        for (int x : test) q.push(x);
        });

    std::cout << "Результаты (мс):\n";
    std::cout << "  Отсортированный массив: " << t1 << "\n";
    std::cout << "  Бинарная куча: " << t2 << "\n";
    std::cout << "  std::priority_queue: " << t3 << "\n";

    // Тест на реальном тексте
    std::cout << "\nПОИСК 20 САМЫХ ДЛИННЫХ СЛОВ\n";
    std::string filename;
    std::cout << "Введите путь к файлу (Enter для пропуска): ";
    std::getline(std::cin, filename);

    if (!filename.empty()) {
        std::cout << "Чтение файла...\n";
        auto words = get_words(filename);
        std::cout << "Найдено слов: " << words.size() << "\n";

        if (words.size() > 0) {
            // Сравниваем время поиска
            std::cout << "Поиск 20 самых длинных слов...\n";

            auto t4 = time_ms([&]() {
                auto res = top_longest<PriorityQueueSorted<std::string>>(words, 20);
                });

            auto t5 = time_ms([&]() {
                auto res = top_longest<PriorityQueueHeap<std::string>>(words, 20);
                });

            auto t6 = time_ms([&]() {
                auto cmp = [](const std::string& a, const std::string& b) {
                    return a.length() < b.length();
                    };
                std::priority_queue<std::string, std::vector<std::string>, decltype(cmp)> pq(cmp);
                for (const auto& w : words) {
                    if (pq.size() < 20) {
                        pq.push(w);
                    }
                    else if (w.length() > pq.top().length()) {
                        pq.pop();
                        pq.push(w);
                    }
                }
                });

            std::cout << "\nВремя поиска (мс):\n";
            std::cout << "  Отсортированный массив: " << t4 << "\n";
            std::cout << "  Бинарная куча: " << t5 << "\n";
            std::cout << "  std::priority_queue: " << t6 << "\n";

            // Выводим результат
            auto result = top_longest<PriorityQueueHeap<std::string>>(words, 20);
            std::sort(result.begin(), result.end(),
                [](const std::string& a, const std::string& b) {
                    return a.length() > b.length();
                });

            std::cout << "\n20 самых длинных слов:\n";
            for (size_t i = 0; i < result.size() && i < 20; i++) {
                std::cout << i + 1 << ". " << result[i] << " (" << result[i].length() << ")\n";
            }
        }
        else {
            std::cout << "В файле не найдено слов!\n";
        }
    }

    std::cout << "\nТест завершен.\n";
    return 0;
}