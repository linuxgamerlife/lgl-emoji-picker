#include "core/theme.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <regex>
#include <sstream>

namespace emojipicker::theme {

namespace {

const std::regex commentPattern(R"(/\*[\s\S]*?\*/)");
const std::regex importPattern(
    R"re(@import\s+(?:url\(\s*)?["']?([^"')\s;]+)["']?\s*\)?\s*;)re");

std::optional<std::string> readText(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return std::nullopt;
    std::ostringstream text;
    text << file.rdbuf();
    return std::regex_replace(text.str(), commentPattern, "");
}

} // namespace

std::vector<std::string> gtkCssPaths(const std::string &configDir)
{
    namespace fs = std::filesystem;
    std::vector<std::string> found;
    std::function<void(const fs::path &)> visit = [&](const fs::path &path) {
        std::error_code error;
        const std::string key = path.string();
        if (std::find(found.begin(), found.end(), key) != found.end()
            || !fs::is_regular_file(path, error))
            return;
        found.push_back(key);
        const auto text = readText(key);
        if (!text)
            return;
        for (auto it = std::sregex_iterator(text->begin(), text->end(), importPattern);
             it != std::sregex_iterator(); ++it) {
            std::string target = (*it)[1].str();
            const bool local = target.find("://") == std::string::npos
                || target.rfind("file://", 0) == 0;
            if (!local)
                continue;
            if (target.rfind("file://", 0) == 0)
                target.erase(0, 7);
            fs::path resolved = fs::weakly_canonical(path.parent_path() / target, error);
            if (error)
                resolved = (path.parent_path() / target).lexically_normal();
            visit(resolved);
        }
    };
    visit(fs::path(configDir) / "gtk-4.0" / "gtk.css");
    return found;
}

} // namespace emojipicker::theme
