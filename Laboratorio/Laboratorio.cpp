#include "Laboratorio.h"
#include "../Estudante/estudante.h"
#include "../PosGraduacao/posgraduacao.h"
#include "../Gestor/gestor.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <mysql-cppconn/mysqlx/xdevapi.h>
using namespace mysqlx;

//lista de todos os laboratórios
std::vector<Laboratorio*> Laboratorio::laboratorios;

//Construtor
Laboratorio::Laboratorio(int id, const std::string &nome, const std::string &departamento, Schema* db)
: id(id), nome(nome), departamento(departamento), db(db)
{
    //Se tem conexão com BD, carrega os reagentes
    if (this->db) {
        std::cout << "Laboratorio conectado ao DB. Reagentes sendo carregados" << std::endl;
        carregarReagentesDoDB();  //busca reagentes no banco
    }
}

//Destrutor
Laboratorio::~Laboratorio()
{
    //Libera todas as retiradas
    for (size_t i = 0; i < retiradas.size(); i++) {
        delete retiradas[i];  // delete para ponteiros alocados com new
    }
    //Libera todos os reagentes
    for (size_t i = 0; i < reagentes.size(); i++) {
        delete reagentes[i];
    }
}

//Carrega reagentes do banco de dados para a memória
void Laboratorio::carregarReagentesDoDB() {
    try {
        //Primeiro carrega todos os reagentes da tabela base
        Table reagenteTable = db->getTable("Reagente");
        RowResult resBase = reagenteTable.select("*").execute();
        std::vector<Row> rowsBase = resBase.fetchAll();  //Pega todas as linhas

        //Para cada reagente no banco
        for (size_t i = 0; i < rowsBase.size(); i++) {
            Row row = rowsBase[i];
            Reagente* novoReagente = nullptr;  //Ponteiro que vai receber o reagente
            
            //Extrai dados da linha do banco
            int id = row[0].get<int>();
            std::string nome = row[1].get<std::string>();
            int qtd = row[2].get<int>();
            int qtdCritica = row[3].get<int>();
            std::string codRef = row[4].get<std::string>();
            std::string marca = row[5].get<std::string>();
            std::string local = row[6].get<std::string>();
            int nivelAcesso = row[7].get<int>();
            std::string unidade = row[8].get<std::string>();
            std::string validade = row[9].get<std::string>();

            //Tenta ver se é reagente líquido
            try {
                Table liquidoTable = db->getTable("ReagenteLiquido");
                RowResult resLiquido = liquidoTable.select("*")
                    .where("id = :id")  //WHERE id = ?
                    .bind("id", id)     //Substitui :id pelo valor
                    .execute();
                
                //Se encontrou na tabela líquido, cria ReagenteLiquido
                if (resLiquido.count() > 0) {
                    Row rowLiquido = resLiquido.fetchOne();
                    double densidade = rowLiquido[1].get<double>();
                    double volume = rowLiquido[2].get<double>();
                    novoReagente = new ReagenteLiquido(id, nome, validade, qtd, qtdCritica, local, 
                                                       nivelAcesso, unidade, marca, codRef, 
                                                       densidade, volume);
                }
            } catch (const mysqlx::Error &err) {
                //Se deu erro, não é líquido - continua para próxima verificação
            }

            //Se não é líquido, verifica se é sólido
            if (!novoReagente) {
                try {
                    Table solidoTable = db->getTable("ReagenteSolido");
                    RowResult resSolido = solidoTable.select("*")
                        .where("id = :id")
                        .bind("id", id)
                        .execute();
                    
                    //Se encontrou na tabela sólido, cria ReagenteSolido
                    if (resSolido.count() > 0) {
                        Row rowSolido = resSolido.fetchOne();
                        double massa = rowSolido[1].get<double>();
                        std::string estado = rowSolido[2].get<std::string>();
                        novoReagente = new ReagenteSolido(id, nome, validade, qtd, qtdCritica, local, 
                                                          nivelAcesso, unidade, marca, codRef, 
                                                          massa, estado);
                    }
                } catch (const mysqlx::Error &err) {
                    //Não é sólido também
                }
            }

            //Se não é líquido nem sólido, cria reagente básico
            if (!novoReagente) {
                novoReagente = new Reagente(id, nome, validade, qtd, qtdCritica, local, 
                                           nivelAcesso, unidade, marca, codRef);
            }

            // Adiciona no vetor de reagentes do laboratório
            if (novoReagente) {
                this->reagentes.push_back(novoReagente);
            }
        }
        std::cout << this->reagentes.size() << " reagentes carregados do DB para a memoria." << std::endl;

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao carregar reagentes do DB: " << err.what() << std::endl;
    }
}

//Busca reagente pelo nome (busca parcial - encontra "ácido" em "ácido sulfúrico")
Reagente *Laboratorio::buscarReagente(const std::string &nome)
{
    //Percorre todos os reagentes
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        //find retorna a posição onde encontrou, ou npos se não encontrou
        if (reagentes[i]->getNome().find(nome) != std::string::npos)
        {
            return reagentes[i];  //Retorna o ponteiro para o reagente
        }
    }
    return nullptr;  //Retorna null se não encontrou
}

//Lista reagentes 
std::vector<Reagente *> Laboratorio::listarReagentes(const std::string &filtroNome)
{
    if (filtroNome.empty())
    {
        return reagentes;  //Retorna cópia do vetor completo
    }
    
    //Se tem filtro, cria novo vetor apenas com os que correspondem
    std::vector<Reagente *> resultado;
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        if (reagentes[i]->getNome().find(filtroNome) != std::string::npos)
        {
            resultado.push_back(reagentes[i]);  //Adiciona no resultado
        }
    }
    return resultado;
}

//Registra uma retirada de reagente
std::string Laboratorio::registrarRetirada(Usuario *usuario, const std::string &nomeReagente, float quantidade)
{
    //Primeiro encontra o reagente
    Reagente *reagente = buscarReagente(nomeReagente);
    if (!reagente)
    {
        return "Erro: Reagente '" + nomeReagente + "' nao encontrado";
    }

    //Verifica se não está vencido
    if (reagente->estaVencido())
    {
        return "Erro: Reagente '" + reagente->getNome() + "' esta vencido";
    }

    //Cria objeto Retirada
    Retirada *novaRetirada = new Retirada(retiradas.size() + 1, usuario, reagente, quantidade);
    
    //Tenta confirmar a retirada (verifica estoque, etc)
    std::string resultado = novaRetirada->confirmarRetirada();

    //Se deu certo (não tem "Erro:" na mensagem)
    if (resultado.find("Erro:") == std::string::npos) 
    {
        try {
            //Pega nova quantidade após retirada
            int novaQuantidade = reagente->getQuantidade();
            int reagenteId = reagente->getId();

            //Atualiza no banco de dados
            db->getTable("Reagente").update()
                .set("quantidade", novaQuantidade)
                .where("id = :id")
                .bind("id", reagenteId)
                .execute();
            
            std::cout << "Banco de dados atualizado para a retirada." << std::endl;
            retiradas.push_back(novaRetirada);  //Adiciona no histórico

        } catch (const mysqlx::Error &err) {
            //Se BD falhou, cancela a retirada na memória
            std::cerr << "ERRO de DB ao atualizar quantidade: " << err.what() << std::endl;
            novaRetirada->cancelarRetirada();  // Reverte estoque na memória
            delete novaRetirada;  // Libera memória
            return "Erro no banco ao registrar retirada. Estoque em memoria revertido.";
        }
    }
    else
    {
        //Se a retirada falhou na memória (ex: sem estoque), libera memória
        delete novaRetirada;
    }

    return resultado;  //Retorna mensagem de sucesso ou erro
}

//Lista reagentes com quantidade crítica (estoque baixo)
std::vector<Reagente *> Laboratorio::getReagentesCriticos()
{
    std::vector<Reagente *> criticos;
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        //Usa método do reagente para verificar se está crítico
        if (reagentes[i]->verificarNivelCritico())
        {
            criticos.push_back(reagentes[i]);
        }
    }
    return criticos;
}

//Carrega todos os laboratórios do banco para a memória
std::vector<Laboratorio*> Laboratorio::listarLaboratorios(Schema* db)
{
    Laboratorio::laboratorios.clear();  // Limpa lista atual
    Table table = db->getTable("Laboratorio");
    RowResult result = table.select("id", "nome", "departamento").execute();
    
    Row row;
    //fetchOne() retorna uma linha por vez, quando acaba retorna Row vazia (false)
    while((row = result.fetchOne())){
        int id = row[0].get<int>();
        std::string nome = row[1].get<std::string>();
        std::string departamento = row[2].get<std::string>();
        
        //Cria novo laboratório e adiciona na lista estática
        Laboratorio* laboratorio = new Laboratorio(id, nome, departamento, db);
        Laboratorio::laboratorios.push_back(laboratorio);
    }
    return Laboratorio::laboratorios;
}

//Imprime lista formatada de laboratórios
void Laboratorio::imprimirLaboratorios()
{
    std::cout << "\n-----------------------------------------------\n";
    std::cout << "| ID  | Nome                 | Departamento    |\n";
    std::cout << "-----------------------------------------------\n";

    //Percorre todos os laboratórios carregados
    for (size_t i = 0; i < Laboratorio::laboratorios.size(); i++) {
        Laboratorio* laboratorio = Laboratorio::laboratorios[i];
        //Usa setw para formatar a tabela bonitinha >-<
        std::cout << "| "
                  << std::setw(3)  << laboratorio->getId()        << " | "
                  << std::setw(20) << laboratorio->getNome()      << " | "
                  << std::setw(15) << laboratorio->getDepartamento()
                  << " |\n";
    }
    std::cout << "-----------------------------------------------\n";
}

// FUNÇÕES NÃO IMPLEMENTADAS

/*
bool Laboratorio::verificarRetiradasPendentes(Usuario *usuario) {
    //Implementar verificação de retiradas pendentes
}
*/

/*
void Laboratorio::cadastrarNovoReagente(
    std::string nome, std::string dataValidade, int quantidade,
    int quantidadeCritica, std::string local, int nivelAcesso,
    std::string unidade, std::string marca, std::string codRef,
    int tipo, double densidade, double volume,
    double massa, std::string estadoFisico
) {
    //Implementar cadastro de novo reagente no sistema
}
*/

/*
std::vector<Reagente *> Laboratorio::listarReagentesPorLocal(const std::string &local) {
    //Implementar listagem de reagentes por local de armazenamento
}
*/

/*
std::vector<Retirada *> Laboratorio::listarRetiradasUsuario(Usuario *usuario) {
    //Implementar listagem de retiradas por usuário
}
*/

/*
std::vector<Retirada *> Laboratorio::getHistoricoRecente() {
    //Implementar obtenção das 10 retiradas mais recentes
}
*/

/*
std::string Laboratorio::adicionarUsuario(Usuario *usuario) {
    //Implementar adição de usuário ao laboratório
}
*/

/*
std::string Laboratorio::removerUsuario(Usuario *usuario) {
    //Implementar remoção de usuário do laboratório
}
*/

/*
std::vector<Reagente *> Laboratorio::getReagentesVencidos() {
    //Implementar obtenção de reagentes vencidos
}
*/

/*
std::string Laboratorio::getAlertasGestor() {
    //Implementar relatório consolidado para gestores
}
*/

/*
std::string Laboratorio::getEstatisticas() {
    //Implementar estatísticas do laboratório
}
*/

/*
std::string Laboratorio::toString() const {
    //Implementar representação em string do laboratório
}
*/

/*
void Laboratorio::limparLaboratorios() {
    //Implementar limpeza de memória de todos os laboratórios
}
*/

/*
void Laboratorio::adicionarGestor(Gestor* gestor) {
    //Implementar adição de gestor ao laboratório
}
*/

/*
void Laboratorio::adicionarEstudante(Estudante* estudante) {
    //Implementar adição de estudante ao laboratório
}
*/

/*
void Laboratorio::removerEstudante(Estudante* estudante) {
    //Implementar remoção de estudante do laboratório
}
*/