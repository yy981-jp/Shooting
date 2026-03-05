#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

#include "util.h"
#include "def.h"
#include "instruction.h"

void syntaxError(const std::string& str) {
    throw std::runtime_error("SyntaxError: " + str);
}


using namespace stg;
class STGCompiler {
public:
    STGCompiler(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs) throw std::runtime_error("file open failed");

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string raw = removeComments(buffer.str());

        std::stringstream ss(raw);
        std::string line;
        while (std::getline(ss, line))
            lines.push_back(line);

        pc = 0;
    }

    std::tuple<std::vector<Instruction>, int, int> compile() {
        std::vector<Instruction> instructions;

        collectLabels();

        for (auto& [label, _] : labels) {
            auto chunk = compileLabel(label);
            instructions.insert(instructions.end(), chunk.begin(), chunk.end());
        }

        return { instructions, labels.at("main"), pc };
    }

private:
    std::vector<std::string> lines;
    std::unordered_map<std::string, int> labels;
    int pc;

    // =========================
    // 1パス目：ラベル収集
    // =========================
    void collectLabels()
    {
        std::string current;

        for (auto& raw : lines) {
            std::string line = trim(raw);
            if (line.empty() || startsWith(line, "//"))
                continue;

            if (startsWith(line, ":")) {
                current = line.substr(1);
                if (labels.count(current))
                    throw std::runtime_error("duplicate label: " + current);

                labels[current] = -1;
            }
        }

        if (!labels.count("main"))
            throw std::runtime_error("main label not found");
    }

    // =========================
    // ラベル単位コンパイル
    // =========================
    std::vector<Instruction> compileLabel(const std::string& name) {
        labels[name] = pc;

        std::vector<Instruction> instructions;
        std::vector<int> loopStack;

        size_t pos = 0;
        while (pos < lines.size()) {
            if (trim(lines[pos]) == ":" + name) {
                pos++;
                break;
            }
            pos++;
        }

        while (pos < lines.size()) {
            std::string line = trim(lines[pos++]);

            if (startsWith(line, ":"))
                break;

            if (line.empty())
                continue;

            auto tokens = split(line);
            std::string cmd = tokens[0];

            pc++;

            if (cmd == "wait") {
                instructions.emplace_back("wait",
                    parseSecond(tokens[1]));
            } else if (cmd == "waitUntil") {
                instructions.emplace_back("waitUntil", tokens[1]);
            } else if (cmd == "spawn") {
                ArgMap args = parseKV(
                    std::vector<std::string>(tokens.begin()+2, tokens.end())
                );

                args["entityType"] = tokens[1];

                if (tokens[1] == "enemyBezier")
                {
                    args["duration"] =
                        parseSecond(std::get<std::string>(args["duration"]));
                }

                instructions.emplace_back("spawn", args);
            } else if (cmd == "call") {
                instructions.emplace_back("call",
                    labels.at(tokens[1]));
            } else if (cmd == "repeat") {
                instructions.emplace_back("repeat",
                    std::stoi(tokens[1]));

                if (loopStack.size() >= MAX_NEST)
                    throw std::runtime_error("nest too deep");

                loopStack.push_back(pc);
            } else if (cmd == "repeatUntil") {
                instructions.emplace_back("repeatUntil",
                    tokens[1]);

                if (loopStack.size() >= MAX_NEST)
                    throw std::runtime_error("nest too deep");

                loopStack.push_back(pc);
            } else if (cmd == "end") {
                if (loopStack.empty())
                    throw std::runtime_error("end without repeat");

                loopStack.pop_back();
                instructions.emplace_back("end");
            } else {
                throw std::runtime_error("unknown command: " + cmd);
            }
        }

        if (!loopStack.empty())
            throw std::runtime_error("repeat missing end");

        if (name == "main")
            instructions.emplace_back("shutdown");

        return instructions;
    }

    // =========================
    // ユーティリティ
    // =========================

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    static bool startsWith(const std::string& s, const std::string& prefix) {
        return s.rfind(prefix, 0) == 0;
    }

    static std::vector<std::string> split(const std::string& line) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> result;

        while (ss >> token)
            result.push_back(token);

        return result;
    }

    static Value parseValue(const std::string& value) {
        std::string v = value;

        if (v == "true") return true;
        if (v == "false") return false;

        // int
        {
            int i;
            auto [ptr, ec] =
                std::from_chars(v.data(), v.data()+v.size(), i);
            if (ec == std::errc() && ptr == v.data()+v.size())
                return i;
        }

        // double
        {
            try {
                size_t pos;
                double d = std::stod(v, &pos);
                if (pos == v.size())
                    return d;
            } catch (...) {}
        }

        return v;
    }

    static ArgMap parseKV(const std::vector<std::string>& items) {
        ArgMap result;

        for (auto& item : items)
        {
            auto pos = item.find('=');
            std::string key = item.substr(0, pos);
            std::string value = item.substr(pos+1);

            result[key] = parseValue(value);
        }

        return result;
    }

    static int parseSecond(const std::string& value) {
        if (value.ends_with("ms"))
            return std::stoi(value.substr(0, value.size()-2));

        if (value.ends_with("s"))
            return std::stoi(value.substr(0, value.size()-1)) * 1000;

        return std::stoi(value);
    }

    static std::string removeComments(const std::string& text) {
        std::string result;
        bool inBlock = false;

        for (size_t i=0; i<text.size(); ) {
            if (inBlock) {
                if (text.substr(i,2)=="*/")
                {
                    inBlock=false; i+=2;
                }
                else i++;
                continue;
            }

            if (text.substr(i,2)=="//") {
                while (i<text.size() && text[i]!='\n') i++;
                continue;
            }

            if (text.substr(i,2)=="/*") {
                inBlock=true; i+=2;
                continue;
            }

            result += text[i++];
        }

        if (inBlock)
            throw std::runtime_error("block comment not closed");

        return result;
    }
};