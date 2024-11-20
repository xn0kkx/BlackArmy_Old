#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>

void discover(const std::string& domain, const std::string& wordlist) {
    std::string outputFile = "/var/tmp/" + domain + ".txt";
    std::string command = "gobuster dns -d " + domain + " -w " + wordlist + " > /var/tmp/raw_output.txt";

    std::cout << "Running DNS enum: " << command << std::endl;
    std::system(command.c_str());

    std::ifstream rawOutput("/var/tmp/raw_output.txt");
    if (!rawOutput.is_open()) {
        std::cerr << "Error: Could not open raw output file." << std::endl;
        return;
    }

    std::ofstream finalOutput(outputFile);
    if (!finalOutput.is_open()) {
        std::cerr << "Error: Could not open final output file: " << outputFile << std::endl;
        return;
    }

    std::vector<std::string> subdomains;
    std::string line;
    while (std::getline(rawOutput, line)) {
        size_t foundPos = line.find("Found:");
        if (foundPos != std::string::npos) {
            std::string subdomain = line.substr(foundPos + 7);
            subdomain.erase(subdomain.find_last_not_of(" \n\r\t") + 1);
            subdomains.push_back(subdomain);
            std::cout << "Discovered subdomain: " << subdomain << std::endl;
        }
    }

    rawOutput.close();
    std::remove("/var/tmp/raw_output.txt");

    for (const auto& subdomain : subdomains) {
        std::string nslookupCommand = "nslookup " + subdomain + " > /var/tmp/nslookup_output.txt";
        std::system(nslookupCommand.c_str());

        std::ifstream nslookupOutput("/var/tmp/nslookup_output.txt");
        if (!nslookupOutput.is_open()) {
            std::cerr << "Error: Could not open nslookup output file for " << subdomain << std::endl;
            continue;
        }

        bool inNonAuthSection = false;
        std::vector<std::string> ips;
        while (std::getline(nslookupOutput, line)) {
            if (line.find("Non-authoritative answer:") != std::string::npos) {
                inNonAuthSection = true;
            } else if (inNonAuthSection && line.find("Address:") != std::string::npos) {
                size_t pos = line.find("Address:");
                if (pos != std::string::npos) {
                    std::string ip = line.substr(pos + 8);
                    ip.erase(ip.find_last_not_of(" \n\r\t") + 1);
                    ips.push_back(ip);
                }
            } else if (inNonAuthSection && line.empty()) {
                break;
            }
        }

        nslookupOutput.close();
        std::remove("/var/tmp/nslookup_output.txt");

        finalOutput << subdomain;
        if (!ips.empty()) {
            finalOutput << "  ";
            for (size_t i = 0; i < ips.size(); ++i) {
                finalOutput << ips[i];
                if (i < ips.size() - 1) {
                    finalOutput << ", ";
                }
            }
        }
        finalOutput << std::endl;
    }

    finalOutput.close();
    std::cout << "DNS enumeration and IP resolution complete. Results saved to " << outputFile << std::endl;
}