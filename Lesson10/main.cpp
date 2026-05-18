

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <set>
#include <map>
#include <string>

using namespace std;

// ЗАДАЧА 6


class GraphForPrim {
private:
    int vertices;                    // количество вершин
    vector<vector<pair<int, int>>> adj;  // списки смежности: для каждой вершины список пар (сосед, вес)

public:
    
    GraphForPrim(int n) : vertices(n), adj(n) {}

   
    void addEdge(int u, int v, int weight) {
        adj[u].push_back({ v, weight });
        adj[v].push_back({ u, weight });  // неориентированный граф
    }

    vector<tuple<int, int, int>> primMST() {
        // Вектор для хранения ребер MST
        vector<tuple<int, int, int>> mst;

        // Массивы для отслеживания состояния вершин
        vector<bool> inMST(vertices, false);     // входит ли вершина в MST
        vector<int> parent(vertices, -1);         // родительская вершина в MST
        vector<int> key(vertices, numeric_limits<int>::max()); // минимальный вес ребра к MST

        // Приоритетная очередь (min-heap): хранит пары (вес, вершина)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        // Начинаем с вершины 0
        key[0] = 0;
        pq.push({ 0, 0 });

        while (!pq.empty()) {
            // Извлекаем вершину с минимальным ключом
            int u = pq.top().second;
            pq.pop();

            // Если вершина уже в MST, пропускаем
            if (inMST[u]) continue;

            // Добавляем вершину в MST
            inMST[u] = true;

            // Если u не стартовая вершина, добавляем ребро (parent[u], u) в MST
            if (parent[u] != -1) {
                // Находим вес ребра между parent[u] и u
                int weight = 0;
                for (const auto& edge : adj[u]) {
                    if (edge.first == parent[u]) {
                        weight = edge.second;
                        break;
                    }
                }
                mst.push_back({ parent[u], u, weight });
            }

            // Обновляем ключи для всех соседей u
            for (const auto& neighbor : adj[u]) {
                int v = neighbor.first;
                int w = neighbor.second;

                // Если v не в MST и вес ребра (u,v) меньше текущего ключа v
                if (!inMST[v] && w < key[v]) {
                    key[v] = w;
                    parent[v] = u;
                    pq.push({ key[v], v });
                }
            }
        }

        return mst;
    }

    
    int getMSTWeight() {
        auto mst = primMST();
        int totalWeight = 0;
        for (const auto& edge : mst) {
            totalWeight += get<2>(edge);
        }
        return totalWeight;
    }

    
     // Вывод графа в консоль (для отладки)
     
    void printGraph() {
        cout << "Граф (списки смежности):\n";
        for (int i = 0; i < vertices; i++) {
            cout << "Вершина " << i << ": ";
            for (const auto& neighbor : adj[i]) {
                cout << "-> " << neighbor.first << "(" << neighbor.second << ") ";
            }
            cout << endl;
        }
    }
};

// ЗАДАЧА 11


class TanNetwork {
private:
    int numCities;                          // количество городов
    vector<tuple<int, int, int>> connections; // список возможных соединений (город1, город2, стоимость)
    map<string, int> cityToIndex;           // отображение названия города в индекс
    vector<string> indexToCity;              // отображение индекса в название города

public:
    
     //Конструктор сети Tan
     
    TanNetwork() : numCities(0) {}

    
     // Добавление города в сеть
     // @param cityName- название города
     
    void addCity(const string& cityName) {
        if (cityToIndex.find(cityName) == cityToIndex.end()) {
            cityToIndex[cityName] = numCities;
            indexToCity.push_back(cityName);
            numCities++;
        }
    }

    /**
     * Добавление возможного соединения между городами
     * @param city1- первый город
     * @param city2- второй город
     * @param cost- стоимость прокладки кабеля
     */
    void addConnection(const string& city1, const string& city2, int cost) {
        // Убеждаемся, что города добавлены
        addCity(city1);
        addCity(city2);

        // Добавляем соединение
        connections.push_back({ cityToIndex[city1], cityToIndex[city2], cost });
    }

    /**
     * Нахождение минимальной стоимости соединения всех городов
     * Используется алгоритм Прима (как в задаче 6)
     * @return структура с результатом: общая стоимость и список выбранных соединений
     */
    struct NetworkResult {
        int totalCost;
        vector<tuple<string, string, int>> selectedConnections;
        bool isConnected;
    };

    NetworkResult findMinimalNetwork() {
        NetworkResult result;
        result.totalCost = 0;
        result.isConnected = false;

        if (numCities == 0) {
            result.isConnected = true;
            return result;
        }

        // Создаем граф и добавляем все соединения
        GraphForPrim graph(numCities);
        for (const auto& conn : connections) {
            int u = get<0>(conn);
            int v = get<1>(conn);
            int cost = get<2>(conn);
            graph.addEdge(u, v, cost);
        }

        // Запускаем алгоритм Прима
        auto mstEdges = graph.primMST();

        // Проверяем, все ли вершины вошли в MST
        // Если количество ребер в MST равноnumCities- 1, то граф связный
        if (mstEdges.size() == numCities - 1 || numCities == 1) {
            result.isConnected = true;
            result.totalCost = graph.getMSTWeight();

            //Конвертируем ребра из индексов в названия городов
            for (const auto& edge : mstEdges) {
                int u = get<0>(edge);
                int v = get<1>(edge);
                int cost = get<2>(edge);
                result.selectedConnections.push_back({
                    indexToCity[u],
                    indexToCity[v],
                    cost
                    });
            }
        }

        return result;
    }

    /**
     * Вывод информации о сети
     */
    void printNetwork() {
        cout << "\n Информация о городах и соединениях \n";
        cout << "Городов: " << numCities << endl;
        cout << "Возможных соединений: " << connections.size() << endl;
        cout << "\nСписок городов:\n";
        for (int i = 0; i < numCities; i++) {
            cout << "  " << i << ". " << indexToCity[i] << endl;
        }
        cout << "\nВозможные соединения:\n";
        for (const auto& conn : connections) {
            cout << "  " << indexToCity[get<0>(conn)] << " -- "
                << indexToCity[get<1>(conn)] << " : " << get<2>(conn) << endl;
        }
    }
};

// ТЕСТЫ

//6
void testTask6() {
    cout << "\nТЕСТИРОВАНИЕ ЗАДАЧИ 6\n";

    // Тест 1: Простой граф из 4 вершин
    {
        cout << "\nТест 1: Простой граф (4 вершины)\n";
        GraphForPrim g(4);
        g.addEdge(0, 1, 10);
        g.addEdge(0, 2, 6);
        g.addEdge(0, 3, 5);
        g.addEdge(1, 3, 15);
        g.addEdge(2, 3, 4);

        g.printGraph();

        auto mst = g.primMST();
        cout << "\nМинимальное остовное дерево (ребра):\n";
        for (const auto& edge : mst) {
            cout << "  " << get<0>(edge) << " -- " << get<1>(edge)
                << " (вес: " << get<2>(edge) << ")\n";
        }
        cout << "Общий вес MST: " << g.getMSTWeight() << endl;

        // Ожидаемый результат: ребра (0-3, 3-2, 0-1) весом 5+4+10=19
        // или (2-3, 3-0, 0-1)-то же самое
        if (g.getMSTWeight() == 19) {
            cout << "? Тест 1 пройден!\n";
        }
        else {
            cout << "? Тест 1 не пройден! Ожидалось 19, получено " << g.getMSTWeight() << endl;
        }
    }

    // Тест 2: Граф с 5 вершинами (пример из лекций)
    {
        cout << "\nТест 2: Граф с 5 вершинами\n";
        GraphForPrim g(5);
        g.addEdge(0, 1, 2);
        g.addEdge(0, 3, 6);
        g.addEdge(1, 2, 3);
        g.addEdge(1, 3, 8);
        g.addEdge(1, 4, 5);
        g.addEdge(2, 4, 7);
        g.addEdge(3, 4, 9);

        int weight = g.getMSTWeight();
        cout << "Общий вес MST: " << weight << endl;

        // Ожидаемый MST: ребра (0-1:2, 1-2:3, 1-4:5, 0-3:6) = 16
        if (weight == 16) {
            cout << "? Тест 2 пройден!\n";
        }
        else {
            cout << "? Тест 2 не пройден! Ожидалось 16, получено " << weight << endl;
        }
    }

    // Тест 3: Граф с одной вершиной
    {
        cout << "\nТест 3: Граф с одной вершиной\n";
        GraphForPrim g(1);
        int weight = g.getMSTWeight();
        cout << "Вес MST для одной вершины: " << weight << endl;
        if (weight == 0) {
            cout << "? Тест 3 пройден!\n";
        }
        else {
            cout << "? Тест 3 не пройден! Ожидалось 0, получено " << weight << endl;
        }
    }
}

//11
void testTask11() {
    cout << "\n ТЕСТИРОВАНИЕ ЗАДАЧИ 11 \n";

    // Тест 1: Пример из условия (4 города)
    {
        cout << "\nТест 1: Сеть из 4 городов\n";
        TanNetwork network;

        network.addCity("Москва");
        network.addCity("Санкт-Петербург");
        network.addCity("Новгород");
        network.addCity("Тверь");

        network.addConnection("Москва", "Санкт-Петербург", 700);
        network.addConnection("Москва", "Новгород", 500);
        network.addConnection("Москва", "Тверь", 200);
        network.addConnection("Санкт-Петербург", "Новгород", 300);
        network.addConnection("Новгород", "Тверь", 400);

        network.printNetwork();

        auto result = network.findMinimalNetwork();

        cout << "\n РЕЗУЛЬТАТ \n";
        if (result.isConnected) {
            cout << "Сеть может быть построена!\n";
            cout << "Минимальная стоимость: " << result.totalCost << endl;
            cout << "Выбранные соединения:\n";
            for (const auto& conn : result.selectedConnections) {
                cout << "  " << get<0>(conn) << " -- " << get<1>(conn)
                    << " (стоимость: " << get<2>(conn) << ")\n";
            }

            // Ожидаемый результат: 200 + 300 + 400 = 900
            if (result.totalCost == 900) {
                cout << "? Тест 1 пройден!\n";
            }
            else {
                cout << "? Тест 1 не пройден! Ожидалось 900, получено " << result.totalCost << endl;
            }
        }
        else {
            cout << "Невозможно соединить все города!\n";
        }
    }

    // Тест 2: Полносвязный граф из 3 городов
    {
        cout << "\nТест 2: Треугольник из городов\n";
        TanNetwork network;

        network.addConnection("A", "B", 10);
        network.addConnection("B", "C", 20);
        network.addConnection("A", "C", 30);

        auto result = network.findMinimalNetwork();

        cout << "Минимальная стоимость: " << result.totalCost << endl;
        if (result.totalCost == 30) { // 10 + 20 = 30
            cout << "? Тест 2 пройден!\n";
        }
        else {
            cout << "? Тест 2 не пройден! Ожидалось 30, получено " << result.totalCost << endl;
        }
    }

    // Тест 3: Несвязный граф (невозможно соединить все города)
    {
        cout << "\nТест 3: Несвязный граф\n";
        TanNetwork network;

        network.addConnection("A", "B", 5);
        network.addConnection("C", "D", 10);
        // Город E вообще без соединений

        auto result = network.findMinimalNetwork();

        if (!result.isConnected) {
            cout << "? Тест 3 пройден - граф несвязный!\n";
        }
        else {
            cout << "? Тест 3 не пройден - граф должен быть несвязным!\n";
        }
    }
}

//Запуск тестов
void runAllTests() {
   
    cout << "ЗАПУСК ВСЕХ ТЕСТОВ";
    

    testTask6();
    testTask11();

    
    cout << "ТЕСТИРОВАНИЕ ЗАВЕРШЕНО";
}

//  ОСНОВНАЯ ФУНКЦИЯ

int main() {
    setlocale(LC_ALL, "Russian");

    // Демонстрация работы задачи 6
    cout << "\n[ДЕМОНСТРАЦИЯ ЗАДАЧИ 6: АЛГОРИТМ ПРИМА]\n";
    GraphForPrim demoGraph(6);
    demoGraph.addEdge(0, 1, 4);
    demoGraph.addEdge(0, 2, 3);
    demoGraph.addEdge(1, 2, 1);
    demoGraph.addEdge(1, 3, 2);
    demoGraph.addEdge(2, 3, 4);
    demoGraph.addEdge(3, 4, 2);
    demoGraph.addEdge(4, 5, 6);

    demoGraph.printGraph();
    auto mst = demoGraph.primMST();
    cout << "\nРезультат работы алгоритма Прима:\n";
    cout << "Ребра минимального остовного дерева:\n";
    for (const auto& edge : mst) {
        cout << "  " << get<0>(edge) << " -- " << get<1>(edge)
            << " (вес: " << get<2>(edge) << ")\n";
    }
    cout << "Общий вес: " << demoGraph.getMSTWeight() << endl;

    // Демонстрация работы задачи 11
    cout << "\n[ДЕМОНСТРАЦИЯ ЗАДАЧИ 11: TAN NETWORK]\n";
    TanNetwork tanDemo;

    // Пример из реальной жизни: города России
    tanDemo.addConnection("Москва", "Казань", 800);
    tanDemo.addConnection("Москва", "Нижний Новгород", 400);
    tanDemo.addConnection("Казань", "Екатеринбург", 900);
    tanDemo.addConnection("Нижний Новгород", "Казань", 500);
    tanDemo.addConnection("Нижний Новгород", "Санкт-Петербург", 700);
    tanDemo.addConnection("Санкт-Петербург", "Москва", 650);

    tanDemo.printNetwork();
    auto tanResult = tanDemo.findMinimalNetwork();

    cout << "\nРЕШЕНИЕ ЗАДАЧИ TAN NETWORK\n";
    if (tanResult.isConnected) {
        cout << "? Оптимальная сеть найдена!\n";
        cout << "Минимальная стоимость прокладки сети: " << tanResult.totalCost << " условных единиц\n";
        cout << "\nРекомендуемые соединения:\n";
        for (const auto& conn : tanResult.selectedConnections) {
            cout << "  • " << get<0>(conn) << " <--> " << get<1>(conn)
                << " : " << get<2>(conn) << " у.е.\n";
        }
    }
    else {
        cout << "? Невозможно соединить все города в единую сеть!\n";
    }

    
     runAllTests();


    return 0;
}