#include <iostream>
#include <chrono>  
#include <list>   
#include "my_list.h"

template<typename T>
void printList(const MyList<T>& list, const std::string& name) {
    std::cout << name << ": ";
    for (const auto& val : list) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    // 1 Создание и добавление элементов
    std::cout << "1 базовые операции:\n";
    MyList<int> my_list;
    my_list.push_back(10);
    my_list.push_back(20);
    my_list.push_front(5);
    my_list.push_back(30);

    std::cout << "   MyList: ";
    for (const auto& val : my_list) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    std::cout << "   первый элемент " << my_list.front() << "\n";
    std::cout << "   последний элемент " << my_list.back() << "\n";
    std::cout << "   размер " << my_list.size() << "\n\n";

    // 2 Сравнение со стандартным списком
    std::cout << "2сравнение со std::list:\n";
    std::list<int> std_list;
    std_list.push_back(10);
    std_list.push_back(20);
    std_list.push_front(5);
    std_list.push_back(30);

    std::cout << "   std::list: ";
    for (const auto& val : std_list) {
        std::cout << val << " ";
    }
    std::cout << "\n\n";

    // 3 Задание 2.1  Подсчет положительных
    std::cout << "3 подсчет положительных элементов:\n";
    MyList<int> list_pos{ -5, 3, -2, 7, 0, -1, 4, -8 };
    printList(list_pos, "   исходный список");
    std::cout << "   положительных элементов " << list_pos.count_positive() << "\n\n";

    // 4 Задание 2.2  Поиск максимума
    std::cout << "4 поиск максимального элемента:\n";
    MyList<int> list_max{ 3, 7, 2, 7, 5, 7, 1 };
    printList(list_max, "   исходный список");
    auto [max_val, count] = list_max.find_max_and_count();
    std::cout << "   максимальный элемент: " << max_val << "\n";
    std::cout << "   количество вхождений: " << count << "\n\n";

    // 5  Задание 2.3 Разделение списка
    std::cout << "5 рпазделение списка на отрицательные и неотрицательные:\n";
    MyList<int> list_split{ -5, 3, -2, 7, 0, -1, 4, -8 };
    MyList<int> negative, non_negative;

    printList(list_split, "   исходный список");
    list_split.split(negative, non_negative);

    printList(negative, "   отрицательные");
    printList(non_negative, "   неотрицательные");
    std::cout << "\n";

    // 6 Задание 2.4 Разворот списка
    std::cout << "6 разворот списка:\n";
    MyList<int> list_rev{ 1, 2, 3, 4, 5 };
    printList(list_rev, "   исходный список");
    list_rev.reverse();
    printList(list_rev, "   после reverse");
    std::cout << "\n";

    // 7 Сравнение производительности 
    std::cout << "7 сравнение производительности (100000 элементов):\n";

    auto start = std::chrono::high_resolution_clock::now();
    MyList<int> big_my_list;
    for (int i = 0; i < 100000; ++i) {
        big_my_list.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto my_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    std::list<int> big_std_list;
    for (int i = 0; i < 100000; ++i) {
        big_std_list.push_back(i);
    }
    end = std::chrono::high_resolution_clock::now();
    auto std_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "   MyList: " << my_time << " ms\n";
    std::cout << "   std::list: " << std_time << " ms\n";

    return 0;
}