#include "vm.h"
#include "../tables/all.h"
#include "stgEntityTable.h"


VM::VM(const std::string& stgdatPath): eventTableDoc(readJson(Assets+"eventTable.json")), eventTable(eventTableDoc), pc(0) {
    std::ifstream ifs(stgdatPath,std::ios::binary|std::ios::ate);
    if (!ifs) throw std::runtime_error("VM::VM(): ifs");
    auto size = ifs.tellg();
    ifs.seekg(0);
    BIN data(size);
    ifs.read(reinterpret_cast<char*>(data.data()), size);

    memcpy(&fh, data.data(),sizeof(FileHeader));

    if (fh.magic != fileMagicNumber) throw std::runtime_error("VM::VM(): stgdat.magic mismatch");

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



void VM::step(GCMS& gcm) {
    if (!running) return;

    // 複数tick処理
    switch (frame.state) {
    case ExecState::WaitTick:
        if (!--frame.waitTick)
            frame.state = ExecState::Running;
        return;

    case ExecState::WaitFlag:
        if (flags[flagsTable[frame.waitFlag]])
            frame.state = ExecState::Running;
        return;

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
            op_spawn(gcm);
            return;
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
}



void VM::op_spawn(GCMS& gcm) {
	uint16_t entityType = read_u16();

	switch(static_cast<STGEntityID>(entityType)) {

#define ENTITY(name, fields) \
case STGEntityID::name: { \
	auto raw = readStruct<STGEntity::name>(); \
	cmd::name c{}; \
	fields \
	gcm(c); \
	break; \
}

#define FIELD_CHOOSER(_1,_2,_3,NAME,...) NAME
#define FIELD(...) FIELD_CHOOSER(__VA_ARGS__, FIELD3, FIELD2)(__VA_ARGS__)

#define FIELD2(type, name) c.name = raw.name;
#define FIELD3(type, name, def) c.name = raw.name;

#include "../../../shared/stgEntity.def"

#undef ENTITY
#undef FIELD
#undef FIELD2
#undef FIELD3
#undef FIELD_CHOOSER

	default:
		throw std::runtime_error("VM::op_spawn()");
	}
}
