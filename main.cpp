#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>
#include <sstream>
#include <functional>
#include <memory>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
#endif

// by lakladon
// Проект: hashface - Генератор аватарок как в GitHub

class HashFace {
private:
    static constexpr int SIZE = 12; // Размер аватарки 12x12
    static constexpr int HALF = SIZE / 2; // Половина для симметрии
    
    struct Color {
        unsigned char r, g, b;
        
        // Конструктор по умолчанию
        Color() : r(0), g(0), b(0) {}
        
        Color(unsigned char red, unsigned char green, unsigned char blue) 
            : r(red), g(green), b(blue) {}
            
        // ANSI escape code для цвета в терминале
        std::string ansiFg() const {
            return "\033[38;2;" + std::to_string(r) + ";" + 
                   std::to_string(g) + ";" + std::to_string(b) + "m";
        }
        
        std::string ansiBg() const {
            return "\033[48;2;" + std::to_string(r) + ";" + 
                   std::to_string(g) + ";" + std::to_string(b) + "m";
        }
    };
    
    // Палитра цветов как в GitHub
    std::vector<Color> palette;
    std::mt19937 rng;
    std::vector<std::vector<int>> pixels;
    Color bgColor;
    Color fgColor;
    
    // Инициализация палитры
    void initPalette() {
        palette = {
            Color(235, 237, 240), // Светло-серый
            Color(155, 193, 188), // Мятный
            Color(106, 153, 168), // Сине-зеленый
            Color(59, 106, 124),  // Темно-синий
            Color(34, 70, 89)     // Очень темный синий
        };
    }
    
    // Хеширование строки для детерминированной генерации
    size_t hashString(const std::string& str) const {
        size_t hash = 14695981039346656037ULL;
        for (char c : str) {
            hash ^= static_cast<size_t>(c);
            hash *= 1099511628211ULL;
        }
        return hash;
    }
    
    // Генерация паттерна на основе хеша
    void generatePattern(size_t seed) {
        std::mt19937 patternRng(seed);
        
        // Выбор цвета из палитры (индексы 1-4, исключая фоновый)
        std::uniform_int_distribution<int> colorDist(1, palette.size() - 1);
        fgColor = palette[colorDist(patternRng)];
        
        // Генерация левой половины
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < HALF; ++x) {
                std::bernoulli_distribution dist(0.5); // 50% вероятность пикселя
                pixels[y][x] = dist(patternRng) ? 1 : 0;
            }
        }
        
        // Зеркальное отражение для правой половины
        for (int y = 0; y < SIZE; ++y) {
            for (int x = HALF; x < SIZE; ++x) {
                pixels[y][x] = pixels[y][SIZE - 1 - x];
            }
        }
        
        // Гарантируем хотя бы несколько пикселей в паттерне
        int count = 0;
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                count += pixels[y][x];
            }
        }
        
        if (count < 5) {
            // Если слишком мало пикселей, добавляем немного случайных
            std::uniform_int_distribution<int> posDist(0, SIZE - 1);
            for (int i = 0; i < 5; ++i) {
                int x = posDist(patternRng);
                int y = posDist(patternRng);
                pixels[y][x] = 1;
            }
        }
    }
    
public:
    HashFace(const std::string& input) 
        : rng(hashString(input)), 
          pixels(SIZE, std::vector<int>(SIZE, 0)) {
        
        initPalette(); // Инициализируем палитру
        bgColor = palette[0]; // Устанавливаем фоновый цвет
        generatePattern(hashString(input));
    }
    
    // Отображение аватарки в терминале
    void display() const {
        std::cout << "\n  HashFace Avatar (12x12)\n";
        std::cout << "  " << std::string(27, '-') << "\n";
        
        for (int y = 0; y < SIZE; ++y) {
            std::cout << "  ";
            for (int x = 0; x < SIZE; ++x) {
                if (pixels[y][x]) {
                    std::cout << fgColor.ansiBg() << "   " << "\033[0m";
                } else {
                    std::cout << bgColor.ansiBg() << "   " << "\033[0m";
                }
            }
            std::cout << "\n";
        }
        
        std::cout << "  " << std::string(27, '-') << "\n";
        std::cout << "  Color: RGB(" << std::setw(3) << (int)fgColor.r << ", "
                  << std::setw(3) << (int)fgColor.g << ", "
                  << std::setw(3) << (int)fgColor.b << ")\n";
    }
    
    // Вывод в формате ASCII арта
    void displayAscii() const {
        std::cout << "\n  ASCII Version:\n";
        for (int y = 0; y < SIZE; ++y) {
            std::cout << "  ";
            for (int x = 0; x < SIZE; ++x) {
                std::cout << (pixels[y][x] ? "██" : "  ");
            }
            std::cout << "\n";
        }
    }
    
    // Вывод в виде строки с символами
    std::string toString() const {
        std::stringstream ss;
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                ss << (pixels[y][x] ? '#' : '.');
            }
            if (y < SIZE - 1) ss << '\n';
        }
        return ss.str();
    }
};

void printUsage(const char* programName) {
    std::cout << "HashFace v1.0 - GitHub-style avatar generator\n";
    std::cout << "by lakladon\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " --text <your text>\n";
    std::cout << "  " << programName << " --name <username>\n";
    std::cout << "  " << programName << " --email <email>\n";
    std::cout << "  " << programName << " --random\n";
    std::cout << "  " << programName << " --help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --name john.doe\n";
    std::cout << "  " << programName << " --text \"Hello World\"\n";
    std::cout << "  " << programName << " --email user@example.com\n";
}

void setupConsole() {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) {
            return;
        }
        
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) {
            return;
        }
        
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    #endif
}

int main(int argc, char* argv[]) {
    // Настройка консоли для поддержки ANSI цветов
    setupConsole();
    
    // Проверка аргументов командной строки
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string mode = argv[1];
    
    if (mode == "--help" || mode == "-h") {
        printUsage(argv[0]);
        return 0;
    }
    
    std::string input;
    
    // Обработка различных режимов
    if (mode == "--random") {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 999999);
        input = "random_" + std::to_string(dis(gen));
        std::cout << "Generated random seed: " << input << "\n";
    }
    else if (mode == "--text" || mode == "-t") {
        if (argc < 3) {
            std::cerr << "Error: Missing text argument\n";
            return 1;
        }
        input = "text:" + std::string(argv[2]);
    }
    else if (mode == "--name" || mode == "-n") {
        if (argc < 3) {
            std::cerr << "Error: Missing name argument\n";
            return 1;
        }
        input = "name:" + std::string(argv[2]);
    }
    else if (mode == "--email" || mode == "-e") {
        if (argc < 3) {
            std::cerr << "Error: Missing email argument\n";
            return 1;
        }
        input = "email:" + std::string(argv[2]);
    }
    else {
        std::cerr << "Unknown option: " << mode << "\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        HashFace avatar(input);
        
        std::cout << "\n╔════════════════════════════════╗";
        std::cout << "\n║       HASHFACE v1.0           ║";
        std::cout << "\n║     by lakladon               ║";
        std::cout << "\n╚════════════════════════════════╝\n";
        
        avatar.display();
        avatar.displayAscii();
        
        std::cout << "\nString representation:\n";
        std::cout << avatar.toString() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
