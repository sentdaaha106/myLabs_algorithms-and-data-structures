#pragma once  // защита от повторного включения
#include <initializer_list>  
#include <memory>          
#include <iterator>        
#include <algorithm>       
#include <stdexcept>       
#include <utility>          

template<typename T>
class MyList
{
private:
   
    struct Node {
        T data;     
        Node* prev; 
        Node* next; 

        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}  
        Node(T&& value) : data(std::move(value)), prev(nullptr), next(nullptr) {}  // конструктор с перемещением
    };

    Node* head;   // указатель на первый элемент
    Node* tail;   // указатель на последний элемент
    size_t count; // количество элементов

    // инициализация пустого списка
    void init() {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

public:
    // стандартные типы для совместимости со stl
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    // итератор 
    class iterator
    {
    private:
        Node* ptr;          // текущий узел
        Node* list_tail;    // хвост списка 
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(Node* p = nullptr, Node* t = nullptr) : ptr(p), list_tail(t) {}

        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        reference operator*() { return ptr->data; }
        pointer operator->() { return &(ptr->data); }

        // префиксный инкремент
        iterator& operator++() {
            if (ptr) ptr = ptr->next;
            return *this;
        }

        // постфиксный инкремент
        iterator operator++(int) {
            iterator tmp = *this;
            if (ptr) ptr = ptr->next;
            return tmp;
        }

        // префиксный декремент
        iterator& operator--() {
            if (ptr && ptr->prev) {
                ptr = ptr->prev;
            }
            else if (!ptr && list_tail) {
                // если это end(), то -- должен вернуть последний элемент
                ptr = list_tail;
            }
            return *this;
        }

        // постфиксный декремент
        iterator operator--(int) {
            iterator tmp = *this;
            if (ptr && ptr->prev) {
                ptr = ptr->prev;
            }
            else if (!ptr && list_tail) {
                ptr = list_tail;
            }
            return tmp;
        }

        Node* node() const { return ptr; }
    };

    //  константный итератор
    class const_iterator
    {
    private:
        const Node* ptr;
        const Node* list_tail;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const Node* p = nullptr, const Node* t = nullptr) : ptr(p), list_tail(t) {}
        const_iterator(const iterator& it) : ptr(it.node()), list_tail(nullptr) {}

        bool operator==(const const_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const const_iterator& other) const { return ptr != other.ptr; }
        const_reference operator*() const { return ptr->data; }

        const_iterator& operator++() {
            if (ptr) ptr = ptr->next;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            if (ptr) ptr = ptr->next;
            return tmp;
        }

        const_iterator& operator--() {
            if (ptr && ptr->prev) {
                ptr = ptr->prev;
            }
            else if (!ptr && list_tail) {
                ptr = list_tail;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            if (ptr && ptr->prev) {
                ptr = ptr->prev;
            }
            else if (!ptr && list_tail) {
                ptr = list_tail;
            }
            return tmp;
        }
    };

    // типы для обратных итераторов
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //конструкторы
    MyList() { init(); }  // конструктор по умолчанию

    // конструктор копирования
    MyList(const MyList& other) : MyList() {
        for (const auto& elem : other) {
            push_back(elem);
        }
    }

    // конструктор от initializer_list {1, 2, 3}
    MyList(std::initializer_list<T> init) : MyList() {
        for (const auto& elem : init) {
            push_back(elem);
        }
    }

    // перемещающий конструктор
    MyList(MyList&& other) noexcept
        : head(other.head), tail(other.tail), count(other.count) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }

    //деструктор 
    ~MyList() { clear(); }

    //  операторы присваивания 
    MyList& operator=(const MyList& other) {
        if (this != &other) {
            clear();
            for (const auto& elem : other) {
                push_back(elem);
            }
        }
        return *this;
    }

    MyList& operator=(MyList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head;
            tail = other.tail;
            count = other.count;
            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    MyList& operator=(std::initializer_list<T> ilist) {
        clear();
        for (const auto& elem : ilist) {
            push_back(elem);
        }
        return *this;
    }

    // доступ к элементам
    reference front() { return head->data; }
    const_reference front() const { return head->data; }
    reference back() { return tail->data; }
    const_reference back() const { return tail->data; }

    //  итераторы 
    iterator begin() noexcept { return iterator(head, tail); }
    iterator end() noexcept { return iterator(nullptr, tail); }
    const_iterator begin() const noexcept { return const_iterator(head, tail); }
    const_iterator end() const noexcept { return const_iterator(nullptr, tail); }
    const_iterator cbegin() const noexcept { return const_iterator(head, tail); }
    const_iterator cend() const noexcept { return const_iterator(nullptr, tail); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    // вместимость
    bool empty() const noexcept { return count == 0; }
    size_type size() const noexcept { return count; }

    //  модификаторы 
    void clear() noexcept {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }

    void push_back(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        ++count;
    }

    void push_front(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        }
        else {
            head->prev = newNode;
            newNode->next = head;
            head = newNode;
        }
        ++count;
    }

    void pop_back() {
        if (empty()) return;
        Node* temp = tail;
        tail = tail->prev;
        if (tail) {
            tail->next = nullptr;
        }
        else {
            head = nullptr;
        }
        delete temp;
        --count;
    }

    void pop_front() {
        if (empty()) return;
        Node* temp = head;
        head = head->next;
        if (head) {
            head->prev = nullptr;
        }
        else {
            tail = nullptr;
        }
        delete temp;
        --count;
    }

    iterator insert(iterator pos, const T& value) {
        Node* newNode = new Node(value);
        Node* current = pos.node();

        if (!current) { // вставка в конец
            if (!head) {
                head = tail = newNode;
            }
            else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }
        else if (current == head) { // вставка в начало
            newNode->next = head;
            if (head) head->prev = newNode;
            head = newNode;
        }
        else { // вставка в середину
            newNode->prev = current->prev;
            newNode->next = current;
            if (current->prev) current->prev->next = newNode;
            current->prev = newNode;
        }
        ++count;
        return iterator(newNode, tail);
    }

    iterator erase(iterator pos) {
        Node* current = pos.node();
        if (!current) return end();

        Node* nextNode = current->next;

        if (current->prev) {
            current->prev->next = current->next;
        }
        else {
            head = current->next;
        }

        if (current->next) {
            current->next->prev = current->prev;
        }
        else {
            tail = current->prev;
        }

        delete current;
        --count;
        return iterator(nextNode, tail);
    }

    iterator erase(iterator first, iterator last) {
        while (first != last) {
            first = erase(first);
        }
        return last;
    }

    // задание 2.4 развернуть список 
    void reverse() noexcept {
        if (count < 2) return;

        Node* current = head;
        Node* temp = nullptr;

        while (current) {
            temp = current->prev;
            current->prev = current->next;
            current->next = temp;
            current = current->prev;
        }

        temp = head;
        head = tail;
        tail = temp;
    }

    //  задание 2.1 подсчет положительных элементов
    size_type count_positive() const {
        size_type result = 0;
        const Node* current = head;
        while (current) {
            if (current->data > 0) ++result;
            current = current->next;
        }
        return result;
    }

    // задание 2.2: найти максимум и сколько раз встречается
    std::pair<T, size_type> find_max_and_count() const {
        if (empty()) throw std::out_of_range("List is empty");

        const Node* current = head;
        T max_elem = current->data;
        size_type count_max = 1;
        current = current->next;

        while (current) {
            if (current->data > max_elem) {
                max_elem = current->data;
                count_max = 1;
            }
            else if (current->data == max_elem) {
                ++count_max;
            }
            current = current->next;
        }
        return { max_elem, count_max };
    }

    //  задание 2.3: разделить на два списка (без выделения памяти) 
    void split(MyList<T>& negative, MyList<T>& non_negative) {
        negative.clear();
        non_negative.clear();

        if (empty()) return;

        Node* current = head;
        while (current) {
            Node* next = current->next;

            // отсоединяем текущий узел от исходного списка
            if (current->prev) {
                current->prev->next = current->next;
            }
            else {
                head = current->next;
            }
            if (current->next) {
                current->next->prev = current->prev;
            }
            else {
                tail = current->prev;
            }

            // обнуляем связи узла
            current->prev = nullptr;
            current->next = nullptr;

            // определяем, в какой список добавлять
            if (current->data < 0) {
                // добавляем в список отрицательных
                if (!negative.head) {
                    negative.head = negative.tail = current;
                }
                else {
                    negative.tail->next = current;
                    current->prev = negative.tail;
                    negative.tail = current;
                }
                ++negative.count;
            }
            else {
                // добавляем в список неотрицательных
                if (!non_negative.head) {
                    non_negative.head = non_negative.tail = current;
                }
                else {
                    non_negative.tail->next = current;
                    current->prev = non_negative.tail;
                    non_negative.tail = current;
                }
                ++non_negative.count;
            }

            current = next;
        }

        // исходный список становится пустым
        head = tail = nullptr;
        count = 0;
    }

    //  универсальные методы с perfect forwarding
    template<typename U>
    iterator insert(iterator pos, U&& value) {
        Node* newNode = new Node(std::forward<U>(value));
        Node* current = pos.node();

        if (!current) { // вставка в конец
            if (!head) {
                head = tail = newNode;
            }
            else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }
        else if (current == head) { // вставка в начало
            newNode->next = head;
            if (head) head->prev = newNode;
            head = newNode;
        }
        else { // вставка в середину
            newNode->prev = current->prev;
            newNode->next = current;
            if (current->prev) current->prev->next = newNode;
            current->prev = newNode;
        }
        ++count;
        return iterator(newNode, tail);
    }

    template<typename U>
    void push_back(U&& value) {
        insert(end(), std::forward<U>(value));
    }

    template<typename U>
    void push_front(U&& value) {
        insert(begin(), std::forward<U>(value));
    }
};