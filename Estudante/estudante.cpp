#include "estudante.h"
#include "../Laboratorio/Laboratorio.h"
#include <iostream>
#include <vector>
#include <iomanip>
using namespace mysqlx;

// Construtor
Estudante::Estudante(std::string nome, std::string email, std::string senha, int nivelAcesso,
                    mysqlx::abi2::r0::Schema* db, std::string matricula, std::string curso, std::string nivel):
    Usuario(nome, email, senha, nivelAcesso, db), matricula(matricula), curso(curso), nivel(nivel) {}

// Destrutor
Estudante::~Estudante() {}

// Gets
std::string Estudante::getMatricula(){
    return matricula;
}
std::string Estudante::getCurso(){
    return curso;
}
std::string Estudante::getNivel(){
    return nivel;
}

// Sets
void Estudante::setMatricula(std::string matricula){
    this->matricula = matricula;
}
void Estudante::setCurso(std::string curso){
    this->curso = curso;
}
void Estudante::setNivel(std::string nivel){
    this->nivel = nivel;
}

//Outros métodos
//Adicionar laboratorio a estudante
void Estudante::adicionarLaboratorio(Laboratorio * laboratorio){
    // Verifica se o laboratorio já esta associado no objeto
    for (int i = 0; i < this->laboratorios.size(); i++) {
        if (laboratorios[i]->getId() == laboratorio->getId()) { // Verifica se o ID é igual, se for retorna
            std::cout << getNome() <<" já possui o " << laboratorio->getNome() << ".\n";
            return;
        }
        //Caso não tenha, adiciona na lista
        this->laboratorios.push_back(laboratorio);
        std::cout << getNome() << " está associado ao " << laboratorio->getNome() << ".\n";
    }
    return;
}
// Remove o objeto no laboratorio no objeto estudante
void Estudante::removerLaboratorioObjeto(Laboratorio* laboratorio) {
    std::vector<Laboratorio*> novoLaboratorios;  // vetor auxiliar para receber os laboratorios associados
    bool removido = false; // flag para informar que foi removido
    for (int i = 0; i < laboratorios.size(); i++) {
        if (laboratorios[i]->getId() != laboratorio->getId()) { // se o id dos laboratorios forem diferentes , copia no novo vetor
            novoLaboratorios.push_back(laboratorios[i]);
        } else { // se for igual, atualiza o flag que foi removido
            removido = true;
        }
        if (removido) {
        // Substitui o vetor antigo pelo novo
        this->laboratorios.swap(novoLaboratorios);
        std::cout << getNome() << " desassociado do " << laboratorio->getNome() << std::endl;
        } else {
            std::cout << "Aviso: Associação com " << laboratorio->getNome() << " não encontrada em memória." << std::endl;
        }
    }
}
// Remover laboratorio do estduante
void Estudante::removerLaboratorio(Laboratorio* laboratorio, Schema* db) {
    // Verifica se o ponteiro do laboratorio e db são inválidos
    if (!laboratorio || !db) {
        std::cerr << "[Erro] Parametros do laboratorio são nulos." << std::endl;
        return;
    }
    // Acessa a tabela associado
    try {
        Table associadoTable = db->getTable("Associado");

        // Verificar se a associação existe (apenas a existência, sem se importar com o papel)
        RowResult associadoResult = associadoTable
            .select("papel")
            .where("estudante_id = :estudanteId AND laboratorio_id = :laboratorioId")
            .bind("estudanteId", this->getId())
            .bind("laboratorioId", laboratorio->getId())
            .execute();

        if (associadoResult.count() == 0) {
            std::cout << "Aviso: Associação com " << laboratorio->getNome() << " não encontrada" << std::endl;
            return;
        }

        // Remoção no Banco de Dados (Tabela Associado)
        // Condição de remoção: estudante_id E laboratorio_id
        int dadosAfetados = associadoTable.remove()
            .where("estudante_id = :estudanteId AND laboratorio_id = :laboratorioId")
            .bind("estudanteId", this->getId())
            .bind("laboratorioId", laboratorio->getId())
            .execute()
            .getAffectedItemsCount(); // Coleta o número de linhas afetadas com o delete

        if (dadosAfetados > 0) {
            std::cout << "Associação(ões) removida(s) com sucesso! (" << dadosAfetados << " registro(s))" << std::endl;
            // Remoção na memória
            this->removerLaboratorioObjeto(laboratorio);
        } else {
            //Apenas para tratamento de erro
            std::cerr << "Erro: Associação encontrada, mas nenhuma linha foi removida." << std::endl;
        }
        } catch (const mysqlx::Error &err) {
            std::cerr << "Erro MySQL ao remover laboratório: " << err.what() << std::endl;
        } catch (const std::exception &ex) {
            std::cerr << "Erro geral ao remover laboratório: " << ex.what() << std::endl;
        }
}
// Associa estudante ao laboratorio
void Estudante::associarLaboratorio(Laboratorio* laboratorio, const std::string& papel, Schema* db) {
    // Verifica se o ponteiro do laboratorio e db são inválidos
    if (!laboratorio || !db) {
        std::cerr << "[Erro] Paramêtros do laboratorio são nulos." << std::endl;
        return;
    }   //Acessa a tabela associado para associar estudante ao laboratorio 
        //Tal tabela representa as possibilidades de um estudante esta associado a varios laboratorios
        //E o laboratório poder ter varios estudantes
    try{
        Table associadoTable = db->getTable("Associado");
        // Verifica se o associacao ja existe o usuaroio com o mesmo papel
        RowResult associadoResult = associadoTable
                                    .select("papel") //  seleciona o papel
                                    .where("estudante_id = :estudanteId AND laboratorio_id = :laboratorioId AND papel = :papel") // mostrar quando o dado sao os dado pelo metodo
                                    .bind("estudanteId", this->getId()) // pega o id do estudante
                                    .bind("laboratorioId", laboratorio->getId()) // pega o id do laboratorio
                                    .bind("papel", papel) // pega o papel do estudante
                                    .execute(); // faz a consulta

            if (associadoResult.count() > 0) { // se a consulta resultar for maior que 0
                // Recebe o papel existente  com a consulta e faz um casting para o tipo de dado do c++
                std::string papelExistente = associadoResult.fetchOne()[0].get<std::string>();
                if (papelExistente == papel) { // Se o papael for igual, já diz que o usuario já possui o papel com base no bd
                    std::cout << "Aviso: O estudante " << this->getNome() << " já está associado ao laboratório "
                            << laboratorio->getNome() << " com o papel '" << papel << "'. Nenhuma alteração necessária." << std::endl;
                    return;
                }
                // caso seja diferente, atualiza no bd essa associação
                associadoTable.update()
                                .set("papel", papel)
                                .where("estudante_id = :estudanteId AND laboratorio_id = :laboratorioId")
                                .bind("estudanteId", this->getId())
                                .bind("laboratorioId", laboratorio->getId())
                                .execute();
                // Informa que o papel foi atualizado
                std::cout << "Associação atualizada! O estudante " << this->getNome() << " agora tem o papel '"
                        << papel << "' no laboratório " << laboratorio->getNome() << "." << std::endl;
                // Para fins de segurança, associamos o laboratorio no estudnate
                this->adicionarLaboratorio(laboratorio);
                return;
            }
            // Faz a inserção no banco de dados, caso nao tenha no bd
            associadoTable.insert("estudante_id", "laboratorio_id", "papel")
            .values(this->getId(), laboratorio->getId(), papel)
            .execute();
            // informa a associação
            std::cout << "Associação registrada! \n" << std::endl;
            // Aloca o laboratorio no objeto estudante
            this->adicionarLaboratorio(laboratorio);
        } catch (mysqlx::Error &err) {
        std::cerr << "Erro MySQL ao associar laboratório: " << err.what() << std::endl;
        } catch (std::exception &ex) {
        std::cerr << "Erro geral ao associar laboratório: " << ex.what() << std::endl;
        }
}

// Esta função checa o nivel de acesso antes de exibir.
void Estudante::acessarReagenteRestrito(int idReagente) {

    std::cout << "\n(Estudante) Tentando acessar Reagente ID: " << idReagente << "\n";
    if (db == nullptr) {
        std::cerr << "ERRO: Estudante não está conectado ao banco." << std::endl;
        return;
    }

    try {
        Table reagenteTable = db->getTable("Reagente");

        // Busca o reagente específico pelo ID
        RowResult res = reagenteTable.select(
            "id", "nome", "quantidade", "unidadeMedida",
            "localArmazenamento", "dataValidade", "nivelAcesso"
        ).where("id = :id").bind("id", idReagente).execute();

        if (res.count() == 0) {
            std::cout << "Reagente com ID " << idReagente << " não encontrado." << std::endl;
            return;
        }

        Row row = res.fetchOne();
        int nivelDoReagente = row[6].get<int>(); // Nivel de segurança do reagente

        // Checa a permissão: Nivel do Estudante (this->nivelAcesso) deve ser menor ou igual ao nivel do reagente
        if (this->getNivelAcesso() <= nivelDoReagente) {
            // Se tem permissão, exibe os dados
            std::cout << "Acesso Permitido (Nível " << this->getNivelAcesso() << "):\n";
            std::cout << "Nome:    " << row[1].get<std::string>() << "\n";
            std::cout << "Qtde:    " << row[2].get<int>() << " " << row[3].get<std::string>() << "\n";
            std::cout << "Local:   " << row[4].get<std::string>() << "\n";
            std::cout << "Validade: " << row[5].get<std::string>() << "\n";
        } else {
            // Se não tem permissao, nega o acesso
            std::cout << "ACESSO NEGADO: Este reagente requer Nível "
                      << nivelDoReagente << ".\n";
            std::cout << "(Seu nível é " << this->getNivelAcesso() << ").\n";
        }
        std::cout << std::endl;

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao acessar reagente: " << err.what() << std::endl;
    }
}

// Lista os laboratórios associados ao estudante
void Estudante::acessarLaboratorios() {
    if (laboratorios.empty()) {
        std::cout << "\nVocê não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n Seus Laboratórios \n";
    for (const auto& lab : laboratorios) {
        std::cout << "- ID: " << lab->getId() 
                  << " | Nome: " << lab->getNome() 
                  << " | Dep: " << lab->getDepartamento() << "\n";
    }
    std::cout << "\n";
}

// Lista reagentes de todos os laboratórios do estudante
void Estudante::consultarEstoque() {
    if (this->laboratorios.empty()) {
        std::cout << "Você não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n Consulta de Estoque \n";
    
    for (Laboratorio* lab : this->laboratorios) {
        std::cout << "\n>> Laboratório: " << lab->getNome() << "\n";
        
        // Usa a função do laboratório para pegar a lista completa
        std::vector<Reagente*> lista = lab->listarReagentes("");

        if (lista.empty()) {
            std::cout << "   (Nenhum reagente cadastrado)\n";
            continue;
        }

        std::cout << std::left 
                  << std::setw(25) << "Nome" 
                  << std::setw(15) << "Qtd" 
                  << std::setw(15) << "Nível" << "\n";
        std::cout << "\n";

        for (Reagente* r : lista) {
            // Define a string de nível
            std::string nivelStr;
            if(r->getNivelAcesso() == 1) nivelStr = "Restrito";
            else if(r->getNivelAcesso() == 2) nivelStr = "Livre";
            else nivelStr = "Pós-Grad";

            std::cout << std::left 
                      << std::setw(25) << r->getNome()
                      << std::setw(15) << (std::to_string(r->getQuantidade()) + " " + r->getUnidadeMedida())
                      << std::setw(15) << nivelStr << "\n";
        }
    }
    std::cout << "\n";
}

// Realiza a retirada de um reagente
void Estudante::retirarReagente() {
    if (laboratorios.empty()) {
        std::cout << "Você precisa estar associado a um laboratório para retirar reagentes.\n";
        return;
    }

    //Escolher o Laboratório
    std::cout << "\n--- Retirada de Reagente ---\n";
    std::cout << "Escolha o laboratório:\n";
    for (size_t i = 0; i < laboratorios.size(); i++) {
        std::cout << i + 1 << ". " << laboratorios[i]->getNome() << "\n";
    }
    std::cout << "Opção: ";
    int opLab;
    std::cin >> opLab;

    if (opLab < 1 || opLab > (int)laboratorios.size()) {
        std::cout << "Opção inválida.\n";
        return;
    }
    Laboratorio* labEscolhido = laboratorios[opLab - 1];

    // Escolher o Reagente (Busca por nome)
    std::cout << "Digite o nome do reagente: ";
    std::string nomeReagente;
    std::cin.ignore();
    std::getline(std::cin, nomeReagente);

    // Escolher a Quantidade
    std::cout << "Digite a quantidade a retirar: ";
    float qtd;
    std::cin >> qtd;

    // Processar Retirada (O Laboratório faz as verificações de estoque e validade)
    std::string resultado = labEscolhido->registrarRetirada(this, nomeReagente, qtd);
    
    std::cout << "\nResultado: " << resultado << "\n";
}
