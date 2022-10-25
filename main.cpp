/**
 * @file main.cpp
 * @author Pavel Sushko (psushko@ville.kirkland.qc.ca)
 *                      (psushko@sbsoftware.ca)
 * @brief
 * @version 1.0
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <fstream>
#include <iostream>
#include <filesystem>
#include "include/json.hpp"

using json = nlohmann::json;

/**
 * @brief Reads a json file and create an object from it
 *
 * @param filename - name of the file to read
 * @return json - object created from the file
 */
json read_json_file(const std::string &filename)
{
    std::ifstream file(filename);
    json j;

    file >> j;

    return j;
}

/**
 * @brief Get the directory from which to run the program
 *
 * @param program - program object from which to get the run directory
 * @return std::string
 */
std::string get_run_directory(json program)
{
    std::string runDirectory = program["runDirectory"];

    // If the runDirectory is empty, then use the current directory
    if (runDirectory == "")
    {
        runDirectory = "./";
    }

    return runDirectory;
}

/**
 * @brief Get the absolute executable path object
 *
 * @param programPath - path to the executable
 * @return std::string - absolute path to the executable
 */
std::string get_absolute_executable_path(std::string programPath)
{
    std::string executablePath = std::filesystem::current_path().string() + "/";
    int pos = programPath.find("./");

    // If the program path is relative, then turn it into an absolute path
    if (pos != std::string::npos)
    {
        // If no ../ is found, then we can simply remove the ./ from the path, and prepend the current path
        // to it.
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

            // Remove the last count amount of directories from the path
            for (int i = 0; i < count; i++)
            {
                executablePath.erase(executablePath.find_last_of("/"), executablePath.length());
            }
        }
    }

    return executablePath;
}

/**
 * @brief Get the args object
 *
 * @param program - program object from which to build the command line arguments
 * @return std::string - command line arguments
 */
std::string get_args(json program)
{
    std::string args = "";

    // Iterate through the arguments array, and add them to the args string
    for (auto &arg : program["args"])
    {
        args.append(arg);
        args.append(" ");
    }

    return args;
}

/**
 * @brief Creates a command line string from the program object
 *
 * @param program - program object from which to build the command line string
 * @return std::string - command line string
 */
std::string make_command(json program)
{
    std::string command = "cd ";

    command.append(get_run_directory(program));
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

    command.append(get_absolute_executable_path(program["path"]));
    command.append(" ");
    command.append(get_args(program));

    std::cout << command << std::endl;

    return command;
}

/**
 * @brief Runs programs from a json file
 *
 * @param programs - json object containing the programs to run
 * @return int - exit code
 */
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
    if (!run_programs(read_json_file("programs.json")))
    {
        return 0;
    }

    return 1;
}
