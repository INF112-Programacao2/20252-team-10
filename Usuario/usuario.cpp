#include "../Usuario/usuario.h"
#include <iostream>
#include <iomanip>
using namespace mysqlx;

// Construtor
Usuario::Usuario(std::string nome, std::string email, std::string senha, int nivelAcesso, mysqlx::abi2::r0::Schema* db)
    : id(-1), nome(nome), email(email), senha(senha), nivelAcesso(nivelAcesso), db(db) {}
// ID corresponde a -1 quando o usuário ainda não foi salvo no banco de dados, assim que ao salvar,
// o ID será atualizado com o valor do banco de dados

// Construtor sem parametros
Usuario::Usuario() 
    : id(-1), nome(""), email(""), senha(""), nivelAcesso(0), db(nullptr) {}


// Destrutor
Usuario::~Usuario() {}

// Gets
// Retorna o nome do Usuario
std::string Usuario::getNome() const {
    return nome;
}
// Retorna o nome do Usuario
std::string Usuario::getEmail() const {
    return email;
}
// Retorna o nome do Usuario
int Usuario::getId() const {
    return id;
}
// Retorna o nome do Usuario
int Usuario::getNivelAcesso() const {
    return nivelAcesso;
}
//Retorna a senha do usuario
std::string Usuario::getSenha() const {
    return senha;
}


// Sets
// Atribui o novo nome do usuario
void Usuario::setNome(std::string nome) {
    this->nome = nome;
}
// Atribui o novo e-mail do usuario
void Usuario::setEmail(std::string email) {
    this->email = email;
}

// Atribui o ID do usuario
void Usuario::setId(int id) {
    this->id = id;
}
// Atribui o novo nivel de acesso do usuario
void Usuario::setNivelAcesso(int nivelAcesso) {
    this->nivelAcesso = nivelAcesso;
}
// Atribui a nova senha de acesso do usuario
void Usuario::setSenha(const std::string s) {
    this->senha = s;
}

// Demais métodos
void Usuario::detalharPerfil() {
    std::cout << "================================================\n";
    std::cout << "|                 Detalhes do Usuário          |\n";
    std::cout << "================================================\n";
    std::cout << std::left << std::setw(25) << "| Nome"            << "| " << nome         << "\n";
    std::cout << std::left << std::setw(25) << "| Email"           << "| " << email        << "\n";
    std::cout << std::left << std::setw(25) << "| ID"              << "| " << id           << "\n";
    std::cout << std::left << std::setw(25) << "| Nível de Acesso" << "| " << "(" << nivelAcesso << ")" 
                                                                                << (nivelAcesso==1 ? "Gestor"
                                                                                : nivelAcesso==2 ? "Pos Graduacao"
                                                                                : nivelAcesso == 3 ? "Graduacao" 
                                                                                : "Desconhecido")  << "\n";
    std::cout << "=========================================\n";
}

bool Usuario::fazerLogin(mysqlx::Schema* db, const std::string& email, const std::string& senha, Usuario* usuarioLogado) {
    if (db == nullptr) {
        throw std::invalid_argument("Conexão com o banco de dados não estabelecida.");
    }
    try{
        Table usuarioTable = db->getTable("Usuario");

        RowResult resultado = usuarioTable
            .select("id", "nome", "email", "senha", "nivelAcesso")
            .where("email = :email AND senha = :senha")
            .bind("email", email)
            .bind("senha", senha)
            .execute();
        
        if (resultado.count() == 0) {
            std::cerr << "[ERRO] Usuário não encontrado. Senha e/ou e-mail estão incorretos\n";
            return false;
        }

        Row row = resultado.fetchOne();
        
        usuarioLogado->setId(row[0].get<int>());
        usuarioLogado->setNome(row[1].get<std::string>());
        usuarioLogado->setEmail(row[2].get<std::string>());
        usuarioLogado->setSenha(row[3].get<std::string>());
        usuarioLogado->setNivelAcesso(row[4].get<int>());

        int nivelAcesso = usuarioLogado->getNivelAcesso();

        if(nivelAcesso == 1){
            // Gestor
            Table gestorTable = db->getTable("Gestor");
            RowResult resultadoGestor = gestorTable
                .select("id", "cadastrado_por_gestor_id")
                .where("id = :id")
                .bind("id", usuarioLogado->getId())
                .execute();

            if(resultadoGestor.count() == 0){
                std::cerr << "[AVISO] Gestor não encontrado na tabela Gestor.\n";
            } else {
                std::cout << "Dados do Gestor carregados com sucesso.\n";
            }
        }
        return true;

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro no banco: " << err.what() << std::endl;
    } catch (std::exception &ex) {
        std::cerr << "Erro geral: " << ex.what() << std::endl;
    }
    return false; // Falha no login
}

bool Usuario::validarEmail(const std::string email) {
        if(email.empty()){ // Verifica se o email esta vazio
            throw std::invalid_argument("O email não pode estar vazio.");
        }
        if(email.find('@') == std::string::npos || email.find('.') == std::string::npos) { // Verifica se contem '@' e '.'
            throw std::invalid_argument("Formato de email inválido. Certifique-se de incluir '@' e '.'");
        }
        return true;
}

bool Usuario::validarSenha(const std::string senha) {
        if(senha.length() < 6){ // Verifica se a senha tem pelo menos 6 caracteres
            throw std::invalid_argument("A senha deve ter pelo menos 6 caracteres.");
        }
        return true;
}

