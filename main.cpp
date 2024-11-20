#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <vector>
#include <commands.hpp>
#include <ascii_art.hpp>

void show_prompt(){
    std::cout << "BlackArmy> ";
}

std::vector<std::string> parse_arguments(const std::string& command) {
    std::vector<std::string> arguments;
    std::stringstream ss(command);
    std::string arg;
    while (ss >> arg) {
        arguments.push_back(arg);
    }
    return arguments;
}

int main(){
    std::unordered_map<std::string, std::function<void()>> commands;
    commands["help"] = show_help;
    commands["clear"] = clear_screen;
    commands["exit"] = exit_program;

    std::string command;
    print_ascii_art();

    while (true){
        show_prompt();
        if (!std::getline(std::cin, command)){
            std::cout << std::endl;
            exit_program();
            break;
        }

        std::vector<std::string> args = parse_arguments(command);

        if (args.empty()) {
            continue;
        }

        if (args[0] == "discover" && args.size() >= 4) {
            std::string domain = args[1];
            std::string wordlist = args[3];
            discover(domain, wordlist);
        } else if (commands.find(args[0]) != commands.end()) {
            commands[args[0]]();
            if (args[0] == "exit") {
                break;
            }
        } else if (!command.empty()) {
            std::system(command.c_str());
        }
    }

    return 0;
}
