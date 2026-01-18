#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <variant>


struct INS_spawn {
};
struct INS_wait {
    unsigned int remainingTic = 0;
};

class STGParser {
    std::ifstream ifs;
    std::unordered_map<std::string,size_t> functions;
    std::vector<std::variant<
        INS_wait
    >> instructions;

    void re(size_t lineNumber, const std::string& str) {
        throw std::runtime_error(std::to_string(lineNumber) + "行目: " + str);
    }

    std::vector<std::string> separate(const std::string& str) {
        std::vector<std::string> tokens;
        size_t i = 0;
        const size_t n = str.size();

        while (i < n) {
            // space / tab をスキップ
            while (i < n && (str[i] == ' ' || str[i] == '\t')) ++i;
            if (i >= n) break;

            size_t start = i;

            // 単語部分
            while (i < n && str[i] != ' ' && str[i] != '\t') ++i;
            
            std::string token = str.substr(start, i - start);
            if (token.starts_with("//")) return tokens; // コメントアウト

            tokens.emplace_back(token);
        }
        return tokens;
    }

    void parseFileStruct() {
        std::string line;
        size_t currentLine = 0;
        bool commentout = false;
        bool mainFunctionExist = false;
        size_t reserveNum = 0;
        while (std::getline(ifs,line)) {
            ++currentLine;
            if (line.empty()) continue;
            if (commentout) {
                if (line.contains("*/")) commentout = false;
                continue;
            }
            if (line.contains("/*")) {
                commentout = true;
                continue;
            }

            ++reserveNum;

            if (!line.contains(":")) continue; // 関数ではない行
            auto segs = separate(line);
            if (segs.size() != 1) re(currentLine,"関数宣言の行にトークンが2つ以上存在します");
            std::string funcName = segs[0].substr(1);
            if (functions.contains(funcName)) re(currentLine, funcName + "関数が2つ以上存在します");
            functions[funcName] = currentLine;

            if (funcName=="main") mainFunctionExist = true;
        }
        instructions.reserve(reserveNum);
        rawInstructions.reserve(reserveNum);
    }

    void convert() {
        size_t mainFuncLine = functions["main"];
        size_t currentLine = 0;
        ifs.seekg(std::ios::beg);
        std::string line;
        while (std::getline(ifs,line)) {
            if (++currentLine <= mainFuncLine) continue;
            auto segs = separate(line);
            if (segs.empty()) continue; // 空行
            if (segs[0]=="wait")  {
                if (segs.size()!=2) re(currentLine,"waitコマンドには引数が1つ必要です");
                INS_wait ins;
                ins.remainingTic = std::stoi(segs[1]);
                rawInstructions.emplace_back(ins);
            } else if (segs[0]=="repeat") {
            }
        }
    }

public:
    STGParser(const std::string& path): ifs(path) {
        if (!ifs) throw std::runtime_error("STGParser::STGParser(): ifs");
        
        parseFileStruct();
        convert();
    }
};