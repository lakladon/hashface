// pngdrawer.cpp - реализация рисования PNG
// by lakladon

#include "pngdrawer.h"
#include <fstream>
#include <cstring>
#include <zlib.h>

// конструктор цвета
Color::Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    : r(red), g(green), b(blue), a(alpha) {
    // просто сохраняем значения
}

// конструктор PngDrawer
PngDrawer::PngDrawer() {
    // инициализируем палитру при создании
    initPalette();
}

// инициализация палитры как в гитхабе
// 5 оттенков зелёного (ну или каких захотите)
void PngDrawer::initPalette() {
    // цвета как в гитхабе - от самого светлого до тёмного
    // я их подобрал чтобы было похоже
    palette.push_back(Color(235, 237, 240));  // 0 - пусто (серый фон)
    palette.push_back(Color(155, 233, 168));  // 1 - уровень 1 (светло-зелёный)
    palette.push_back(Color(64, 196, 99));    // 2 - уровень 2 (зелёный)
    palette.push_back(Color(48, 161, 78));    // 3 - уровень 3 (тёмно-зелёный)
    palette.push_back(Color(33, 110, 57));    // 4 - уровень 4 (очень тёмный)
}

// пишет 4 байта в big-endian формате
// PNG требует именно такой порядок байтов
void PngDrawer::writeInt32(std::vector<unsigned char>& data, unsigned int value) {
    data.push_back((value >> 24) & 0xFF);
    data.push_back((value >> 16) & 0xFF);
    data.push_back((value >> 8) & 0xFF);
    data.push_back(value & 0xFF);
}

// таблица для CRC
// это нужно для контрольной суммы в PNG
static unsigned int crcTable[256];
static bool crcTableComputed = false;

// вычисляет таблицу CRC
// делается один раз при старте
static void makeCRCTable() {
    if (crcTableComputed) return;
    
    for (unsigned int n = 0; n < 256; n++) {
        unsigned int c = n;
        for (int k = 0; k < 8; k++) {
            if (c & 1) {
                c = 0xedb88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crcTable[n] = c;
    }
    crcTableComputed = true;
}

// вычисляет CRC для данных
unsigned int PngDrawer::calcCRC(const std::vector<unsigned char>& data) {
    makeCRCTable();
    unsigned int crc = 0xffffffffL;
    
    for (size_t i = 0; i < data.size(); i++) {
        crc = crcTable[(crc ^ data[i]) & 0xff] ^ (crc >> 8);
    }
    
    return crc ^ 0xffffffffL;
}

// кодирует изображение в PNG формат
// использует zlib для сжатия
std::vector<unsigned char> PngDrawer::encodePng(const std::vector<std::vector<Color>>& pixels) {
    std::vector<unsigned char> result;
    
    // сначала собираем сырые данные изображения
    // каждый ряд начинается с байта фильтра (0 = без фильтра)
    std::vector<unsigned char> rawData;
    
    int height = pixels.size();
    int width = height > 0 ? pixels[0].size() : 0;
    
    for (int y = 0; y < height; y++) {
        // байт фильтра для строки (0 = None)
        rawData.push_back(0);
        
        for (int x = 0; x < width; x++) {
            // пишем RGBA для каждого пикселя
            rawData.push_back(pixels[y][x].r);
            rawData.push_back(pixels[y][x].g);
            rawData.push_back(pixels[y][x].b);
            rawData.push_back(pixels[y][x].a);
        }
    }
    
    // сжимаем данные с помощью zlib
    uLongf compressedSize = compressBound(rawData.size());
    std::vector<unsigned char> compressed(compressedSize);
    
    int zlibResult = compress2(compressed.data(), &compressedSize,
                               rawData.data(), rawData.size(), 9);
    
    if (zlibResult != Z_OK) {
        // если сжатие не удалось, возвращаем пустой вектор
        return result;
    }
    
    // усекаем до реального размера
    compressed.resize(compressedSize);
    
    // теперь собираем PNG файл
    
    // 1. PNG сигнатура (магические байты)
    const unsigned char signature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    for (int i = 0; i < 8; i++) {
        result.push_back(signature[i]);
    }
    
    // 2. IHDR чанк (заголовок)
    std::vector<unsigned char> ihdrData;
    writeInt32(ihdrData, width);           // ширина
    writeInt32(ihdrData, height);          // высота
    ihdrData.push_back(8);                 // бит на канал
    ihdrData.push_back(6);                 // тип цвета (RGBA)
    ihdrData.push_back(0);                 // сжатие (deflate)
    ihdrData.push_back(0);                 // метод фильтрации
    ihdrData.push_back(0);                 // чересстрочность (нет)
    
    // пишем IHDR чанк
    writeInt32(result, 13);  // длина данных IHDR
    result.push_back('I');
    result.push_back('H');
    result.push_back('D');
    result.push_back('R');
    for (size_t i = 0; i < ihdrData.size(); i++) {
        result.push_back(ihdrData[i]);
    }
    
    // CRC для IHDR (включая тип и данные)
    std::vector<unsigned char> ihdrForCrc;
    ihdrForCrc.push_back('I');
    ihdrForCrc.push_back('H');
    ihdrForCrc.push_back('D');
    ihdrForCrc.push_back('R');
    for (size_t i = 0; i < ihdrData.size(); i++) {
        ihdrForCrc.push_back(ihdrData[i]);
    }
    writeInt32(result, calcCRC(ihdrForCrc));
    
    // 3. IDAT чанк (данные изображения)
    writeInt32(result, compressed.size());
    result.push_back('I');
    result.push_back('D');
    result.push_back('A');
    result.push_back('T');
    for (size_t i = 0; i < compressed.size(); i++) {
        result.push_back(compressed[i]);
    }
    
    // CRC для IDAT
    std::vector<unsigned char> idatForCrc;
    idatForCrc.push_back('I');
    idatForCrc.push_back('D');
    idatForCrc.push_back('A');
    idatForCrc.push_back('T');
    for (size_t i = 0; i < compressed.size(); i++) {
        idatForCrc.push_back(compressed[i]);
    }
    writeInt32(result, calcCRC(idatForCrc));
    
    // 4. IEND чанк (конец файла)
    writeInt32(result, 0);  // длина 0
    result.push_back('I');
    result.push_back('E');
    result.push_back('N');
    result.push_back('D');
    
    // CRC для IEND
    std::vector<unsigned char> iendForCrc;
    iendForCrc.push_back('I');
    iendForCrc.push_back('E');
    iendForCrc.push_back('N');
    iendForCrc.push_back('D');
    writeInt32(result, calcCRC(iendForCrc));
    
    return result;
}

// сохраняет сетку в PNG файл
// grid - сетка с индексами цветов (0-4)
// filename - имя файла
// scale - во сколько раз увеличить
bool PngDrawer::saveGrid(const std::vector<std::vector<int>>& grid,
                         const std::string& filename,
                         int scale) {
    // проверяем что сетка не пустая
    if (grid.empty() || grid[0].empty()) {
        return false;
    }
    
    // получаем размеры исходной сетки
    int originalHeight = grid.size();
    int originalWidth = grid[0].size();
    
    // создаём пиксели с учётом масштаба
    // каждый "квадратик" сетки становится scale x scale пикселей
    std::vector<std::vector<Color>> pixels(originalHeight * scale,
                                           std::vector<Color>(originalWidth * scale,
                                           Color(0, 0, 0)));
    
    // заполняем пиксели
    for (int y = 0; y < originalHeight; y++) {
        for (int x = 0; x < originalWidth; x++) {
            // получаем цвет из палитры
            int colorIndex = grid[y][x];
            if (colorIndex < 0 || colorIndex >= (int)palette.size()) {
                colorIndex = 0;  // если что-то не так, берём первый цвет
            }
            Color c = palette[colorIndex];
            
            // заполняем квадратик scale x scale
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    pixels[y * scale + dy][x * scale + dx] = c;
                }
            }
        }
    }
    
    // кодируем в PNG
    std::vector<unsigned char> pngData = encodePng(pixels);
    
    if (pngData.empty()) {
        return false;
    }
    
    // записываем в файл
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(pngData.data()), pngData.size());
    file.close();
    
    return true;
}