# Makefile для сборки hashface
# by lakladon

# компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lz

# файлы проекта
SOURCES = zapuskator.cpp gridmaker.cpp pngdrawer.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = hashface

# цель по умолчанию - собрать всё
all: $(TARGET)

# линковка исполняемого файла
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# компиляция объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# очистка собранных файлов
clean:
	rm -f $(OBJECTS) $(TARGET) avatar.png

# запуск с примером
test: $(TARGET)
	./$(TARGET) --name "test"

# установить zlib (для Ubuntu/Debian)
install-deps:
	sudo apt-get install -y zlib1g-dev

.PHONY: all clean test install-deps