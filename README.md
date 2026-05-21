# Crystal: Phase Field Simulator


## Описание

**Crystal** — это приложение для моделирования динамики роста кристаллов и затвердевания различных материалов. Симулятор реализует уравнения Карма-Раппель (Karma-Rappel) для фазового поля и поля температуры, позволяя наблюдать сложные паттерны кристаллизации в реальном времени.

### Ключевые особенности

- **Визуализация в реальном времени** с использованием SFML
- **Поддержка различных материалов**: SCN, Silver, Aluminum Oxide, Ice, Silicon с физическими параметрами
- **Сохранение метрик и снимков** для анализа динамики процесса
- **Система контрольных точек (Checkpoints)** для возобновления симуляции
- **Параллелизм** с использованием OpenMP для ускорения вычислений
- **Конфигурируемость** через JSON файлы

## Требования

### Зависимости

- **CMake** >= 3.16
- **C++17** компилятор (Clang, GCC, или MSVC)
- **SFML 3** — Simple and Fast Multimedia Library
- **nlohmann_json** — JSON для современного C++
- **OpenMP** — для многопоточности

### Установка зависимостей

#### macOS (Homebrew)

```bash
brew install cmake sfml nlohmann-json libomp
```

#### Ubuntu/Debian

```bash
sudo apt-get install cmake libsfml-dev nlohmann-json3-dev libomp-dev
```

## Сборка

### Стандартная сборка (Release)

```bash
# Создать директорию сборки
mkdir build
cd build

# Сгенерировать Makefile
cmake ..

# Собрать проект
make

# Исполняемый файл: build/crystal
```

## Использование

### Базовый запуск

```bash
./crystal
```

Запустит симуляцию со значениями по умолчанию (материал SCN, без сохранения метрик).

### С параметрами

```bash
# Выбрать материал
./crystal -m SILVER
./crystal --material ICE

# Сохранить метрики и кадры
./crystal -m ALUMINUM_OXIDE --with-metrics

# Возобновить из последней контрольной точки
./crystal --resume

# Комбинированный пример
./crystal -m SILICON --with-metrics
```

### Доступные материалы

- **SCN** — Succinonitrile (по умолчанию)
- **SILVER** — Silver
- **ALUMINUM_OXIDE** — Aluminum Oxide (Sapphire)
- **ICE** — Ice/Water
- **SILICON** — Silicon

### Справка

```bash
./crystal -h
./crystal --help
```

## Конфигурация

Параметры симуляции задаются в JSON файле (`configs/default.json`):

```json
{
  "material": "SCN",
  "grid_size": 1600,
  "seed_radius": 10.0,
  "render_scale": 0.5,
  "interval_render": 5,
  "interval_csv": 100,
  "interval_snapshot": 500,
  "interval_ostream": 1000,
  "interval_checkpoint": 5000,
  "save_metrics": true
}
```

### Параметры конфигурации

| Параметр | Описание | По умолчанию |
|----------|---------|---|
| `material` | Материал для симуляции | SCN |
| `grid_size` | Размер сетки (N×N узлов) | 1600 |
| `seed_radius` | Радиус начального зерна | 10.0 |
| `render_scale` | Масштаб визуализации (0-1) | 0.5 |
| `interval_render` | Кадры между обновлением визуализации | 5 |
| `interval_csv` | Кадры между сохранением CSV | 100 |
| `interval_snapshot` | Кадры между сохранением снимков | 500 |
| `interval_ostream` | Кадры между выводом в консоль | 1000 |
| `interval_checkpoint` | Кадры между контрольными точками | 5000 |
| `save_metrics` | Сохранять ли метрики и изображения | true |

## Структура проекта

```
crystal/
├── src/               # Исходные файлы (.cpp)
├── include/          # Заголовочные файлы (.h)
├── configs/          # Конфигурационные JSON файлы
├── CMakeLists.txt    # Конфигурация CMake
└── README.md         # Этот файл
```

### Основные модули

- **PhaseField** — вычисления фазового поля
- **TemperatureField** — вычисления поля температуры
- **FieldRender** — визуализация в реальном времени
- **Metrics** — запись метрик и изображений
- **MaterialsLibrary** — библиотека физических параметров материалов
- **ConfigLoader** — загрузка конфигурации из JSON
- **Statistics** — статистический анализ результатов

## Результаты

При запуске с флагом `--with-metrics` симулятор создает:

- **runs/frames/** — PNG снимки состояния поля
- **runs/metrics.csv** — временные ряды метрик
- **runs/final_statistics.csv** — итоговая статистика
- **checkpoints/** — контрольные точки для возобновления

## Примеры команд

```bash
# Быстрая демонстрация с визуализацией
./crystal -m ICE --no-metrics

# Полная симуляция с сохранением всех данных
./crystal -m SILVER --with-metrics

# Возобновить прерванную симуляцию
./crystal --resume

# Новая симуляция с другим материалом
./crystal -m ALUMINUM_OXIDE --with-metrics
```