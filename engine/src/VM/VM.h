#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>


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
    
    enum opCode {
        shutdown,
        wait,
        waitUntil,
        spawn,
        call,
        ret,
        repeat,
        repeatUntil,
        end
    };


    bool running = true;
    BIN instr;
    uint32_t pc;
    std::vector<uint32_t> callStack, loopStack;

public:
    VM(const std::string& stgdatPath) {
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
    }

    bool step() {
        if (!running) return false;
        ++pc;
        switch (instr[pc]) {
            case shutdown: running = false; break;
            case wait: {
            } break;
            case waitUntil: {
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
};
