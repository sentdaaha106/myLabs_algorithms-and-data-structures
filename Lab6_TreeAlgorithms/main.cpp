#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <set>
#include <map>

using namespace std;

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ 

// Очистка слова (удаляем всё кроме букв, приводим к нижнему регистру)
string cleanWord(const string& word) {
    string result;
    for (char c : word) {
        if (isalpha((unsigned char)c)) {
            result += tolower((unsigned char)c);
        }
    }
    return result;
}

// Проверка, является ли слово собственным именем
bool isProperName(const string& word) {
    if (word.empty()) return false;
    if (!isupper((unsigned char)word[0])) return false;
    for (size_t i = 1; i < word.size(); ++i) {
        if (!islower((unsigned char)word[i])) return false;
    }
    return true;
}

// Ключ для анаграммы: отсортированные буквы
string anagramKey(const string& word) {
    string sorted = word;
    sort(sorted.begin(), sorted.end());
    return sorted;
}

// Чтение слов из файла (разбиваем текст на слова)
vector<string> readWordsFromFile(const string& filename) {
    ifstream file(filename);
    vector<string> words;
    string line;

    if (!file.is_open()) {
        cerr << "ОШИБКА: Не удалось открыть файл " << filename << endl;
        return words;
    }

    // Читаем построчно и разбиваем на слова
    while (getline(file, line)) {
        string currentWord;
        for (char c : line) {
            if (isalpha((unsigned char)c)) {
                currentWord += c;
            }
            else if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        }
        if (!currentWord.empty()) {
            words.push_back(currentWord);
        }
    }
    file.close();
    return words;
}

// ЗАДАЧА 1 
void task1(const vector<string>& words) {
    unordered_map<string, int> freq;

    for (const auto& w : words) {
        string clean = cleanWord(w);
        if (!clean.empty()) {
            freq[clean]++;
        }
    }

    // Перенос в вектор для сортировки
    vector<pair<string, int>> freqVec(freq.begin(), freq.end());
    sort(freqVec.begin(), freqVec.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) { return a.second > b.second; });

    cout << "\n ЗАДАЧА 1\n";
    int count = min(40, (int)freqVec.size());
    for (int i = 0; i < count; ++i) {
        cout << i + 1 << ". " << freqVec[i].first << " : " << freqVec[i].second << " раз\n";
    }
}

//  ЗАДАЧА 2
void task2(const vector<string>& words) {
    unordered_map<string, int> properFreq;

    for (const auto& w : words) {
        if (isProperName(w)) {
            string lower = cleanWord(w);
            properFreq[lower]++;
        }
    }

    vector<pair<string, int>> freqVec(properFreq.begin(), properFreq.end());
    sort(freqVec.begin(), freqVec.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) { return a.second > b.second; });

    cout << "\n ЗАДАЧА 2\n";
    int count = min(20, (int)freqVec.size());
    for (int i = 0; i < count; ++i) {
        cout << i + 1 << ". " << freqVec[i].first << " : " << freqVec[i].second << " раз\n";
    }
}

// ЗАДАЧА 3 
void task3(const vector<string>& words) {
    unordered_map<string, vector<string>> anagramGroups;

    for (const auto& w : words) {
        string clean = cleanWord(w);
        if (clean.length() >= 3) {
            string key = anagramKey(clean);
            anagramGroups[key].push_back(clean);
        }
    }

    // Поиск самой большой группы
    string largestKey;
    size_t maxSize = 0;

    for (unordered_map<string, vector<string>>::iterator it = anagramGroups.begin(); it != anagramGroups.end(); ++it) {
        if (it->second.size() > maxSize) {
            maxSize = it->second.size();
            largestKey = it->first;
        }
    }

    cout << "\nЗАДАЧА 3\n";
    if (!largestKey.empty()) {
        vector<string>& largestGroup = anagramGroups[largestKey];
        cout << "Самая большая группа анаграмм содержит " << maxSize << " слов:\n";

        // Выводим первые 10 для наглядности
        for (size_t i = 0; i < min((size_t)10, largestGroup.size()); ++i) {
            cout << largestGroup[i] << " ";
        }
        if (largestGroup.size() > 10) cout << "...";
        cout << "\n";

        // Поиск двух самых длинных слов в этой группе
        sort(largestGroup.begin(), largestGroup.end(),
            [](const string& a, const string& b) { return a.length() > b.length(); });

        if (largestGroup.size() >= 2) {
            cout << "\nПара самых длинных анаграмм:\n";
            cout << "  - " << largestGroup[0] << " (" << largestGroup[0].length() << " букв)\n";
            cout << "  - " << largestGroup[1] << " (" << largestGroup[1].length() << " букв)\n";
        }
    }
}

//  СТРУКТУРА ДЛЯ ИГР 
struct Game {
    string name;
    string platform;
    int year = 0;              
    string genre;
    string publisher;
    string developer;
    double globalSales = 0.0;  
};

// Парсинг CSV с играми (vgsales.csv)
vector<Game> loadGamesFromCSV(const string& filename) {
    vector<Game> games;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "ОШИБКА: Не удалось открыть " << filename << endl;
        cerr << "Для задач 4-6 нужен файл vgsales.csv" << endl;
        return games;
    }

    // Пропускаем заголовок
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Game game;
        string token;

        // Парсим поля (формат vgsales.csv)
        getline(ss, token, ','); // Rank (пропускаем)
        getline(ss, game.name, ',');
        getline(ss, game.platform, ',');

        getline(ss, token, ',');
        if (!token.empty()) game.year = atoi(token.c_str());

        getline(ss, game.genre, ',');
        getline(ss, game.publisher, ',');

        // Пропускаем NA_Sales, EU_Sales, JP_Sales, Other_Sales
        for (int i = 0; i < 4; i++) getline(ss, token, ',');

        getline(ss, token, ',');
        if (!token.empty()) game.globalSales = atof(token.c_str());

        // Developer в этом датасете отсутствует
        game.developer = "Unknown";

        if (!game.publisher.empty() && game.publisher != "Unknown" && game.year > 0) {
            games.push_back(game);
        }
    }
    file.close();

    cout << "Загружено записей об играх: " << games.size() << endl;
    return games;
}

//  ЗАДАЧА 4 
void task4(const vector<Game>& games) {
    // Издатель  Жанр  Суммарный доход
    map<string, map<string, double>> publisherGenreRevenue;

    for (const auto& game : games) {
        publisherGenreRevenue[game.publisher][game.genre] += game.globalSales;
    }

    cout << "\n ЗАДАЧА 4\n";

    int shown = 0;
    for (map<string, map<string, double>>::iterator itPub = publisherGenreRevenue.begin(); itPub != publisherGenreRevenue.end(); ++itPub) {
        if (shown >= 10) {
            cout << "\n... и ещё " << publisherGenreRevenue.size() - 10 << " издателей\n";
            break;
        }

        cout << "\nИздатель: " << itPub->first << "\n";

        // Сортируем жанры по доходу
        vector<pair<string, double>> genreVec(itPub->second.begin(), itPub->second.end());
        sort(genreVec.begin(), genreVec.end(),
            [](const pair<string, double>& a, const pair<string, double>& b) { return a.second > b.second; });

        for (size_t i = 0; i < genreVec.size(); ++i) {
            cout << "   " << genreVec[i].first << ": " << genreVec[i].second << " млн $\n";
        }
        shown++;
    }
}

//  ЗАДАЧА 5 
void task5(const vector<Game>& games) {
    // Год -> Жанр -> Суммарный доход
    map<int, map<string, double>> yearGenreRevenue;

    for (const auto& game : games) {
        if (game.year >= 1980 && game.year <= 2020) {
            yearGenreRevenue[game.year][game.genre] += game.globalSales;
        }
    }

    cout << "\n ЗАДАЧА 5\n";

    // Получаем годы и сортируем
    vector<int> years;
    for (map<int, map<string, double>>::iterator it = yearGenreRevenue.begin(); it != yearGenreRevenue.end(); ++it) {
        years.push_back(it->first);
    }
    sort(years.begin(), years.end());

    int shown = 0;
    for (size_t i = 0; i < years.size(); ++i) {
        int year = years[i];
        if (shown >= 30) {
            cout << "\n... и ещё " << years.size() - 30 << " лет\n";
            break;
        }

        // Находим самый доходный жанр в этом году
        string topGenre;
        double maxRevenue = 0;

        map<string, double>& genres = yearGenreRevenue[year];
        for (map<string, double>::iterator itGen = genres.begin(); itGen != genres.end(); ++itGen) {
            if (itGen->second > maxRevenue) {
                maxRevenue = itGen->second;
                topGenre = itGen->first;
            }
        }

        cout << year << ": " << topGenre << " (" << maxRevenue << " млн $)\n";
        shown++;
    }
}

// ЗАДАЧА 6 
void task6(const vector<Game>& games) {
    // Издатель -> Разработчик -> Суммарный доход
    map<string, map<string, double>> publisherDeveloperRevenue;

    for (const auto& game : games) {
        publisherDeveloperRevenue[game.publisher][game.developer] += game.globalSales;
    }

    cout << "\n ЗАДАЧА 6\n";

    int shownPublishers = 0;
    for (map<string, map<string, double>>::iterator itPub = publisherDeveloperRevenue.begin(); itPub != publisherDeveloperRevenue.end(); ++itPub) {
        if (shownPublishers >= 10) {
            cout << "\n... и ещё " << publisherDeveloperRevenue.size() - 10 << " издателей\n";
            break;
        }

        // Сортируем разработчиков по доходу
        vector<pair<string, double>> devVec(itPub->second.begin(), itPub->second.end());
        sort(devVec.begin(), devVec.end(),
            [](const pair<string, double>& a, const pair<string, double>& b) { return a.second > b.second; });

        cout << "\nИздатель: " << itPub->first << "\n";
        int topCount = min(5, (int)devVec.size());
        for (int i = 0; i < topCount; ++i) {
            cout << "   " << i + 1 << ". " << devVec[i].first << ": " << devVec[i].second << " млн $\n";
        }
        if (devVec.size() < 5) {
            cout << "   (всего " << devVec.size() << " разработчиков)\n";
        }
        shownPublishers++;
    }
}


int main() {
    setlocale(LC_ALL, "Russian");

    //  ЗАДАЧИ 1-3: работа со словами 
    string wordsFile = "Harry_Potter_1.txt"; 
    vector<string> words = readWordsFromFile(wordsFile);

    if (words.empty()) {
        cerr << "\nВНИМАНИЕ: Файл " << wordsFile << " не найден!\n";
        cerr << "Положите текстовый файл с книгой в папку с проектом.\n\n";
    }
    else {
        cout << "Загружено слов из книги: " << words.size() << endl;
        task1(words);
        task2(words);
        task3(words);
    }

    // ЗАДАЧИ 4-6: работа с играми 
    string gamesFile = "vgsales.csv";
    vector<Game> games = loadGamesFromCSV(gamesFile);

    if (games.empty()) {
        cerr << "\nВНИМАНИЕ: Файл " << gamesFile << " не найден!\n";
        cerr << "Скачайте vgsales.csv и положите в папку с проектом.\n\n";
    }
    else {
        task4(games);
        task5(games);
        task6(games);
    }

    cout << "\nПрограмма завершена. Нажмите Enter для выхода...";
    cin.get();
    return 0;
}