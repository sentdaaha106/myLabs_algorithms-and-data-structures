#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <set>

// Узел AVL-дерева
struct Node {
    int data;
    Node* left;
    Node* right;
    int height;

    Node(int val) : data(val), left(nullptr), right(nullptr), height(1) {}
};

// AVL-дерево (множество без повторов)
class AVLTree {
private:
    Node* root;
    int rotations;  // счетчик вращений
    int count;      // количество элементов

    // Высота узла
    int h(Node* node) {
        return node ? node->height : 0;
    }

    // Обновление высоты
    void upd(Node* node) {
        if (node) {
            node->height = 1 + std::max(h(node->left), h(node->right));
        }
    }

    // Баланс-фактор
    int bf(Node* node) {
        return node ? h(node->left) - h(node->right) : 0;
    }

    // Правый поворот
    Node* rotateRight(Node* y) {
        rotations++;
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        upd(y);
        upd(x);
        return x;
    }

    // Левый поворот
    Node* rotateLeft(Node* x) {
        rotations++;
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        upd(x);
        upd(y);
        return y;
    }

    // Балансировка
    Node* balance(Node* node) {
        if (!node) return nullptr;

        upd(node);
        int factor = bf(node);

        // Левое тяжелее
        if (factor > 1) {
            if (bf(node->left) < 0) {
                node->left = rotateLeft(node->left);  // LR
            }
            return rotateRight(node);  // LL
        }
        // Правое тяжелее
        if (factor < -1) {
            if (bf(node->right) > 0) {
                node->right = rotateRight(node->right);  // RL
            }
            return rotateLeft(node);  // RR
        }

        return node;
    }

    // Вставка
    Node* insert(Node* node, int val) {
        if (!node) {
            count++;
            return new Node(val);
        }

        if (val < node->data) {
            node->left = insert(node->left, val);
        }
        else if (val > node->data) {
            node->right = insert(node->right, val);
        }
        else {
            return node;  // уже есть
        }

        return balance(node);
    }

    // Минимальный узел
    Node* minNode(Node* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    // Удаление
    Node* remove(Node* node, int val) {
        if (!node) return nullptr;

        if (val < node->data) {
            node->left = remove(node->left, val);
        }
        else if (val > node->data) {
            node->right = remove(node->right, val);
        }
        else {
            count--;
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }
            else {
                Node* temp = minNode(node->right);
                node->data = temp->data;
                node->right = remove(node->right, temp->data);
            }
        }

        return balance(node);
    }

    // Поиск
    bool find(Node* node, int val) {
        if (!node) return false;
        if (val < node->data) return find(node->left, val);
        if (val > node->data) return find(node->right, val);
        return true;
    }

    // lower_bound
    int lower(Node* node, int val) {
        int res = -1;
        while (node) {
            if (node->data >= val) {
                res = node->data;
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return res;
    }

    // upper_bound
    int upper(Node* node, int val) {
        int res = -1;
        while (node) {
            if (node->data > val) {
                res = node->data;
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return res;
    }

    // Вывод боком
    void print(Node* node, int level) {
        if (!node) return;
        print(node->right, level + 1);
        for (int i = 0; i < level; i++) std::cout << "    ";
        std::cout << node->data << std::endl;
        print(node->left, level + 1);
    }

    // Сохранение
    void save(Node* node, std::ofstream& f) {
        if (!node) {
            f << "#\n";
            return;
        }
        f << node->data << "\n";
        save(node->left, f);
        save(node->right, f);
    }

    // Загрузка
    Node* load(std::ifstream& f) {
        std::string line;
        if (!getline(f, line)) return nullptr;
        if (line == "#") return nullptr;

        Node* node = new Node(stoi(line));
        node->left = load(f);
        node->right = load(f);
        upd(node);
        return node;
    }

    // Очистка
    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr), rotations(0), count(0) {}

    ~AVLTree() { clear(root); }

    // Вставка
    void insert(int val) {
        rotations = 0;
        root = insert(root, val);
    }

    int getRotations() { return rotations; }

    // Удаление
    void erase(int val) {
        rotations = 0;
        root = remove(root, val);
    }

    // Поиск
    bool find(int val) {
        return find(root, val);
    }

    // Границы
    int lower_bound(int val) { return lower(root, val); }
    int upper_bound(int val) { return upper(root, val); }

    // Вывод
    void print() {
        print(root, 0);
    }

    // Сохранение
    void saveFile(const std::string& name) {
        std::ofstream f(name);
        save(root, f);
    }

    // Загрузка
    void loadFile(const std::string& name) {
        clear(root);
        std::ifstream f(name);
        root = load(f);
        count = 0;
        // пересчет размера (можно опустить для простоты)
    }

    int size() { return count; }
    bool empty() { return count == 0; }
};

int main() {
    setlocale(LC_ALL, "Russian");

    AVLTree tree;

    // Вставка
    std::cout << "Вставка элементов:\n";
    int arr[] = { 50, 30, 70, 20, 40, 60, 80, 25, 35 };
    for (int x : arr) {
        tree.insert(x);
        std::cout << "  + " << x << " (вращений: " << tree.getRotations() << ")\n";
    }

    // Вывод
    std::cout << "\nДерево (боком):\n";
    tree.print();

    // Поиск
    std::cout << "\nПоиск:\n";
    std::cout << "  40: " << (tree.find(40) ? "есть" : "нет") << "\n";
    std::cout << "  100: " << (tree.find(100) ? "есть" : "нет") << "\n";

    // Границы
    std::cout << "\nlower_bound(35): " << tree.lower_bound(35) << "\n";
    std::cout << "upper_bound(35): " << tree.upper_bound(35) << "\n";

    // Удаление
    std::cout << "\nУдаление 20 и 50:\n";
    tree.erase(20);
    std::cout << "  удаление 20 (вращений: " << tree.getRotations() << ")\n";
    tree.erase(50);
    std::cout << "  удаление 50 (вращений: " << tree.getRotations() << ")\n";

    std::cout << "\nДерево после удаления:\n";
    tree.print();

    // Сохранение
    tree.saveFile("tree.txt");
    std::cout << "\nСохранено в tree.txt\n";

    // Загрузка
    AVLTree tree2;
    tree2.loadFile("tree.txt");
    std::cout << "Загружено из файла:\n";
    tree2.print();

    // Тест производительности
    std::cout << "\nТест производительности (10000 элементов)\n";
    const int N = 10000;

    auto start = std::chrono::high_resolution_clock::now();
    AVLTree myTree;
    for (int i = 0; i < N; i++) myTree.insert(rand() % N);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "AVLTree: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " мс\n";

    start = std::chrono::high_resolution_clock::now();
    std::set<int> stdSet;
    for (int i = 0; i < N; i++) stdSet.insert(rand() % N);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "std::set: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " мс\n";

    return 0;
}