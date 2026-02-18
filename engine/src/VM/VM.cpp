#include "VM.h"
#include "../tables/all.h"


    VM::VM(const std::string& stgdatPath): eventTableDoc(readJson(Assets+"eventTable.json")), eventTable(eventTableDoc), pc(0) {
        std::ifstream ifs(stgdatPath,std::ios::binary|std::ios::ate);
        if (!ifs) throw std::runtime_error("VM::VM(): ifs");
        auto size = ifs.tellg();
        ifs.seekg(0);
        BIN data(size);
        ifs.read(reinterpret_cast<char*>(data.data()), size);

        memcpy(&fh, data.data(),sizeof(FileHeader));

        if (fh.magic == fileMagicNumber) throw std::runtime_error("VM::VM(): stgdat.magic mismatch");

        instr.assign(
            data.begin() + sizeof(FileHeader),
            data.end()
        );

        size_t i = 0;
        for (const auto& v: eventTable["flags"].GetArray()) {
            flagsTable[++i] = v.GetString();
            flags[v.GetString()] = false;
        }
    }



    VM::ReturnCode VM::step() {
        if (!running) return ReturnCode::finished;

        // 複数tick処理
        switch (frame.state) {
        case ExecState::WaitTick:
            if (!--frame.waitTick)
                frame.state = ExecState::Running;
            return ReturnCode::success;

        case ExecState::WaitFlag:
            if (flags[flagsTable[frame.waitFlag]])
                frame.state = ExecState::Running;
            return ReturnCode::success;

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
                op_spawn();
                return ReturnCode::spawnRequest;
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
        return ReturnCode::success;
    }



    void VM::op_spawn() {
        uint16_t entityType = read_u16();
        switch (static_cast<EntityType>(entityType)) {
            using enum EntityType;
            case enemyBezier: {
                cmd::enemyBezier c;
                c.x = read_s16();
                c.y = read_s16();
                c.pattern = read_u16();
                c.duration = read_u32();
                gamecommand = std::move(c);
            } break;
            default: throw std::runtime_error("VM::op_spawn(): entityType");
        }
    }
