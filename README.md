# HashFace - GitHub-style Avatar Generator

Генератор дефолтных аватарок в стиле GitHub на C++.

## Описание

HashFace создаёт уникальные идентификоны (identicons) на основе MD5 хеша входной строки. Каждый аватар представляет собой симметричный паттерн 5x5 (по умолчанию), где цвет и форма определяются хешем.

## Требования

- C++17 совместимый компилятор
- CMake 3.10+
- zlib (zlib1g-dev)

### Установка зависимостей (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake zlib1g-dev
```

## Сборка

```bash
mkdir build && cd build
cmake ..
make
```

## Использование

```bash
./hashface [options] <input_string>
```

### Опции

| Опция | Описание | По умолчанию |
|-------|----------|--------------|
| `-o <file>` | Имя выходного файла | `avatar.png` |
| `-s <size>` | Размер изображения в пикселях | `420` |
| `-g <grid>` | Размер сетки | `5` |
| `-h, --help` | Показать справку | - |

### Примеры

```bash
# Базовое использование
./hashface "john@example.com"

# Указать выходной файл и размер
./hashface -o user123.png -s 256 "user123"

# Использовать сетку 7x7
./hashface -g 7 "octocat"
```

## Как это работает

1. Вычисляется MD5 хеш входной строки
2. Первые 3 байта хеша определяют цвет аватара
3. Биты хеша определяют, какие ячейки сетки будут закрашены
4. Паттерн зеркально отражается по горизонтали (как в GitHub)
5. Результат сохраняется в PNG файл

## Структура проекта

```
hashface/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── avatar_generator.hpp
│   └── md5.hpp
└── src/
    ├── main.cpp
    ├── avatar_generator.cpp
    └── md5.cpp
```

## Лицензия

MIT