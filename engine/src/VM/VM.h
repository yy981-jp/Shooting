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
    
    enum OPCode: uint8_t {
        shutdown,
        wait,
        waitUntil,
        spawn,
        call,
        ret,
        repeat,
        repeatUntil,
        end,
    };


    enum class ExecState : uint8_t {
        Running,
        WaitTick,
        WaitFlag
    };

    struct ExecFrame {
        uint32_t pc;
        ExecState state;

        uint32_t waitTick = 0;
        uint16_t waitFlag = 0;
        uint16_t loopCount = 0;
    } frame;


    enum class LoopType : uint8_t {
        Count,		// repeat N
        Until		// repeatUntil flag
    };

    struct LoopFrame {
        LoopFrame(const LoopType& type, const uint32_t& begin_pc, const uint16_t& value)
            : type(type), begin_pc(begin_pc), value(value) {}

        LoopType type;
        uint32_t begin_pc;

        uint16_t value;
    };


    uint16_t read_u16() {
        uint16_t v;
        memcpy(&v, &instr[pc], sizeof(uint16_t));
        pc += sizeof(uint16_t);
        return v;
    }
    uint32_t read_u32() {
        uint32_t v;
        memcpy(&v, &instr[pc], sizeof(uint32_t));
        pc += sizeof(uint32_t);
        return v;
    }


    bool running = true;
    BIN instr;
    uint32_t pc;
    std::vector<uint32_t> callStack;
    std::vector<LoopFrame> loopStack;
    inline static bool instanced = false; // 2つ以上インスタンス作られるとstatic変数がバグる

    constexpr static std::string nullStr = "VM_const-null";

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

        // 複数tick処理
        switch (frame.state) {
        case ExecState::WaitTick:
            if (--frame.waitTick == 0)
                frame.state = ExecState::Running;
            return true;

        case ExecState::WaitFlag:
            if (flags[flagsTable[frame.waitFlag]])
                frame.state = ExecState::Running;
            return true;

        case ExecState::Running:
            break;
        }
        
        // bin解析
        uint8_t opCode = instr[pc++];
        switch (opCode) {
            case shutdown: running = false; break;
            case wait: {
                frame.waitTick = read_u32();
                frame.state = ExecState::WaitTick;
            } break;
            case waitUntil: {
                frame.waitFlag = read_u16();
                frame.state = ExecState::WaitFlag;
            } break;
            case spawn: {

            } break;
            case call: {
                uint32_t jumpAddr = read_u32();
                callStack.push_back(pc);
                pc = jumpAddr;
            } break;
            case ret: {
                uint32_t jumpAddr = callStack.back();
                callStack.pop_back();
                pc = jumpAddr;
            } break;
            case repeat: {
                uint16_t loopCount = read_u16();
                loopStack.push_back({LoopType::Count, pc, loopCount});
            } break;
            case repeatUntil: {
                uint16_t flagID = read_u16();
                loopStack.push_back({LoopType::Until, pc, flagID});
            } break;
            case end: {
                LoopFrame& lf = loopStack.back();
                switch (lf.type) {
                    case LoopType::Count: {
                        if (!--lf.value) loopStack.pop_back();
                            else pc = lf.begin_pc;
                    } break;
                    case LoopType::Until: {
                        if (flags[flagsTable[lf.value]]) loopStack.pop_back();
                            else pc = lf.begin_pc;
                    } break;
                }
            } break;
        }
        return true;
    }

    operator bool() {
        return running;
    }
};
