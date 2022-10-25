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

std::string get_executable_path(std::string programPath)
{
    std::string executablePath = std::filesystem::current_path().string() + "/";
    int pos = programPath.find("./");

    if (pos != std::string::npos)
    {
        if (programPath.find("../") == std::string::npos)
        {
            programPath.erase(pos, 2);

            executablePath += programPath;
        }
        else
        {
            // Count the amount of "../" in the path
            int count = 0;

            while (programPath.find("../") != std::string::npos)
            {
                programPath.erase(programPath.find("../"), 3);
                count++;
            }

            std::cout << "Count: " << count << std::endl;

            // Remove the last count amount of directories from the path
            for (int i = 0; i < count; i++)
            {
                executablePath.erase(executablePath.find_last_of("/"), executablePath.length());
            }

            std::cout << "Executable path: " << executablePath << std::endl;

            std::cout << "Error: Invalid path" << std::endl;
            exit(1);
        }
    }

    return executablePath;
}

std::string get_args(json program)
{
    std::string args = "";

    for (auto &arg : program["args"])
    {
        args.append(arg);
        args.append(" ");
    }

    return args;
}

std::string make_command(json program)
{
    std::string runDirectory = program["runDirectory"];
    std::string command = "cd ";

    if (runDirectory == "")
    {
        runDirectory = "./";
    }

    command.append(runDirectory);
    command.append(" && ");

    std::string programPath = program["path"];
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

        exit(1);
    }

    command.append(get_executable_path(program["path"]));
    command.append(" ");
    command.append(get_args(program));

    std::cout << command << std::endl;

    return command;
}

int run_programs(json programs)
{
    for (int i = 0; i < programs.size(); i++)
    {
        system(make_command(programs[i]).c_str());
    }

    return 0;
}

int main()
{
    run_programs(read_json_file("programs.json"));

    return 0;
}
