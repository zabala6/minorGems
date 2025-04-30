#ifndef CUSTOM_SETTINGS_MANAGER_H
#define CUSTOM_SETTINGS_MANAGER_H

#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept> 

class CustomSettingsManager {
public:
    enum class SettingFile {
        Game,
        Server,
        Editor
    };
    static CustomSettingsManager& getInstance();

    template <typename T>
    T getSetting(SettingFile file, const std::string& section, const std::string& key, T defaultValue, char *outValueFound);

    template <typename T>
    void setSetting(SettingFile file, const std::string& section, const std::string& key, const T& value);
    void saveToFile(SettingFile file);

private:
    CustomSettingsManager();
    CustomSettingsManager(const CustomSettingsManager&) = delete;
    CustomSettingsManager& operator=(const CustomSettingsManager&) = delete;

    void trim(std::string& str);
    std::string getSectionName(const std::string& line);
    std::pair<std::string, std::string> getKeyValue(const std::string& line);
    template <typename T>
    T convertTo(const std::string&str);
    std::string getFileName(SettingFile file);

    std::map<std::string, std::map<std::string, std::string>> m_settings;

    std::map<SettingFile, std::string> m_filePaths;
};

#endif