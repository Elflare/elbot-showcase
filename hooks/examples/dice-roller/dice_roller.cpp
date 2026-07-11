#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

std::string json_string(const std::string& json, const std::string& field) {
    const std::string needle = "\"" + field + "\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos = json.find(':', pos + needle.size());
    if (pos == std::string::npos) return "";
    pos = json.find('"', pos + 1);
    if (pos == std::string::npos) return "";

    std::string result;
    for (++pos; pos < json.size() && json[pos] != '"'; ++pos) {
        if (json[pos] == '\\' && ++pos < json.size()) {
            switch (json[pos]) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += json[pos];
            }
        } else {
            result += json[pos];
        }
    }
    return result;
}

std::string json_escape(const std::string& text) {
    std::string escaped;
    for (char c : text) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c;
        }
    }
    return escaped;
}

struct DiceSpec {
    int count;
    int sides;
};

DiceSpec parse_dice(const std::string& text) {
    std::regex dice_re(R"((\d+)?d(\d+))", std::regex::icase);
    std::smatch match;
    if (!std::regex_search(text, match, dice_re)) return {1, 6};
    const int count = std::clamp(match[1].str().empty() ? 1 : std::stoi(match[1].str()), 1, 100);
    const int sides = std::clamp(std::stoi(match[2].str()), 2, 1000);
    return {count, sides};
}

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        const std::string id = json_string(line, "id");
        const std::string method = json_string(line, "method");

        if (method == "system.init") {
            std::cout << "{\"type\":\"response\",\"id\":\"" << json_escape(id)
                      << "\",\"ok\":true,\"result\":{}}" << std::endl;
            continue;
        }
        if (method == "system.shutdown") {
            std::cout << "{\"type\":\"response\",\"id\":\"" << json_escape(id)
                      << "\",\"ok\":true,\"result\":{}}" << std::endl;
            return 0;
        }
        if (method != "event.handle") {
            std::cout << "{\"type\":\"response\",\"id\":\"" << json_escape(id)
                      << "\",\"ok\":false,\"error\":\"unsupported method\"}" << std::endl;
            return 0;
        }

        const DiceSpec spec = parse_dice(json_string(line, "intent_text"));
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> roll(1, spec.sides);
        std::vector<int> results;
        int total = 0;
        for (int i = 0; i < spec.count; ++i) {
            const int value = roll(rng);
            results.push_back(value);
            total += value;
        }

        std::ostringstream text;
        text << spec.count << "d" << spec.sides << ": ";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) text << " + ";
            text << results[i];
        }
        text << " = " << total;

        std::cout << "{\"type\":\"response\",\"id\":\"" << json_escape(id)
                  << "\",\"ok\":true,\"result\":{\"status\":\"completed\",\"outputs\":[{\"kind\":\"text\",\"text\":\""
                  << json_escape(text.str()) << "\"}]}}" << std::endl;
        return 0;
    }
    return 0;
}
