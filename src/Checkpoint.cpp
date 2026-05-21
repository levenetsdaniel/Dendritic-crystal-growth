#include "Checkpoint.h"
#include "PhaseField.h"
#include "TemperatureField.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include <algorithm>


std::string Checkpoint::makeFilename(const std::string& dir, uint64_t frame) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "checkpoint_%08llu.ckpt",
                  static_cast<unsigned long long>(frame));
    return (std::filesystem::path(dir) / buf).string();
}

void Checkpoint::save(const std::string&      dir,
                      uint64_t                frame,
                      const PhaseField&       phi,
                      const TemperatureField& temp) {
    std::filesystem::create_directories(dir);

    const std::string path = makeFilename(dir, frame);
    std::ofstream out(path, std::ios::binary);
    if (!out)
        throw std::runtime_error("Checkpoint::save — не удалось открыть: " + path);

    // Заголовок
    Header hdr;
    hdr.magic    = MAGIC;
    hdr.version  = VERSION;
    hdr.nx       = phi.sizeX();
    hdr.ny       = phi.sizeY();
    hdr.frame    = frame;
    hdr.saved_at = static_cast<int64_t>(std::time(nullptr));
    out.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));

    // PhaseField
    const auto& phi_data  = phi.rawData();
    uint64_t    phi_count = phi_data.size();
    out.write(reinterpret_cast<const char*>(&phi_count), sizeof(phi_count));
    out.write(reinterpret_cast<const char*>(phi_data.data()),
              static_cast<std::streamsize>(phi_count * sizeof(float)));

    // TemperatureField
    const auto& tmp_data  = temp.rawData();
    uint64_t    tmp_count = tmp_data.size();
    out.write(reinterpret_cast<const char*>(&tmp_count), sizeof(tmp_count));
    out.write(reinterpret_cast<const char*>(tmp_data.data()),
              static_cast<std::streamsize>(tmp_count * sizeof(float)));

    if (!out)
        throw std::runtime_error("Checkpoint::save — ошибка записи: " + path);

    std::cout << "[Checkpoint] Сохранён: " << path
              << "  (" << (phi_count + tmp_count) * sizeof(float) / 1024 / 1024
              << " МБ)\n";
}

bool Checkpoint::load(const std::string& path,
                      uint64_t&          frame,
                      PhaseField&        phi,
                      TemperatureField&  temp) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "[Checkpoint] Не удалось открыть: " << path << "\n";
        return false;
    }

    // Заголовок
    Header hdr{};
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in || hdr.magic != MAGIC) {
        std::cerr << "[Checkpoint] Неверный формат файла: " << path << "\n";
        return false;
    }
    if (hdr.version != VERSION) {
        std::cerr << "[Checkpoint] Неподдерживаемая версия "
                  << hdr.version << ": " << path << "\n";
        return false;
    }
    if (hdr.nx != phi.sizeX() || hdr.ny != phi.sizeY()) {
        std::cerr << "[Checkpoint] Размер сетки не совпадает: файл "
                  << hdr.nx << "×" << hdr.ny
                  << ", симуляция " << phi.sizeX() << "×" << phi.sizeY() << "\n";
        return false;
    }

    // PhaseField
    uint64_t phi_count = 0;
    in.read(reinterpret_cast<char*>(&phi_count), sizeof(phi_count));
    std::vector<float> phi_buf(phi_count);
    in.read(reinterpret_cast<char*>(phi_buf.data()),
            static_cast<std::streamsize>(phi_count * sizeof(float)));
    phi.loadData(phi_buf);

    // TemperatureField
    uint64_t tmp_count = 0;
    in.read(reinterpret_cast<char*>(&tmp_count), sizeof(tmp_count));
    std::vector<float> tmp_buf(tmp_count);
    in.read(reinterpret_cast<char*>(tmp_buf.data()),
            static_cast<std::streamsize>(tmp_count * sizeof(float)));
    temp.loadData(tmp_buf);

    if (!in) {
        std::cerr << "[Checkpoint] Ошибка чтения: " << path << "\n";
        return false;
    }

    frame = hdr.frame;
    std::time_t ts = static_cast<std::time_t>(hdr.saved_at);
    std::cout << "[Checkpoint] Загружен: " << path
              << "\n             Кадр: "   << frame
              << ", сохранён: "            << std::ctime(&ts);
    return true;
}

// находит checkpoint с наибольшим frame в директории.
std::string Checkpoint::latest(const std::string& dir) {
    if (!std::filesystem::exists(dir)) return "";

    std::string best;
    uint64_t    best_frame = 0;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        const std::string name = entry.path().filename().string();
        if (name.size() < 5 || name.substr(name.size() - 5) != ".ckpt") continue;

        uint64_t f = 0;
        if (std::sscanf(name.c_str(), "checkpoint_%llu.ckpt",
                        reinterpret_cast<unsigned long long*>(&f)) == 1) {
            if (f >= best_frame) {
                best_frame = f;
                best       = entry.path().string();
            }
        }
    }
    return best;
}

void Checkpoint::printInfo(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) { std::cerr << "Не удалось открыть: " << path << "\n"; return; }

    Header hdr{};
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in || hdr.magic != MAGIC) { std::cerr << "Неверный формат.\n"; return; }

    std::time_t ts = static_cast<std::time_t>(hdr.saved_at);
    std::cout << "=== Checkpoint Info ===\n"
              << "  File   : " << path    << "\n"
              << "  Grid   : " << hdr.nx  << " × " << hdr.ny << "\n"
              << "  Frame  : " << hdr.frame << "\n"
              << "  Saved  : " << std::ctime(&ts);
}