#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;


template<typename Key, typename Compare = less<Key>>
class BinarySearchTree {
private:
    // Структура узла с указателем на родителя
    struct Node {
        Key data;
        Node* left;
        Node* right;
        Node* parent;

        Node(const Key& value, Node* p = nullptr)
            : data(value), left(nullptr), right(nullptr), parent(p) {
        }
    };

    Node* root;
    Compare comp;
    size_t tree_size;

    // Вспомогательная рекурсивная функция для вставки
    Node* insert(Node* node, const Key& value, Node* parent = nullptr) {
        if (node == nullptr) {
            tree_size++;
            return new Node(value, parent);
        }

        if (comp(value, node->data)) {
            node->left = insert(node->left, value, node);
        }
        else if (comp(node->data, value)) {
            node->right = insert(node->right, value, node);
        }
        // Если значение уже существует, ничего не делаем (как в set)

        return node;
    }

    // Поиск минимального элемента в поддереве
    Node* findMin(Node* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }

    // Поиск максимального элемента в поддереве
    Node* findMax(Node* node) {
        while (node && node->right)
            node = node->right;
        return node;
    }

    // Вспомогательная функция для удаления узла
    Node* remove(Node* node, const Key& value) {
        if (node == nullptr) return nullptr;

        if (comp(value, node->data)) {
            node->left = remove(node->left, value);
        }
        else if (comp(node->data, value)) {
            node->right = remove(node->right, value);
        }
        else {
            // Нашли удаляемый узел

            // Случай 1 нет детей или только один ребенок
            if (node->left == nullptr) {
                Node* rightChild = node->right;
                if (rightChild) rightChild->parent = node->parent;
                delete node;
                tree_size--;
                return rightChild;
            }
            else if (node->right == nullptr) {
                Node* leftChild = node->left;
                if (leftChild) leftChild->parent = node->parent;
                delete node;
                tree_size--;
                return leftChild;
            }

            // Случай 2 два ребенка
            // Находим минимальный элемент в правом поддереве
            Node* successor = findMin(node->right);
            node->data = successor->data;
            node->right = remove(node->right, successor->data);
        }
        return node;
    }

    // Рекурсивное сравнение двух деревьев
    bool isEqual(Node* node1, Node* node2) {
        if (node1 == nullptr && node2 == nullptr) return true;
        if (node1 == nullptr || node2 == nullptr) return false;

        return (node1->data == node2->data) &&
            isEqual(node1->left, node2->left) &&
            isEqual(node1->right, node2->right);
    }

public:
    // Конструктор
    BinarySearchTree() : root(nullptr), tree_size(0) {}

    // Деструктор (освобождаю память)
    ~BinarySearchTree() {
        clear(root);
    }

    // Очистка дерева
    void clear(Node* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    // 1 Добавление элемента
    void insert(const Key& value) {
        root = insert(root, value, nullptr);
    }

    // 2 Поиск элемента (возвращает указатель на узел или nullptr)
    Node* find(const Key& value) {
        Node* current = root;
        while (current) {
            if (comp(value, current->data)) {
                current = current->left;
            }
            else if (comp(current->data, value)) {
                current = current->right;
            }
            else {
                return current; // Нашли
            }
        }
        return nullptr; // Не нашли
    }

    // 3 Поиск минимального элемента
    Node* findMin() {
        return findMin(root);
    }

    // 3 Поиск максимального элемента
    Node* findMax() {
        return findMax(root);
    }

    // 4 Удаление элемента
    void remove(const Key& value) {
        root = remove(root, value);
    }

    // 5 lower_bound (первый элемент не меньше заданного)
    Node* lower_bound(const Key& value) {
        Node* current = root;
        Node* result = nullptr;

        while (current) {
            if (!comp(current->data, value)) { // current->data >= value
                result = current;
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        return result;
    }

    // 5 upper_bound (первый элемент больше заданного)
    Node* upper_bound(const Key& value) {
        Node* current = root;
        Node* result = nullptr;

        while (current) {
            if (comp(value, current->data)) { // value < current->data
                result = current;
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        return result;
    }

    // 6.1 Вывод: Левый/Текущий/Правый (in-order обход, рекурсивно)
    void printInOrder(Node* node) {
        if (node) {
            printInOrder(node->left);
            cout << node->data << " ";
            printInOrder(node->right);
        }
    }

    void printInOrder() {
        printInOrder(root);
        cout << endl;
    }

    // 6.2 Вывод: Правый/Текущий/Левый (обратный in-order, с использованием стека)
    void printReverseWithStack() {
        if (!root) return;

        stack<Node*> s;
        Node* current = root;

        while (current || !s.empty()) {
            // Сначала идем вправо
            while (current) {
                s.push(current);
                current = current->right;
            }

            current = s.top();
            s.pop();
            cout << current->data << " ";

            // Затем идем влево
            current = current->left;
        }
        cout << endl;
    }

    // 6.3 Вывод по слоям (breadth-first, с использованием очереди)
    void printLevelOrder() {
        if (!root) return;

        queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* current = q.front();
            q.pop();

            cout << current->data << " ";

            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
        cout << endl;
    }

    // 7 Сравнение двух деревьев на равенство
    bool operator==(const BinarySearchTree& other) {
        return isEqual(root, other.root);
    }

    // 8 Поиск следующего элемента (аналог ++iterator)
    Node* next(Node* node) {
        if (!node) return nullptr;

        // Если есть правое поддерево, следующий - минимальный в правом поддереве
        if (node->right) {
            return findMin(node->right);
        }

        // Иначе поднимаемся вверх, пока не найдем узел, который является левым ребенком
        Node* parent = node->parent;
        while (parent && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }

    // 8 Поиск предыдущего элемента (аналог --iterator)
    Node* prev(Node* node) {
        if (!node) return nullptr;

        // Если есть левое поддерево, предыдущий - максимальный в левом поддереве
        if (node->left) {
            return findMax(node->left);
        }

        // Иначе поднимаемся вверх, пока не найдем узел, который является правым ребенком
        Node* parent = node->parent;
        while (parent && node == parent->left) {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }

    // Получение размера дерева
    size_t size() const {
        return tree_size;
    }

    // Проверка на пустоту
    bool empty() const {
        return root == nullptr;
    }
};

// РЕШЕТО ЭРАТОСФЕНА 

// Способ 1 на основе бинарного дерева поиска
vector<int> sieveWithTree(int n) {
    BinarySearchTree<int> tree;

    // Заполняем дерево числами от 2 до n
    for (int i = 2; i <= n; i++) {
        tree.insert(i);
    }

    vector<int> primes;

    for (int p = 2; p * p <= n; ) {
        // Удаляем кратные числа
        for (int multiple = p * p; multiple <= n; multiple += p) {
            tree.remove(multiple);
        }

        // Находим следующее простое число
        auto nextNode = tree.upper_bound(p);
        if (nextNode) {
            p = nextNode->data;
        }
        else {
            break;
        }
    }

    // Собираем все оставшиеся числа (простые)
    auto node = tree.findMin();
    while (node) {
        primes.push_back(node->data);
        node = tree.next(node);
    }

    return primes;
}

// Способ 2 на основе массива 
vector<int> sieveWithArray(int n) {
    vector<bool> isPrime(n + 1, true);
    vector<int> primes;

    isPrime[0] = isPrime[1] = false;

    for (int p = 2; p * p <= n; p++) {
        if (isPrime[p]) {
            for (int multiple = p * p; multiple <= n; multiple += p) {
                isPrime[multiple] = false;
            }
        }
    }

    for (int i = 2; i <= n; i++) {
        if (isPrime[i]) {
            primes.push_back(i);
        }
    }

    return primes;
}

// Функция для сравнения времени выполнения
void comparePerformance(int n) {
    cout << "Сравнение алгоритмов Решето Эратосфена для N = " << n << ":\n";

    // Способ 1 с деревом
    auto start = high_resolution_clock::now();
    vector<int> primesTree = sieveWithTree(n);
    auto end = high_resolution_clock::now();
    auto durationTree = duration_cast<milliseconds>(end - start);

    // Способ 2 с массивом
    start = high_resolution_clock::now();
    vector<int> primesArray = sieveWithArray(n);
    end = high_resolution_clock::now();
    auto durationArray = duration_cast<milliseconds>(end - start);

    cout << "С использованием дерева: " << durationTree.count() << " мс\n";
    cout << "С использованием массива: " << durationArray.count() << " мс\n";

    // Проверка, что результаты совпадают
    if (primesTree.size() == primesArray.size()) {
        cout << "Количество найденных простых чисел: " << primesTree.size() << "\n";
    }
}

// Демонстрация работы всех операций
int main() {
    setlocale(LC_ALL, "Russian");
    // Создаем дерево
    BinarySearchTree<int> bst;

    // 1. Добавление элементов
    cout << "1 Добавление элементов: 50, 30, 70, 20, 40, 60, 80\n";
    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);

    // 2 Поиск элемента
    cout << "2 Поиск элемента 40: ";
    auto node = bst.find(40);
    if (node) cout << "найден\n"; else cout << "не найден\n";

    // 3 Минимальный и максимальный элементы
    cout << "3 Минимальный элемент: " << bst.findMin()->data << "\n";
    cout << "   Максимальный элемент: " << bst.findMax()->data << "\n";

    // 4 Вывод разными способами
    cout << "4 Вывод дерева:\n";
    cout << "   Левый/Текущий/Правый (рекурсивно): ";
    bst.printInOrder();

    cout << "   Правый/Текущий/Левый (со стеком): ";
    bst.printReverseWithStack();

    cout << "   По слоям (с очередью): ";
    bst.printLevelOrder();

    // 5 lower_bound и upper_bound
    cout << "5 lower_bound(35): " << bst.lower_bound(35)->data << "\n";
    cout << "   upper_bound(35): " << bst.upper_bound(35)->data << "\n";

    // 6 Следующий и предыдущий элементы
    auto node40 = bst.find(40);
    cout << "6 Следующий после 40: " << bst.next(node40)->data << "\n";
    cout << "   Предыдущий перед 40: " << bst.prev(node40)->data << "\n";

    // 7 Удаление элемента
    cout << "7 Удаление элемента 30\n";
    bst.remove(30);
    cout << "   После удаления: ";
    bst.printInOrder();

    // 8 Сравнение деревьев
    BinarySearchTree<int> bst2;
    bst2.insert(50);
    bst2.insert(30);
    bst2.insert(70);

    cout << "8 Равенство деревьев: " << (bst == bst2 ? "да" : "нет") << "\n";

    // Сравнение алгоритмов Решето Эратосфена
    cout << "\n Решето Эратосфена\n";
    comparePerformance(1000);  // Для N=1000
    comparePerformance(10000); // Для N=10000

    return 0;
}