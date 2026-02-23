#include <list>
#include <vector>
#include <string>
#include "CppUnitTest.h"
#include "my_list.h"

namespace ListTests { class SimpleElem; }
namespace ListTests { class NoCopyElem; }

template<typename T> using ContainerTemplate = MyList<T>;
//template<typename T> using ContainerTemplate = std::list<T>;

// Используем SimpleElem для тестов
using Elem = ListTests::SimpleElem;
//using Elem = ListTests::NoCopyElem;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ListTests
{
    class SimpleElem {
        uint32_t watch;
        int data;
        static size_t elem_counter;
        void check() const { Assert::IsTrue(watch == 0xDEADBEEF, L"Повреждение памяти!! (Обращение к неинициализированному экземпляру класса SimpleElem)"); }
    public:
        SimpleElem(const SimpleElem& e) : data(e.data), watch(e.watch) { e.check(); ++elem_counter; }
        SimpleElem& operator=(const SimpleElem& e) { e.check(); data = e.data; watch = e.watch; return *this; }
        explicit SimpleElem(int value) : data(value), watch(0xDEADBEEF) { ++elem_counter; }
        SimpleElem(SimpleElem&& e) noexcept { e.check(); data = e.data; watch = e.watch; ++elem_counter; }
        SimpleElem& operator=(SimpleElem&& e) noexcept { check(); e.check(); data = e.data; watch = e.watch; return *this; }
        bool operator==(const SimpleElem& e) const { check(); e.check(); return data == e.data; }
        ~SimpleElem() { check(); watch = 0; --elem_counter; }
        static size_t count() { return elem_counter; }
    };
    size_t SimpleElem::elem_counter = 0;

    class NoCopyElem {
        uint32_t watch;
        int data;
        static size_t elem_counter;
        void check() const { Assert::IsTrue(watch == 0xDEADDEAD, L"Повреждение памяти!! (Обращение к неинициализированному экземпляру класса NoCopyElem)"); }
    public:
        NoCopyElem(const NoCopyElem& e) = delete;
        NoCopyElem& operator=(const NoCopyElem& e) = delete;
        explicit NoCopyElem(int value) : data(value), watch(0xDEADDEAD) { ++elem_counter; }
        NoCopyElem(NoCopyElem&& e) noexcept { e.check(); data = e.data; watch = e.watch; ++elem_counter; }
        NoCopyElem& operator=(NoCopyElem&& e) noexcept { check(); e.check(); data = e.data; watch = e.watch; return *this; }
        bool operator==(const NoCopyElem& e) const { check(); e.check(); return data == e.data; }
        ~NoCopyElem() { check(); watch = 0; --elem_counter; }
        static size_t count() { return elem_counter; }
    };
    size_t NoCopyElem::elem_counter = 0;


    TEST_CLASS(ComplexTypeTests)
    {
    public:

        TEST_METHOD(IteratorTests)
        {
            ContainerTemplate<int> list{ 3, 4, 8, 5, 2, 13, 20 };
            list.push_back(10);
            Assert::IsTrue(list.end() == ++--list.end(), L"++--list.end() должен возвращать list.end()");
            Assert::AreEqual(10, *(--list.end()), L"--list.end() должен возвращать последний элемент");
        }

        TEST_METHOD(StringTests)
        {
            ContainerTemplate<std::string> list{ "123", "AAAAAAAA", "abc", "cde" };
            std::vector<std::string> expected1{ "123", "AAAAAAAA", "abc", "cde" };

            Assert::AreEqual(size_t(4), list.size(), L"Неправильно считается количество строковых элементов");
            Assert::IsTrue(std::equal(list.begin(), list.end(), expected1.begin(), expected1.end()), L"Неправильный порядок строковых элементов");
            Assert::IsTrue(std::equal(list.rbegin(), list.rend(), expected1.rbegin(), expected1.rend()), L"Неправильный порядок строковых элементов");

            for (const auto& str : expected1)
                list.erase(std::find(list.begin(), list.end(), str));

            Assert::AreEqual(size_t(0), list.size(), L"Список не пуст после удаления всех элементов");
            for (const auto& elem : list)
                Assert::Fail(L"Что-то лежит в списке после удаления всех элементов");

            list.push_back("test-1");
            list.push_back("test-2");
            list.push_back("test-4");
            std::vector<std::string> expected2{ "test-1", "test-2", "test-4" };
            Assert::IsTrue(std::equal(list.begin(), list.end(), expected2.begin(), expected2.end()), L"Неправильный порядок строковых элементов");
            Assert::IsTrue(std::equal(list.rbegin(), list.rend(), expected2.rbegin(), expected2.rend()), L"Неправильный порядок строковых элементов");
        }

        TEST_METHOD(ElemTests)
        {
            size_t init_count = Elem::count();
            {
                ContainerTemplate<Elem> list;
                Assert::AreEqual(size_t(0), Elem::count() - init_count, L"Создан лишний экземпляр класса Elem");

                for (const auto& elem : list)
                    Assert::Fail(L"Что-то лежит в пустом списке");

                list.push_back(Elem(40));
                list.push_back(Elem(75));
                list.push_front(Elem(50));
                list.push_back(Elem(23));
                list.push_back(Elem(87));
                Assert::IsTrue(Elem::count() - init_count == 5, L"Создан лишний экземпляр класса Elem");

                auto find_result = std::find(list.begin(), list.end(), Elem(75));
                Assert::IsTrue(find_result != list.end(), L"Не найден элемент со значением 75");
                Assert::IsTrue(*find_result == Elem(75), L"Неправильно найден элемент со значением 75");

                list.erase(find_result);
                Assert::IsTrue(Elem::count() - init_count == 4, L"Неправильно работает удаление элементов списка");
            }
            Assert::IsTrue(Elem::count() - init_count == 0, L"Утечка памяти!!");
        }

        TEST_METHOD(CopyElemTests)
        {
            size_t init_count = Elem::count();
            {
                ContainerTemplate<Elem> list;
                {
                    ContainerTemplate<Elem> list1;
                    ContainerTemplate<Elem> list2(std::move(list1));
                    for (const auto& elem : list2)
                        Assert::Fail(L"Что-то лежит в пустом списке после копирования");

                    list2.push_back(Elem(40));
                    list2.push_front(Elem(50));
                    list2.push_back(Elem(23));
                    list2.push_back(Elem(87));
                    Assert::IsTrue(Elem::count() - init_count == 4, L"Неправильно работает удаление элементов списка");
                    list = std::move(list2);
                }
                Assert::IsTrue(list.back() == Elem(87), L"Некправильно работает копирование списка");
                Assert::IsTrue(Elem::count() - init_count == 4, L"Неправильно работает удаление элементов списка");
            }
            Assert::IsTrue(Elem::count() - init_count == 0, L"Утечка памяти!!");
        }

        TEST_METHOD(CountPositiveTests)
        {
            ContainerTemplate<int> list{ -3, 5, -1, 8, 0, -7, 2, 4 };
            Assert::AreEqual(size_t(4), list.count_positive(), L"Неправильно подсчитаны положительные элементы");

            ContainerTemplate<int> empty_list;
            Assert::AreEqual(size_t(0), empty_list.count_positive(), L"В пустом списке должно быть 0 положительных");

            ContainerTemplate<int> all_negative{ -1, -2, -3 };
            Assert::AreEqual(size_t(0), all_negative.count_positive(), L"В списке с отрицательными должно быть 0 положительных");
        }

        TEST_METHOD(FindMaxAndCountTests)
        {
            ContainerTemplate<int> list{ 3, 7, 2, 7, 5, 7, 1 };
            auto [max_val, count] = list.find_max_and_count();
            Assert::AreEqual(7, max_val, L"Неправильно найден максимальный элемент");
            Assert::AreEqual(size_t(3), count, L"Неправильно подсчитано количество вхождений максимума");

            ContainerTemplate<int> list2{ 5 };
            auto [max_val2, count2] = list2.find_max_and_count();
            Assert::AreEqual(5, max_val2, L"Неправильно найден максимум в списке из одного элемента");
            Assert::AreEqual(size_t(1), count2, L"Неправильно подсчитано количество в списке из одного элемента");

            auto test_empty = [&]() {
                ContainerTemplate<int> empty_list;
                auto result = empty_list.find_max_and_count();
                };
            Assert::ExpectException<std::out_of_range>(test_empty, L"Должно быть исключение для пустого списка");
        }

        TEST_METHOD(SplitTests)
        {
            ContainerTemplate<int> list{ -5, 3, -2, 7, 0, -1, 4, -8 };
            ContainerTemplate<int> negative, non_negative;

            list.split(negative, non_negative);

            Assert::AreEqual(size_t(0), list.size(), L"Исходный список должен быть пуст после split");
            Assert::AreEqual(size_t(4), negative.size(), L"Неправильное количество отрицательных");
            Assert::AreEqual(size_t(4), non_negative.size(), L"Неправильное количество неотрицательных");

            for (const auto& val : negative) {
                Assert::IsTrue(val < 0, L"В negative попал неотрицательный элемент");
            }

            for (const auto& val : non_negative) {
                Assert::IsTrue(val >= 0, L"В non_negative попал отрицательный элемент");
            }
        }

        TEST_METHOD(ReverseTests)
        {
            ContainerTemplate<int> list{ 1, 2, 3, 4, 5 };
            list.reverse();

            std::vector<int> expected{ 5, 4, 3, 2, 1 };
            Assert::IsTrue(std::equal(list.begin(), list.end(), expected.begin()),
                L"Неправильно развернут список");

            ContainerTemplate<int> single{ 42 };
            single.reverse();
            Assert::AreEqual(42, single.front(), L"reverse с одним элементом должен работать");

            ContainerTemplate<int> empty;
            empty.reverse();
        }
    };
}