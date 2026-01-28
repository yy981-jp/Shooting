#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <limits>

#include "../core/def.h"
#include "../core/util.h"
#include "commands.h"


class VM {
    using BIN = std::vector<uint8_t>;

    // ファイルヘッダ構造の定義
    #pragma pack(push, 1)
    struct FileHeader {
        char magic[8];
        uint16_t version;
        uint32_t entry_pc;
        uint32_t code_size;
    } fh;
    #pragma pack(pop)
    static_assert(sizeof(FileHeader) == 18);
    
    // 命令セット memo/VMInstructionTable.txtを基準とする
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


    // 複数tick処理用の状態保持 - 型定義
    enum class ExecState : uint8_t {
        Running,
        WaitTick,
        WaitFlag
    };

    // 複数tick処理用の状態保持 - 実装
    struct ExecFrame {
        uint32_t pc;
        ExecState state;

        uint32_t waitTick = 0;
        uint16_t waitFlag = 0;
        uint16_t loopCount = 0;
    };

    // repeatとrepeatUntil - 型定義
    enum class LoopType : uint8_t {
        Count,		// repeat N
        Until		// repeatUntil flag
    };
    
    // repeatとrepeatUntil - 実装
    struct LoopFrame {
        LoopFrame(const LoopType& type, const uint32_t& begin_pc, const uint16_t& value)
            : type(type), begin_pc(begin_pc), value(value) {}

        LoopType type;
        uint32_t begin_pc;

        uint16_t value;
    };

    struct OP_spawn_cache {
        uint16_t enemyBezier = UINT16_MAX;
    } entityRTable_Cache;

    // spawn命令だけ長くなりそうなので隔離
    void op_spawn();


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

    constexpr static std::string nullStr = "VM_const-null";

    const rj::Value& eventTable;
    std::unordered_map<uint16_t,std::string> flagsTable;
    
public:
    ExecFrame frame;
    std::unordered_map<std::string,bool> flags; // ゲームフラグ
    GameCommand gamecommand;

    VM(const std::string& stgdatPath);

    enum class ReturnCode: uint8_t {
        error,              // 何らかのエラー
        success,            // 通常終了
        spawnRequest       // spawn命令
    };

    ReturnCode step();

    operator bool() {
        return running;
    }
};
