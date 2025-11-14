#ifndef CLASSE_DATABASECONNECTION
#define CLASSE_DATABASECONNECTION

#include <mysql-cppconn/mysqlx/xdevapi.h>
// Precisa instalar o conector C++ da MySQL com suporte ao X DevAPI
// [COMANDO] sudo apt install libmysqlcppconn-dev - conector C++ com MySQL
#include <iostream>

using namespace mysqlx;

class DatabaseConnection {
private:
    // Credenciais do Banco de Dados
    const std::string HOST = "localhost";
    const int PORT = 33060;  // Porta do X Protocol
    const std::string USER = "root";
    const std::string PASSWORD = ""; // Senha
    const std::string SCHEMA = "LabUFV"; // Nome do BD

    // Instancias de conexão ao banco de dados
    Session *session; // ponteiro para a sessão do banco de dados
    Schema *db; // ponteiro para o esquema do banco de dados

public:
    DatabaseConnection(); // Construtor
    ~DatabaseConnection(); // Destrutor
    Session* getSession(); // Retorna a sessão do banco de dados
    Schema* getSchema(); // Retorna o esquema do banco de dados
};

#endif