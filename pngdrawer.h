// pngdrawer.h - заголовок для рисования PNG
// by lakladon

#ifndef PNGDRAWER_H
#define PNGDRAWER_H

#include <string>
#include <vector>

// структура для цвета RGB
// простая но работает
struct Color {
    unsigned char r;  // красный
    unsigned char g;  // зелёный
    unsigned char b;  // синий
    unsigned char a;  // альфа (прозрачность)
    
    // конструктор для удобства
    Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
};

// класс для рисования PNG файлов
// пишет PNG вручную без библиотек, это же круто!
class PngDrawer {
public:
    // конструктор
    PngDrawer();
    
    // сохраняет сетку в PNG файл
    // grid - это сетка цветов (значения 0-4)
    // filename - имя файла для сохранения
    // scale - масштаб (умножает размер, по умолчанию 10)
    bool saveGrid(const std::vector<std::vector<int>>& grid, 
                  const std::string& filename, 
                  int scale = 10);

private:
    // палитра цветов как в гитхабе
    // 5 цветов от светло-зелёного до тёмно-зелёного
    std::vector<Color> palette;
    
    // инициализирует палитру
    void initPalette();
    
    // пишет 4 байта в big-endian формате
    // PNG требует такой формат
    void writeInt32(std::vector<unsigned char>& data, unsigned int value);
    
    // вычисляет CRC для чанка PNG
    // это типа контрольная сумма
    unsigned int calcCRC(const std::vector<unsigned char>& data);
    
    // кодирует данные в PNG формат
    // использует простое сжатие zlib (deflate)
    std::vector<unsigned char> encodePng(const std::vector<std::vector<Color>>& pixels);
};

#endif // PNGDRAWER_H