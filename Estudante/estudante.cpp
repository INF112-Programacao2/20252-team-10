#include "estudante.h"
#include "../Laboratorio/Laboratorio.h"
#include "../PosGraduacao/posgraduacao.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
using namespace mysqlx;

// Construtor
Estudante::Estudante(std::string nome, std::string email, std::string senha, int nivelAcesso,
                    mysqlx::abi2::r0::Schema* db, std::string matricula, std::string curso, std::string nivel):
    Usuario(nome, email, senha, nivelAcesso, db), matricula(matricula), curso(curso), nivel(nivel) {}

// Destrutor
Estudante::~Estudante() {}

// Gets
std::string Estudante::getMatricula() const{
    return matricula;
}
std::string Estudante::getCurso() const{
    return curso;
}
std::string Estudante::getNivel() const{
    return nivel;
}

std::vector<std::pair<Laboratorio*, std::string>> Estudante::getLaboratorios() const {
    return laboratorios;
}

// Sets
void Estudante::setMatricula(const std::string &matricula){
    this->matricula = matricula;
}
void Estudante::setCurso(const std::string &curso){
    this->curso = curso;
}
void Estudante::setNivel(const std::string &nivel){
    this->nivel = nivel;
}

//Outros métodos
//Adicionar laboratorio a estudante
void Estudante::adicionarLaboratorio(Laboratorio* laboratorio, const std::string& papel) {
    for (int i = 0; i < (int)laboratorios.size(); i++) {
        if (laboratorios[i].first->getId() == laboratorio->getId()) {
            std::cout << getNome() << " já possui o " << laboratorio->getNome()
                      << " associado como " << laboratorios[i].second
                      << ". Atualizando papel para " << papel << ".\n";
            laboratorios[i].second = papel; // Atualiza papel
            return;
        }
    }

    laboratorios.push_back(std::make_pair(laboratorio, papel));
    std::cout << getNome() << " está associado ao " << laboratorio->getNome()
              << " como " << papel << ".\n";
}

// Remove o objeto no laboratorio no objeto estudante
void Estudante::removerLaboratorioObjeto(Laboratorio *laboratorio)
{
    std::vector<std::pair<Laboratorio *, std::string>> novoLaboratorios; // vetor auxiliar para receber os laboratorios associados
    bool removido = false;                                               // flag para informar que foi removido
    for (int i = 0; i < (int)laboratorios.size(); i++)
    {
        if (laboratorios[i].first->getId() != laboratorio->getId())
        {
            novoLaboratorios.push_back(laboratorios[i]); // copia os que não serão removidos
        }
        else
        {
            removido = true; // encontrou e vai remover
        }
    }

    if (removido)
    {
        this->laboratorios.swap(novoLaboratorios);
        std::cout << getNome() << " desassociado do " << laboratorio->getNome() << std::endl;
    }
    else
    {
        std::cout << "Aviso: Associação com " << laboratorio->getNome() << " não encontrada em memória." << std::endl;
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
void Estudante::associarLaboratorio(Laboratorio* laboratorio, const std::string& papel) {
    // Verifica se o ponteiro do laboratorio  inválidos
    if (!laboratorio ) {
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
                this->adicionarLaboratorio(laboratorio, papel);
                return;
            }
            // Faz a inserção no banco de dados, caso nao tenha no bd
            associadoTable.insert("estudante_id", "laboratorio_id", "papel")
            .values(this->getId(), laboratorio->getId(), papel)
            .execute();
            // informa a associação
            std::cout << "Associação registrada! \n" << std::endl;
            // Aloca o laboratorio no objeto estudante
            this->adicionarLaboratorio(laboratorio, papel);
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

    std::cout << "\n=== SEUS LABORATÓRIOS ===\n";
    
    for (int i = 0; i < laboratorios.size(); i++) {
        Laboratorio* lab = laboratorios[i].first;
        std::string papel = laboratorios[i].second;

        std::cout << "\n[" << i + 1 << "] " << lab->getNome() 
                  << " (" << lab->getDepartamento() << ")\n";
        std::cout << "   Papel: " << papel << "\n";
        std::cout << "   ID: " << lab->getId() << "\n";
    }

    // Opção para gerenciar um laboratório específico
    std::cout << "\nO que deseja fazer?\n";
    std::cout << "1. Ver reagentes de um laboratório\n";
    std::cout << "2. Retirar reagente de um laboratório\n";
    std::cout << "3. Consultar estoque geral\n";
    std::cout << "0. Voltar\n";
    std::cout << "Escolha: ";
    
    int opcao;
    std::cin >> opcao;

    switch (opcao) {
        case 1: {
            if (laboratorios.size() == 1) {
                // Se só tem um laboratório, usa ele diretamente
                consultarEstoqueLaboratorio(laboratorios[0].first);
            } else {
                std::cout << "\nEscolha o laboratório:\n";
                for (int i = 0; i < laboratorios.size(); i++) {
                    std::cout << i + 1 << ". " << laboratorios[i].first->getNome() << "\n";
                }
                std::cout << "Opção: ";
                
                int escolha;
                std::cin >> escolha;
                
                if (escolha >= 1 && escolha <= static_cast<int>(laboratorios.size())) {
                    consultarEstoqueLaboratorio(laboratorios[escolha - 1].first);
                } else {
                    std::cout << "Opção inválida!\n";
                }
            }
            break;
        }
        case 2:
            retirarReagente(); 
            break;
        case 3:
            consultarEstoque(); 
            break;
        case 0:
            return;
        default:
            std::cout << "Opção inválida!\n";
    }
}

// Funcão auxiliar para consultar estoque de um laboratório específico
void Estudante::consultarEstoqueLaboratorio(Laboratorio* lab) {
    if (!lab) {
        std::cout << "Laboratório inválido.\n";
        return;
    }

    std::cout << "\n=== ESTOQUE DO LABORATÓRIO " << lab->getNome() << " ===\n";
    
    std::vector<Reagente*> lista = lab->listarReagentes("");
    
    if (lista.empty()) {
        std::cout << "Nenhum reagente cadastrado neste laboratório.\n";
        return;
    }

    // Cabecalho da tabela
    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(25) << "Nome"
              << std::setw(15) << "Qtd/Unidade"
              << std::setw(15) << "Validade"
              << std::setw(10) << "Nível" << "\n";
    std::cout << std::string(70, '-') << "\n";

    // Lista os reagentes
    for (Reagente* r : lista) {
        // Verifica se o estudante tem permissão para ver este reagente
        if (this->getNivelAcesso() <= r->getNivelAcesso()) {
            std::string nivelStr;
            switch(r->getNivelAcesso()) {
                case 1: nivelStr = "Restrito"; break;
                case 2: nivelStr = "Livre"; break;
                case 3: nivelStr = "Pós"; break;
                default: nivelStr = "Desconhecido";
            }

            std::cout << std::left
                      << std::setw(5) << r->getId()
                      << std::setw(25) << r->getNome()
                      << std::setw(15) << (std::to_string(r->getQuantidade()) + " " + r->getUnidadeMedida())
                      << std::setw(15) << r->getDataValidade()
                      << std::setw(10) << nivelStr << "\n";
        } else {
            // Se não tem permissão, mostra apenas nome e indica que é restrito
            std::cout << std::left
                      << std::setw(5) << r->getId()
                      << std::setw(25) << r->getNome()
                      << std::setw(15) << "***"
                      << std::setw(15) << "***"
                      << std::setw(10) << "Restrito" << "\n";
        }
    }
    
    std::cout << std::string(70, '-') << "\n";
    
    // Opção para ver detalhes de um reagente específico
    std::cout << "\nDigite o ID do reagente para ver detalhes (0 para sair): ";
    int idReagente;
    std::cin >> idReagente;
    
    if (idReagente > 0) {
        acessarReagenteRestrito(idReagente);
    }
}

// Lista reagentes de todos os laboratórios do estudante
void Estudante::consultarEstoque() {
    if (this->laboratorios.empty()) {
        std::cout << "Você não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== CONSULTA DE ESTOQUE GERAL ===\n";

    // Itera sobre o vetor de pair<Laboratorio*, string>
    for (int i = 0; i < laboratorios.size(); i++) {
        std::pair<Laboratorio*, std::string> p = laboratorios[i];
        Laboratorio* lab = p.first;
        std::string papel = p.second;

        std::cout << "\n>> Laboratório: " << lab->getNome()
                  << " | Papel: " << papel << "\n";

        std::vector<Reagente*> lista = lab->listarReagentes("");

        if (lista.empty()) {
            std::cout << "   (Nenhum reagente cadastrado)\n";
            continue;
        }

        std::cout << std::left
                  << std::setw(25) << "Nome"
                  << std::setw(15) << "Qtd"
                  << std::setw(15) << "Nível" << "\n\n";

        for (int j = 0; j < lista.size(); j++) {
            Reagente* r = lista[j];
            std::string nivelStr;
            if (r->getNivelAcesso() == 1) nivelStr = "Restrito";
            else if (r->getNivelAcesso() == 2) nivelStr = "Livre";
            else nivelStr = "Pós-Grad";

            // Verifica permissão
            if (this->getNivelAcesso() <= r->getNivelAcesso()) {
                std::cout << std::left
                          << std::setw(25) << r->getNome()
                          << std::setw(15) << (std::to_string(r->getQuantidade()) + " " + r->getUnidadeMedida())
                          << std::setw(15) << nivelStr << "\n";
            } else {
                std::cout << std::left
                          << std::setw(25) << r->getNome()
                          << std::setw(15) << "***"
                          << std::setw(15) << "Restrito" << "\n";
            }
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

    // Escolher o Laboratório
    std::cout << "\n=== RETIRADA DE REAGENTE ===\n";
    std::cout << "Escolha o laboratório:\n";
    for (int i = 0; i < laboratorios.size(); i++) {
        std::cout << i + 1 << ". " << laboratorios[i].first->getNome() << "\n";
    }
    std::cout << "Opção: ";
    int opLab;
    std::cin >> opLab;

    if (opLab < 1 || opLab > (int)laboratorios.size()) {
        std::cout << "Opção inválida.\n";
        return;
    }
    Laboratorio* labEscolhido = laboratorios[opLab - 1].first;

    // Escolher o Reagente (Busca por nome)
    std::cout << "Digite o nome do reagente: ";
    std::string nomeReagente;
    std::cin.ignore();
    std::getline(std::cin, nomeReagente);

    // Escolher a Quantidade
    std::cout << "Digite a quantidade a retirar: ";
    float qtd;
    std::cin >> qtd;

    // Validar quantidade
    if (qtd <= 0) {
        std::cout << "Quantidade deve ser maior que zero.\n";
        return;
    }

    // Processar Retirada (O Laboratório faz as verificações de estoque e validade)
    std::string resultado = labEscolhido->registrarRetirada(this, nomeReagente, qtd);

    std::cout << "\nResultado: " << resultado << "\n";
}