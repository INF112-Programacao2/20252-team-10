#include "../Usuario/usuario.h"
#include "../Gestor/gestor.h"
#include <iostream>
using namespace mysqlx;

// Construtor
Usuario::Usuario(std::string nome, std::string email, std::string senha, int nivelAcesso, mysqlx::abi2::r0::Schema* db)
    : id(-1), nome(nome), email(email), senha(senha), nivelAcesso(nivelAcesso), db(db) {
        cadastradoPor = nullptr;
    }
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
// Retorna o gestor que cadastrou o osuário
Gestor * Usuario::getCadastradoPor() const {
    return cadastradoPor;
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
void Usuario::setCadastradoPor (Gestor * gestor){
    this->cadastradoPor = gestor;
}

// Demais métodos
void Usuario::detalharPerfil() {
    std::cout << "Nome: " << nome << std::endl;
    std::cout << "Email: " << email << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Nível de Acesso: " << nivelAcesso << std::endl;
}

bool Usuario::fazerLogin(Schema* db, const std::string& email, const std::string& senha, Usuario* usuarioLogado) {
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
        return true;

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro no banco: " << err.what() << std::endl;
    } catch (std::exception &ex) {
        std::cerr << "Erro geral: " << ex.what() << std::endl;
    }
    return false; // Falha no login
}

bool Usuario::validarEmail(const std::string& email) {
        if(email.empty()){ // Verifica se o email esta vazio
            throw std::invalid_argument("O email não pode estar vazio.");
        }
        if(email.find('@') == std::string::npos || email.find('.') == std::string::npos) { // Verifica se contem '@' e '.'
            throw std::invalid_argument("Formato de email inválido. Certifique-se de incluir '@' e '.'");
        }
        return true;
}

bool Usuario::validarSenha(const std::string& senha) {
        if(senha.length() < 6){ // Verifica se a senha tem pelo menos 6 caracteres
            throw std::invalid_argument("A senha deve ter pelo menos 6 caracteres.");
        }
        return true;
}

bool Usuario::verificarSenha(std::string senha) {
    return this->senha == senha;
}

void Usuario::alterarSenha(std::string senhaAntiga, std::string senhaNova) {
    if (verificarSenha(senhaAntiga)) {
        this->senha = senhaNova;
        std::cout << "Senha alterada com sucesso!" << std::endl;
    } else {
        std::cout << "Senha antiga incorreta!" << std::endl;
    }
}

//Implementacao do metodo para consultar reagentes no DB.
void Usuario::consultarReagente() {

    std::cout << "\n Consultando Reagentes no Banco de Dados \n";

    // Verificacao de seguranca
    if (db == nullptr) {
        std::cerr << "ERRO: Usuario nao esta conectado ao banco de dados." << std::endl;
        return;
    }

    try {
        // Pega a tabela 'Reagente'
        Table reagenteTable = db->getTable("Reagente");

        // Seleciona as colunas que queremos mostrar
        RowResult res = reagenteTable.select(
            "id", "nome", "quantidade", "unidadeMedida",
            "localArmazenamento", "dataValidade", "nivelAcesso"
        ).execute();

        // Verifica se o banco retornou alguma linha
        if (res.count() == 0) {
            std::cout << "Nenhum reagente cadastrado no sistema." << std::endl;
            return;
        }

        std::cout << "Exibindo reagentes compativeis com seu nivel de acesso ("
                  << this->nivelAcesso << "):\n";

        int reagentesExibidos = 0;

        // analisa sobre todas as linhas (reagentes) que o banco encontrou
        for (Row row : res.fetchAll()) {

            // Pega o nivel de acesso necessario para este reagente
            int nivelDoReagente = row[6].get<int>();

            // Um usuario pode ver um reagente se o seu nivel de acesso
            // for menor ou igual ao nivel do reagente.
            // Ex: Gestor (1) <= Reagente (3) -> (Pode ver)
            // Ex: Grad (3) <= Reagente (1) -> (NAO pode ver)
            if (this->nivelAcesso <= nivelDoReagente) {

                // Se o usuario tem permissao, imprime
                std::cout << "Nome:    " << row[1].get<std::string>()
                          << " (ID: " << row[0].get<int>() << ")\n";
                std::cout << "Qtde:    " << row[2].get<int>()
                          << " " << row[3].get<std::string>() << "\n";
                std::cout << "Local:   " << row[4].get<std::string>() << "\n";
                std::cout << "Validade: " << row[5].get<std::string>() << "\n";
                std::cout << "(Nivel de acesso necessario: " << nivelDoReagente << ")\n";

                reagentesExibidos++;
            }
            // Se o usuario nao tem permissao, o 'if' falha e o reagente e simplesmente ignorado (nao impresso).
        }

        if (reagentesExibidos == 0) {
            std::cout << "Nenhum reagente encontrado para o seu nivel de acesso.\n";
        }

    } catch (const mysqlx::Error &err) {
        // Captura qualquer erro do banco de dados (ex: tabela nao existe)
        std::cerr << "Erro ao consultar reagentes: " << err.what() << std::endl;
    }
}

void Usuario::acessarReagenteRestrito(int idReagente){
   std::cout << "Somente gestores podem acessar reagentes Restritos" << std::endl;
}
