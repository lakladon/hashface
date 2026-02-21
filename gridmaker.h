// gridmaker.h - заголовок для создания сетки аватарки
// by lakladon

#ifndef GRIDMAKER_H
#define GRIDMAKER_H

#include <string>
#include <vector>

// короче это класс который делает сетку 12x12 для аватарки
// как в гитхабе, типа такие квадратики цветные
class GridMaker {
public:
    // палитра цветов как в гитхабе (5 цветов)
    // я их сам подобрал, выглядит похоже
    static const int PALETTE_SIZE = 5;
    
    // конструктор пустой, ничего не надо
    GridMaker();
    
    // главная функция - генерирует сетку из строки
    // берет строку, хеширует её и делает паттерн
    std::vector<std::vector<int>> generate(const std::string& input);
    
private:
    // функция хеширования - превращает строку в число
    // чтобы из одинаковых строк получались одинаковые аватарки
    unsigned int hashString(const std::string& input);
    
    // получает цвет из хеша для конкретной ячейки
    int getColorFromHash(unsigned int hash, int x, int y);
};

#endif // GRIDMAKER_H