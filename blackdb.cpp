#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <cstdlib>
#include <ctime>

// Função para gerar uma senha segura aleatória
std::string generateSecurePassword(size_t length = 16) {
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_=+[]{}|;:'\",.<>?/`~";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string password;
    for (size_t i = 0; i < length; ++i) {
        password += characters[dis(gen)];
    }
    return password;
}

// Função para salvar a senha no arquivo do PostgreSQL
void savePostgresPassword(const std::string& password, const std::string& dbUser, const std::string& dbName) {
    // Caminho do arquivo de configuração do PostgreSQL (ajuste conforme necessário)
    const std::string pgConfFile = "/etc/postgresql/pg_hba.conf"; 

    std::ofstream file(pgConfFile, std::ios::app);  // Adiciona ao final do arquivo
    if (file.is_open()) {
        file << "\n# Generated for blackdb\n";
        file << "password = " << password << "\n";
        file.close();
        std::cout << "Password saved to PostgreSQL configuration file." << std::endl;
    } else {
        std::cerr << "Error opening PostgreSQL configuration file." << std::endl;
    }
}

// Função para salvar a senha em um arquivo de configuração do blackdb
void saveBlackdbConfig(const std::string& password, const std::string& dbUser, const std::string& dbName) {
    // Caminho para o arquivo de configuração específico do blackdb
    const std::string dbConfigFile = "blackdb_config.ini";

    std::ofstream file(dbConfigFile);
    if (file.is_open()) {
        file << "[database]\n";
        file << "user = " << dbUser << "\n";
        file << "password = " << password << "\n";
        file << "dbname = " << dbName << "\n";
        file.close();
        std::cout << "Password saved to blackdb configuration file." << std::endl;
    } else {
        std::cerr << "Error opening blackdb configuration file." << std::endl;
    }
}

int main() {
    const std::string dbUser = "blackarmy_user";  // Nome do usuário do banco de dados
    const std::string dbName = "blackarmy";  // Nome do banco de dados

    // Gerar uma senha segura de 16 caracteres
    std::string password = generateSecurePassword(16);

    // Salvar a senha nos arquivos de configuração
    savePostgresPassword(password, dbUser, dbName);
    saveBlackdbConfig(password, dbUser, dbName);

    return 0;
}
