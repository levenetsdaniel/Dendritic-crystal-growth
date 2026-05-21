#ifndef CRYSTAL_CHECKPOINT_H
#define CRYSTAL_CHECKPOINT_H


#include <string>
#include <cstdint>
#include <filesystem>

class PhaseField;
class TemperatureField;

class Checkpoint {
public:
    // Сохраняет checkpoint в директорию dir
    static void save(const std::string& dir,
                     uint64_t            frame,
                     const PhaseField&   phi,
                     const TemperatureField& temp);

    // Загружает checkpoint из файла path
    static bool load(const std::string& path,
                     uint64_t&          frame,
                     PhaseField&        phi,
                     TemperatureField&  temp);

    // Возвращает путь к последнему checkpoint в директории
    static std::string latest(const std::string& dir);

    // Печатает метаданные checkpoint без загрузки полей
    static void printInfo(const std::string& path);

private:
    static constexpr uint32_t MAGIC   = 0x53595243;
    static constexpr uint32_t VERSION = 1;

    struct Header {
        uint32_t magic;
        uint32_t version;
        uint64_t nx;
        uint64_t ny;
        uint64_t frame;
        int64_t  saved_at;
    };

    static std::string makeFilename(const std::string& dir, uint64_t frame);
};

#endif // CRYSTAL_CHECKPOINT_H