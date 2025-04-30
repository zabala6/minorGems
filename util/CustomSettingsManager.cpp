#include "CustomSettingsManager.h"

CustomSettingsManager::CustomSettingsManager() {
    m_filePaths[SettingFile::Game] = "settings/game.ini";
    m_filePaths[SettingFile::Server] = "settings/server.ini";
    m_filePaths[SettingFile::Editor] = "settings/editor.ini";
}

CustomSettingsManager& CustomSettingsManager::getInstance() {
    static CustomSettingsManager instance;
    return instance;
}

template <typename T>
T CustomSettingsManager::getSetting(SettingFile file, const std::string& section, const std::string& key, T defaultValue, char *outValueFound) {
    std::string filename = getFileName(file);
    std::ifstream ifs(filename);
    if(!ifs.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
    return defaultValue;
    }

    std::string line, currentSection;
    while (std::getline(ifs, line)) {
        trim(line);
        if (line.empty() || line[0] == ';') continue;
        if (line[0] == '[') {
            currentSection = getSectionName(line);
        }
        else {
            auto keyValue = getKeyValue(line);
            if (!currentSection.empty() && keyValue.first != "" && keyValue.second != "") {
                m_settings[currentSection][keyValue.first] = keyValue.second;
                *outValueFound = true;
            }
        }
    }

    ifs.close();

    auto sectionIt = m_settings.find(section);
    if (sectionIt != m_settings.end()) {
        auto keyIt = sectionIt->second.find(key);
        if (keyIt != sectionIt->second.end()) {
            return convertTo<T>(keyIt->second);
        }
    }
    return defaultValue;
}

std::string CustomSettingsManager::getFileName(SettingFile file) {
    auto it = m_filePaths.find(file);
    if (it != m_filePaths.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Invalid SettingFile requested");
    }
}

void CustomSettingsManager::saveToFile(SettingFile file) {
    std::string filename = m_filePaths[file];
    std::ofstream ofs(filename);
    if (!ofs.is_open()){
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
        return;
    }

    for (const auto& section : m_settings){
        ofs << "[" << section.first << "]" << std::endl;
        for (const auto& keyVal : section.second) {
            ofs << keyVal.first << "=" << keyVal.second << std::endl;
        }
        ofs.close();
    }
}

void CustomSettingsManager::trim(std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    str = (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start,end - start + 1);
}

std::string CustomSettingsManager::getSectionName(const std::string& line) {
        return line.substr(1, line.find(']') - 1);
}

std::pair<std::string, std::string> CustomSettingsManager::getKeyValue(const std::string& line) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            trim(key);
            trim(value);
            return {key, value};
        }
        return {"", ""};
}

template <typename T>
void CustomSettingsManager::setSetting(SettingFile file, const std::string& section, const std::string& key, const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        m_settings[section][key] = value;
    }else {
        m_settings[section][key] = std::to_string(value);
    }
    saveToFile(file);
}

template <typename T>
T CustomSettingsManager::convertTo(const std::string& str){
    return T();
}

template <>
int CustomSettingsManager::convertTo<int>(const std::string& str) {
    return std::stoi(str);
}

template <>
float CustomSettingsManager::convertTo<float>(const std::string& str) {
    return std::stof(str);
}

template <>
std::string CustomSettingsManager::convertTo<std::string>(const std::string& str) {
    return str;
}

template int CustomSettingsManager::getSetting<int>(SettingFile file, const std::string& section, const std::string& key, int defaultValue, char* outValueFound);
template float CustomSettingsManager::getSetting<float>(SettingFile file, const std::string& section, const std::string& key, float defaultValue, char* outValueFound);
template std::string CustomSettingsManager::getSetting<std::string>(SettingFile file, const std::string& section, const std::string& key, std::string defaultValue, char* outValueFound);

template void CustomSettingsManager::setSetting<int>(SettingFile file, const std::string& section, const std::string& key, const int& value);
template void CustomSettingsManager::setSetting<float>(SettingFile file, const std::string& section, const std::string& key, const float& value);
template void CustomSettingsManager::setSetting<std::string>(SettingFile file, const std::string& section, const std::string& key, const std::string& value);
