#include <fstream>
#include <iostream>
#include <filesystem>
#include "include/json.hpp"

using json = nlohmann::json;

json read_json_file(const std::string &filename)
{
    std::ifstream file(filename);
    json j;
    file >> j;

    return j;
}

int run_programs(json programs)
{
    for (int i = 0; i < programs.size(); i++)
    {
        if (programs[i]["runDirectory"] == "")
        {
            programs[i]["runDirectory"] = "./";
        }

        std::string command = "cd ";

        command.append(programs[i]["runDirectory"]);
        command.append(" && ");

        std::string programPath = programs[i]["path"];
        std::transform(programPath.begin(), programPath.end(), programPath.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });

        if (programPath.find(".py") != std::string::npos)
        {
            command.append("python ");
        }
        else if (programPath.find(".ps1") != std::string::npos)
        {
            command.append("powershell ");
        }
        else if (programPath.find(".exe") != std::string::npos)
        {
            // Do nothing. The executable can just be run by being called
        }
        else
        {
            std::cout << "Language not supported." << std::endl;
            std::cout << "Supported languages: Python, PowerShell, C++ or any other .exe" << std::endl;
            std::cout << "To add support for a language, simply add an \"else if\" statement with \"command.append({\"programCommand\"})\"." << std::endl;
            std::cout << "To exit the program, press \"Enter\"." << std::endl;

            std::cin.ignore();

            return 1;
        }

        command.append(programs[i]["path"]);
        // command.append(programs[i]["args"]);

        std::cout << "Running \"" << command << "\"" << std::endl;
        system(command.c_str());

        if (programPath.find("../"))
        {
            // Split the directory from the program name
            std::string programDirectory = programPath.substr(0, programPath.find_last_of("/\\"));
            std::string programName = programPath.substr(programPath.find_last_of("/\\") + 1);

            // Get the current directory
            std::string currentDirectory = std::filesystem::current_path();

            // Go up the directory tree the amount of times "../" appears in the program path
            for (int i = 0; i < std::count(programPath.begin(), programPath.end(), '.'); i++)
            {
                currentDirectory = currentDirectory.substr(0, currentDirectory.find_last_of("/\\"));
            }
        }

        return 0;
    }

    return 0;
}

json load_programming_languages(std::string);

int main()
{
    run_programs(read_json_file("programs.json"));

    std::cin.ignore();

    return 0;
}
