#pragma once
#include "instruction.h"
#include "src/core/json.h"
#include "src/core/fsutil.h"

#include <vector>
#include <fstream>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <variant>
#include <cstring>
#include <stdexcept>


using namespace stg;
class STGBin {

    // 外部からセットする想定
    std::unordered_map<std::string, int> FLAGS_TABLE;
    std::unordered_map<std::string, int> ENTITY_TABLE;
    rj::Document EVENT_TABLE;
    rj::Value PARAM_TABLE;

    // ------------------------------------------------
    // little endian 書き込みヘルパ
    // ------------------------------------------------

    inline void writeU8(std::vector<uint8_t>& buf, uint8_t v) {
        buf.push_back(v);
    }

    inline void writeU16(std::vector<uint8_t>& buf, uint16_t v) {
        buf.push_back(v & 0xFF);
        buf.push_back((v >> 8) & 0xFF);
    }

    inline void writeU32(std::vector<uint8_t>& buf, uint32_t v) {
        for (int i = 0; i < 4; ++i)
            buf.push_back((v >> (8 * i)) & 0xFF);
    }

    inline void writeI16(std::vector<uint8_t>& buf, int16_t v) {
        writeU16(buf, static_cast<uint16_t>(v));
    }

    // ------------------------------------------------
    // packParam
    // ------------------------------------------------

    std::vector<uint8_t> packParam(const Instruction& ins) {
        std::vector<uint8_t> code;

        std::visit([&](auto&& cmd) {
            using T = std::decay_t<decltype(cmd)>;

            // shutdown
            if constexpr (std::is_same_v<T, Shutdown>) {
                writeU8(code, 0);
            }

            // wait
            else if constexpr (std::is_same_v<T, Wait>) {
                writeU8(code, 1);
                writeU32(code, static_cast<uint32_t>(cmd.ms));
            }

            // waitUntil
            else if constexpr (std::is_same_v<T, WaitUntil>) {
                writeU8(code, 2);

                auto it = FLAGS_TABLE.find(cmd.condition);
                if (it == FLAGS_TABLE.end())
                    throw std::runtime_error("unknown flag");

                writeU16(code, static_cast<uint16_t>(it->second));
            }

            // spawn
            else if constexpr (std::is_same_v<T, Spawn>) {
                writeU8(code, 3);

                if (cmd.entityType != "enemyBezier")
                    throw std::runtime_error("spawn: unknown enemy");

                int entityID = ENTITY_TABLE.at(cmd.entityType);

                int16_t x = std::get<int64_t>(cmd.args.at("x"));
                int16_t y = std::get<int64_t>(cmd.args.at("y"));

                uint16_t pattern = 0;
                if (cmd.args.count("pattern")) {
                    std::string p =
                        std::get<std::string>(cmd.args.at("pattern"));

                    // auto pit = enemyBezier_patterns.find(p);
                    // if (pit != enemyBezier_patterns.end())
                    //     pattern = pit->second;
                }

                uint32_t duration = 5000;
                if (cmd.args.count("duration"))
                    duration =
                        static_cast<uint32_t>(
                            std::get<int64_t>(cmd.args.at("duration")));

                writeU16(code, static_cast<uint16_t>(entityID));
                writeI16(code, x);
                writeI16(code, y);
                writeU16(code, pattern);
                writeU32(code, duration);
            }

            // call
            else if constexpr (std::is_same_v<T, Call>) {
                writeU8(code, 4);
                writeU32(code, static_cast<uint32_t>(cmd.address));
            }

            // repeat
            else if constexpr (std::is_same_v<T, Repeat>) {
                writeU8(code, 6);
                writeU16(code, static_cast<uint16_t>(cmd.count));
            }

            // repeatUntil
            else if constexpr (std::is_same_v<T, RepeatUntil>) {
                writeU8(code, 7);

                auto it = FLAGS_TABLE.find(cmd.condition);
                if (it == FLAGS_TABLE.end())
                    throw std::runtime_error("unknown flag");

                writeU16(code, static_cast<uint16_t>(it->second));
            }

            // end
            else if constexpr (std::is_same_v<T, End>) {
                writeU8(code, 8);
            }

        }, ins);

        return code;
    }


public:

    STGBin(const std::string& path) {
        EVENT_TABLE = readJson(path);
        for (const auto& [key, value]: EVENT_TABLE["flags"].GetObject()) {
            FLAGS_TABLE[key.GetString()] = key.GetInt();
        }
        for (const auto& [key, value]: EVENT_TABLE["flags"].GetObject()) {
            FLAGS_TABLE[key.GetString()] = key.GetInt();
        }

    }

    void writeBinary(
        const std::vector<Instruction>& instructions,
        int entry_pc,
        int endAddr,
        const std::string& filename) {
        std::vector<uint8_t> code;

        // Header
        const char magic[8] = { 'y','9','S','T','G','B','i','n' };
        code.insert(code.end(), magic, magic + 8);

        writeU16(code, 1);                       // version
        writeU32(code, static_cast<uint32_t>(entry_pc));
        writeU32(code, static_cast<uint32_t>(endAddr));

        // Body
        for (auto& ins : instructions) {
            auto chunk = packParam(ins);
            code.insert(code.end(), chunk.begin(), chunk.end());
        }

        std::ofstream ofs(filename + ".dat", std::ios::binary);
        ofs.write(reinterpret_cast<char*>(code.data()), code.size());
    }
};
