
// zapuskator.cpp - главный файл запускалки
// by lakladon

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "gridmaker.h"
#include "pngdrawer.h"

// показывает справку по использованию
// когда пользователь не знает что делать
void showHelp() {
    std::cout << "hashface -  avatar generator\n";
    std::cout << "by lakladon\n\n";
    std::cout << "Usage:\n";
    std::cout << "  hashface --name John        generate from name\n";
    std::cout << "  hashface --text \"hello\"     generate from text\n";
    std::cout << "  hashface --email a@b.com    generate from email\n";
    std::cout << "  hashface --random           random avatar\n";
    std::cout << "  hashface --output file.png  set output filename\n";
    std::cout << "  hashface --help             show this help\n\n";
    std::cout << "Avatars are saved to avatar.png by default\n";
    std::cout << "Size: 120x120 pixels (12x12 grid scaled x10)\n";
}

// выводит аватарку в терминал
// типа ASCII art но цветной
void printAvatar(const std::vector<std::vector<int>>& grid) {
    // символы для разных уровней цвета
    // чем темнее цвет тем больше символ заполнен
    const char* symbols[] = {"  ", "░░", "▒▒", "▓▓", "██"};
    
    std::cout << "\n+------------------------+\n";
    for (size_t y = 0; y < grid.size(); y++) {
        std::cout << "|";
        for (size_t x = 0; x < grid[y].size(); x++) {
            int colorIndex = grid[y][x];
            if (colorIndex < 0) colorIndex = 0;
            if (colorIndex > 4) colorIndex = 4;
            std::cout << symbols[colorIndex];
        }
        std::cout << "|\n";
    }
    std::cout << "+------------------------+\n\n";
}

// генерирует случайную строку
// для режима --random
std::string generateRandomString() {
    // разные символы для случайной строки
    const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    
    // делаем строку длиной 16 символов
    // этого хватит для хорошей уникальности
    for (int i = 0; i < 16; i++) {
        result += chars[rand() % (sizeof(chars) - 1)];
    }
    
    return result;
}

// главная функция - точка входа
// тут всё начинается
int main(int argc, char* argv[]) {
    // инициализируем генератор случайных чисел
    // для режима --random
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // строка для генерации аватарки
    std::string inputString;
    
    // имя выходного файла (по умолчанию avatar.png)
    std::string outputFile = "avatar.png";
    
    // разбираем аргументы командной строки
    // проверяем каждый аргумент
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            // показываем справку и выходим
            showHelp();
            return 0;
        }
        else if (arg == "--name" && i + 1 < argc) {
            // берём имя из следующего аргумента
            inputString = "name:" + std::string(argv[++i]);
        }
        else if (arg == "--text" && i + 1 < argc) {
            // берём текст из следующего аргумента
            inputString = "text:" + std::string(argv[++i]);
        }
        else if (arg == "--email" && i + 1 < argc) {
            // берём email из следующего аргумента
            inputString = "email:" + std::string(argv[++i]);
        }
        else if (arg == "--random") {
            // генерируем случайную строку
            inputString = "random:" + generateRandomString();
        }
        else if (arg == "--output" && i + 1 < argc) {
            // берём имя файла из следующего аргумента
            outputFile = argv[++i];
        }
        else {
            // неизвестный аргумент
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cerr << "Use --help for usage info\n";
            return 1;
        }
    }
    
    // если ничего не передали, показываем справку
    if (inputString.empty()) {
        std::cout << "Error: no input source specified\n\n";
        showHelp();
        return 1;
    }
    
    // создаём генератор сетки
    GridMaker gridMaker;
    
    // генерируем сетку 12x12
    std::cout << "Gen avatar from: " << inputString << "\n";
    std::vector<std::vector<int>> grid = gridMaker.generate(inputString);
    
    // выводим аватарку в терминал
    // чтобы было красиво
    printAvatar(grid);
    
    // создаём рисовальщик PNG
    PngDrawer pngDrawer;
    
    // сохраняем в файл с масштабом x10
    // получается 120x120 пикселей
    
    if (pngDrawer.saveGrid(grid, outputFile, 10)) {
        std::cout << "Avatar saved to: " << outputFile << "\n";
        return 0;
    } else {
        std::cerr << "Error: failed to save file\n";
        return 1;
    }
}
