// gridmaker.cpp - реализация генератора сетки
// by lakladon

#include "gridmaker.h"
#include <functional>

// конструктор - он пустой потому что нечего инициализировать
// просто чтобы был
GridMaker::GridMaker() {
    // тут ничего нет, и не надо
}

// функция хеширования строки
// использует стандартный хеш из C++, но я ещё добавил свои магические числа
// чтобы было круче и более случайно
unsigned int GridMaker::hashString(const std::string& input) {
    // используем std::hash для базового хеширования
    std::hash<std::string> hashFunc;
    unsigned int hash = hashFunc(input);
    
    // добавляю свои магические числа для лучшего распределения
    // эти числа я придумал сам, они типа магические
    hash ^= (hash >> 13);
    hash *= 0x5bd1e995;
    hash ^= (hash >> 15);
    
    return hash;
}

// получает цвет для ячейки по координатам и хешу
// цвет берётся из палитры (0-4)
int GridMaker::getColorFromHash(unsigned int hash, int x, int y) {
    // комбинируем хеш с координатами
    // чтобы каждая ячейка была уникальной но детерминированной
    unsigned int cellHash = hash ^ (x * 374761393) ^ (y * 668265263);
    
    // ещё немного перемешиваем
    cellHash ^= (cellHash >> 13);
    cellHash *= 1274126177;
    cellHash ^= (cellHash >> 16);
    
    // берём остаток от деления на размер палитры
    return cellHash % PALETTE_SIZE;
}

// главная функция - генерирует сетку 12x12
// левая половина генерируется случайно, правая - зеркальное отражение
// получается симметричная аватарка как в гитхабе
std::vector<std::vector<int>> GridMaker::generate(const std::string& input) {
    // создаём сетку 12x12 заполненную нулями
    // 12 - это стандартный размер как в гитхабе
    const int SIZE = 12;
    std::vector<std::vector<int>> grid(SIZE, std::vector<int>(SIZE, 0));
    
    // получаем хеш из входной строки
    unsigned int hash = hashString(input);
    
    // заполняем только левую половину (6 столбцов)
    // потом отразим их направо
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE / 2; x++) {
            grid[y][x] = getColorFromHash(hash, x, y);
        }
    }
    
    // теперь отражаем левую половину на правую
    // это делает аватарку симметричной и красивой
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE / 2; x++) {
            // берём значение слева и ставим справа зеркально
            grid[y][SIZE - 1 - x] = grid[y][x];
        }
    }
    
    // возвращаем готовую сетку
    return grid;
}