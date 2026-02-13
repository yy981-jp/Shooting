#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <string>
#include <map>

#include "def.h"


namespace rj = rapidjson;

class Json {
public:
    rj::Document doc;

    Json(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs) throw std::runtime_error("Game::loadEntityTable(): ifs");
        rj::IStreamWrapper isw(ifs);
        doc.ParseStream(isw);
    }

    static std::map<std::string, const rj::Value*> items(const rj::Value& obj) {
        std::map<std::string, const rj::Value*> jsonMap;
        for (auto itr = obj.MemberBegin(); itr != obj.MemberEnd(); ++itr) {
            jsonMap[itr->name.GetString()] = &itr->value;
        }
        return jsonMap;
    }
};
