#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <vector>
#include <regex>

// Extract a string field value from JSON (simple scan, no external lib)
std::string extract_json_string(const std::string& json, const std::string& field) {
    std::string needle = "\"" + field + ":\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            pos++;
            switch (json[pos]) {
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                default:   result += json[pos]; break;
            }
        } else {
            result += json[pos];
        }
        pos++;
    }
    return result;
}

std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

struct DiceSpec {
    int count;
    int sides;
};

// Parse NdM notation from user text (e.g. "3d6", "2d20")
// Falls back to 1d6 for keywords like 掷骰子 / 摇骰子
DiceSpec parse_dice(const std::string& text) {
    std::regex dice_re(R"((\d+)?d(\d+))", std::regex::icase);
    std::smatch m;
    if (std::regex_search(text, m, dice_re)) {
        int count = m[1].str().empty() ? 1 : std::stoi(m[1].str());
        int sides = std::stoi(m[2].str());
        count = std::clamp(count, 1, 100);
        sides = std::clamp(sides, 2, 1000);
        return {count, sides};
    }
    return {1, 6};
}

class DiceRoller {
    std::mt19937 rng_;
public:
    DiceRoller() : rng_(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

    std::vector<int> roll(int count, int sides) {
        std::uniform_int_distribution<int> dist(1, sides);
        std::vector<int> results;
        results.reserve(count);
        for (int i = 0; i < count; ++i)
            results.push_back(dist(rng_));
        return results;
    }

    static int sum(const std::vector<int>& rolls) {
        int total = 0;
        for (int v : rolls) total += v;
        return total;
    }
};

int main() {
    // hook.v1: read only the first line (init frame)
    std::string line;
    if (!std::getline(std::cin, line)) return 0;

    DiceSpec spec = parse_dice(line);

    DiceRoller roller;
    auto rolls = roller.roll(spec.count, spec.sides);
    int total = DiceRoller::sum(rolls);

    std::ostringstream oss;
    oss << spec.count << "d" << spec.sides << ": ";
    for (size_t i = 0; i < rolls.size(); ++i) {
        if (i > 0) oss << " + ";
        oss << rolls[i];
    }
    oss << " = " << total;

    std::cout << "{\"type\":\"output\",\"outputs\":[{\"kind\":\"text\",\"text\":\""
              << json_escape(oss.str()) << "\"}]}" << std::endl;
    std::cout << "{\"type\":\"done\",\"result\":\"ok\"}" << std::endl;
}
