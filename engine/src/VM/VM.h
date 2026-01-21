#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <limits>

#include "../core/def.h"
#include "../core/util.h"


class VM {
    using BIN = std::vector<uint8_t>;

    #pragma pack(push, 1)
    struct FileHeader {
        char magic[8];
        uint16_t version;
        uint32_t entry_pc;
        uint32_t code_size;
    } fh;
    #pragma pack(pop)
    static_assert(sizeof(FileHeader) == 18);
    
    enum opCode: uint8_t {
        shutdown,
        wait,
        waitUntil,
        spawn,
        call,
        ret,
        repeat,
        repeatUntil,
        end,

        none = UINT8_MAX
    };

    uint16_t read_u16() {
        uint16_t v;
        memcpy(&v, &instr[++pc], sizeof(uint16_t));
        return v;
    }
    uint32_t read_u32() {
        uint32_t v;
        memcpy(&v, &instr[++pc], sizeof(uint32_t));
        return v;
    }


    bool running = true;
    BIN instr;
    uint32_t pc;
    std::vector<uint32_t> callStack, loopStack;
    inline static bool instanced = false; // 2つ以上インスタンス作られるとstatic変数がバグる

    constexpr static std::string nullStr = "VM_const-null";

    uint8_t processing = none;
    const rj::Value& eventTable;
    std::unordered_map<uint16_t,std::string> flagsTable;
    
public:
    std::unordered_map<std::string,bool> flags;

    VM(const std::string& stgdatPath): eventTable(readJson(Assets+"eventTable")) {
        if (instanced) throw std::runtime_error("VM::VM(): already instansed");
        
        std::ifstream ifs(stgdatPath,std::ios::binary|std::ios::ate);
        if (!ifs) throw std::runtime_error("VM::VM(): ifs");
        auto size = ifs.tellg();
        ifs.seekg(0);
        BIN data(size);
        ifs.read(reinterpret_cast<char*>(data.data()), size);

        memcpy(&fh, data.data(),sizeof(FileHeader));

        if (strcmp(fh.magic,"y9STGBin")) throw std::runtime_error("VM::VM(): stgdat.magic mismatch");

        instr.assign(
            data.begin() + sizeof(FileHeader),
            data.end()
        );

        size_t i = 0;
        for (const auto& v: eventTable["flags"].GetArray()) {
            flagsTable[++i] = v.GetString();
            flags[v.GetString()] = false;
        }

        instanced = true;
    }

    bool step() {
        if (!running) return false;
        if (processing == none) processing = instr[++pc];
        switch (processing) {
            case shutdown: running = false; break;
            case wait: {
                static uint32_t remainingTick = 0;
                if (!remainingTick) {
                    remainingTick = read_u32();
                    processing = wait;
                }
                if (!--remainingTick) processing = none;
            } break;
            case waitUntil: {
                static std::string flagID = nullStr;
                if (flagID == nullStr) {
                    flagID = flagsTable[read_u16()];
                    processing = waitUntil;
                }
                if (flags[flagID]) {
                    processing = none;
                    flagID = nullStr;
                }
            } break;
            case spawn: {

            } break;
            case call: {
                
            } break;
            case ret: {
            } break;
            case repeat: {
            } break;
            case repeatUntil: {
            } break;
            case end: {
            } break;
        }
        return true;
    }

    operator bool() {
        return running;
    }
};
