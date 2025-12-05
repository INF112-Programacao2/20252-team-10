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

// lista de todos os laboratórios
std::vector<Laboratorio *> Laboratorio::laboratorios;

// Construtor
Laboratorio::Laboratorio(int id, const std::string &nome, const std::string &departamento, Schema *db)
    : id(id), nome(nome), departamento(departamento), db(db)
{
    // Se tem conexão com BD, carrega os reagentes
    if (this->db)
    {
        carregarReagentesDoDB(); // busca reagentes no banco
        carregarAlertasDB();
        getReagentesCriticos();
        getReagentesVencidos();
    }
}

// Destrutor
Laboratorio::~Laboratorio()
{
    // Libera todas as retiradas
    for (int i = 0; i < retiradas.size(); i++)
    {
        if (retiradas[i] != nullptr)
            delete retiradas[i]; // delete para ponteiros alocados com new
    }
    // Libera todos os reagentes
    for (int i = 0; i < reagentes.size(); i++)
    {
        if (reagentes[i] != nullptr)
            delete reagentes[i];
    }
    // Libera todos os alertas
    for (int i = 0; i < alertas.size(); i++)
    {
        if (alertas[i] != nullptr)
            delete alertas[i];
    }
}
void Laboratorio::removerEstudante(Estudante *estudante)
{
    int idEstudante = estudante->getId();
    // Remove graduação
    for (int i = 0; i < (int)estudantesGraduacao.size(); i++)
    {
        Estudante *e = estudantesGraduacao[i];

        if (e != nullptr && e->getId() == idEstudante)
        {
            estudantesGraduacao.erase(estudantesGraduacao.begin() + i);
            return; // já removeu, sai
        }
    }
    // Remove pós
    for (int i = 0; i < (int)estudantesPosGraduacao.size(); i++)
    {
        PosGraduacao *p = estudantesPosGraduacao[i];

        if (p != nullptr && p->getId() == idEstudante)
        {
            estudantesPosGraduacao.erase(estudantesPosGraduacao.begin() + i);
            return; // removeu
        }
    }
}
bool Laboratorio::estaAssociado(Estudante *estudante)
{
    if (estudante == nullptr)
        return false;
    bool emMemoria = false; // Flag para verificar se está em memória
    bool noBanco = false;   // Flag para verificar se está no banco de dados
    int idEstudante = estudante->getId();

    for (Estudante *e : estudantesGraduacao)
        if (e->getId() == idEstudante)
            emMemoria = true;

    for (PosGraduacao *p : estudantesPosGraduacao)
        if (p->getId() == idEstudante)
            emMemoria = true;

    // Verifica se já está associado no BD
    try
    {
        Table associado = db->getTable("Associado");
        RowResult r = associado.select("estudante_id")
                          .where("estudante_id = :id AND laboratorio_id = :lab")
                          .bind("id", idEstudante)
                          .bind("lab", this->id)
                          .execute();

        noBanco = (r.count() > 0);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao verificar associação no BD: " << e.what() << std::endl;
        return false;
    }
    // caso 1: MEMÓRIA = true, BD = false
    if (emMemoria && !noBanco)
    {
        // std::cerr << "Aviso: Estudante está no OBJETO mas não no BD! Corrigindo...\n";

        // remover do objeto
        removerEstudante(estudante);
        estudante->removerLaboratorio(this);

        return false;
    }
    // caso 2: MEMÓRIA = false, BD = true
    if (!emMemoria && noBanco)
    {
        // std::cerr << "Aviso: Estudante está no BD mas não no OBJETO! Corrigindo...\n";

        // adicionar ao objeto
        adicionarEstudante(estudante);

        return true;
    }
    // caso 3: ambos true ou ambos false
    return emMemoria && noBanco;
}

// Carrega reagentes do banco de dados para a memória
void Laboratorio::carregarReagentesDoDB()
{
    try
    {
        // Primeiro carrega todos os reagentes da tabela base
        Table reagenteTable = db->getTable("Reagente");
        RowResult resBase = reagenteTable.select("*").execute();
        std::vector<Row> rowsBase = resBase.fetchAll(); // Pega todas as linhas

        // LEFT JOIN é usado para pegar dados de Liquido ou Solido
        // ORDER BY Permite listar reagentes em ordem alfabetica
        SqlResult res = db->getSession().sql("SELECT *, DATE_FORMAT(validade, '%Y-%m-%d') as data_formatada FROM LabUFV.Reagente AS R LEFT JOIN LabUFV.ReagenteLiquido AS RL ON R.id = RL.id LEFT JOIN LabUFV.ReagenteSolido AS RS ON R.id = RS.id ORDER BY R.nome ASC").execute();

        int size = res.count();
        for (int j = 0; j < size; j++)
        {
            Row row = res.fetchOne();
            Reagente *novoReagente = nullptr;

            std::string validade = "Desconhecida";

            int id = row[0].get<int>();
            std::string nome = row[2].get<std::string>();
            double qtd = row[3].get<double>();
            double qtdCritica = row[4].get<double>();
            std::string codRef = row[5].get<std::string>();
            std::string marca = row[6].get<std::string>();
            std::string local = row[7].get<std::string>();
            int nivelAcesso = row[8].get<int>();
            std::string unidade = row[9].get<std::string>();
            // a validade formatada vem na ultima linha
            if (!(row[row.colCount() - 1].isNull()))
            {
                validade = row[row.colCount() - 1].get<std::string>();
            }
            else
            {
                validade = "Desconhecida";
            }
            // Verifica se e Liquido (checa se a coluna do JOIN nao e nula)
            if (!(row[12].isNull()))
            {
                double densidade = row[13].get<double>();
                double volume = row[14].get<double>();
                novoReagente = new ReagenteLiquido(id, nome, validade, qtd, qtdCritica, local,
                                                   nivelAcesso, unidade, marca, codRef,
                                                   densidade, volume); //
            }
            // Verifica se e Solido
            else if (!(row[15].isNull()))
            {
                double massa = row[16].get<double>();
                std::string estado = row[17].get<std::string>();
                novoReagente = new ReagenteSolido(id, nome, validade, qtd, qtdCritica, local,
                                                  nivelAcesso, unidade, marca, codRef,
                                                  massa, estado); //
            }
            if (novoReagente)
            {
                this->reagentes.push_back(novoReagente); // Adiciona no vector
            }
        }
        // std::cout << this->reagentes.size() << " reagentes carregados do DB para a memoria." << std::endl;
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao carregar reagentes do DB: " << err.what() << std::endl;
    }
}

// Busca reagente pelo nome (busca parcial - encontra "ácido" em "ácido sulfúrico")
Reagente *Laboratorio::buscarReagente(const std::string &nome)
{
    // Percorre todos os reagentes
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        std::cout << "Verificando reagente: '" << reagentes[i]->getId() << " - " << reagentes[i]->getNome() << "'\n";
        // find retorna a posição onde encontrou, ou npos se não encontrou
        if (reagentes[i]->getNome().find(nome) != std::string::npos)
        {
            return reagentes[i]; // Retorna o ponteiro para o reagente
        }
    }
    std::cout << "Nao encontrado\n";
    return nullptr; // Retorna null se não encontrou
}

// Lista reagentes
std::vector<Reagente *> Laboratorio::listarReagentes(const std::string &filtroNome)
{
    if (filtroNome.empty())
    {
        return reagentes; // Retorna cópia do vetor completo
    }

    // Se tem filtro, cria novo vetor apenas com os que correspondem
    std::vector<Reagente *> resultado;
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        if (reagentes[i]->getNome().find(filtroNome) != std::string::npos)
        {
            resultado.push_back(reagentes[i]); // Adiciona no resultado
        }
    }
    return resultado;
}

// Registra uma retirada de reagente
std::string Laboratorio::registrarRetirada(Usuario *usuario, const std::string &nomeReagente, float quantidade)
{
    // Primeiro encontra o reagente
    Reagente *reagente = buscarReagente(nomeReagente);
    if (reagente == nullptr)
    {
        std::cout << "Erro: Reagente '" + nomeReagente + "' nao encontrado" << std::endl;
        return "Erro: Reagente '" + nomeReagente + "' nao encontrado";
    }

    // Verifica se não está vencido
    if (reagente->estaVencido())
    {
        std::cout << "Erro: Reagente '" + reagente->getNome() + "' esta vencido" << std::endl;
        return "Erro: Reagente '" + reagente->getNome() + "' esta vencido";
    }

    // Cria objeto Retirada
    Retirada *novaRetirada = new Retirada(retiradas.size() + 1, usuario, reagente, quantidade);

    // Tenta confirmar a retirada (verifica estoque, etc)
    std::string resultado = novaRetirada->confirmarRetirada();

    // Se deu certo (não tem "Erro:" na mensagem)
    if (resultado.find("Erro:") == std::string::npos)
    {
        try
        {
            // Pega nova quantidade após retirada
            int novaQuantidade = reagente->getQuantidade();
            int reagenteId = reagente->getId();

            // Atualiza no banco de dados
            db->getTable("Reagente").update().set("quantidade", novaQuantidade).where("id = :id").bind("id", reagenteId).execute();

            std::cout << "Banco de dados atualizado para a retirada." << std::endl;
            retiradas.push_back(novaRetirada); // Adiciona no histórico

            // Gerar data/hora atual
            time_t agora = time(nullptr);
            struct tm tempoInfo;

// Usar localtime_s / localtime_r
#ifdef _WIN32
            localtime_s(&tempoInfo, &agora);
#else
            localtime_r(&agora, &tempoInfo);
#endif

            // Buffer
            char buff[30];
            strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", &tempoInfo);

            std::string hora = std::string(buff);
            std::cout << "Data/Hora registrada: " << hora << std::endl;

            db->getTable("Retirada")
                .insert("usuario_id", "reagente_id", "dataHoraRetirada", "quantidadeRetirada")
                .values(usuario->getId(), reagenteId, hora, quantidade)
                .execute();

            std::cout << "Retirada registrada no banco com sucesso." << std::endl;
        }
        catch (const mysqlx::Error &err)
        {
            // Se BD falhou, cancela a retirada na memória
            std::cerr << "ERRO de DB ao atualizar quantidade: " << err.what() << std::endl;
            novaRetirada->cancelarRetirada(); // Reverte estoque na memória
            delete novaRetirada;              // Libera memória
            return "Erro no banco ao registrar retirada. Estoque em memoria revertido.";
        }
    }
    else
    {
        // Se a retirada falhou na memória (ex: sem estoque), libera memória
        delete novaRetirada;
    }

    return resultado; // Retorna mensagem de sucesso ou erro
}

// Lista reagentes com quantidade crítica (estoque baixo)
std::vector<Reagente *> Laboratorio::getReagentesCriticos()
{
    Alerta *al;
    std::vector<Reagente *> criticos;
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        // Usa método do reagente para verificar se está crítico
        if (reagentes[i]->verificarNivelCritico())
        {
            criticos.push_back(reagentes[i]);
            al = new AlertaQuantidade(reagentes[i], 1);
            alertas.push_back(al);
            al->adicionarAlertaBD(db);
        }
    }
    return criticos;
}

std::vector<Reagente *> Laboratorio::getReagentesVencidos()
{
    Alerta *al;
    std::vector<Reagente *> vencidos;
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        // Usa método do reagente para verificar se está crítico
        if (reagentes[i]->estaVencido())
        {
            vencidos.push_back(reagentes[i]);
            al = new AlertaValidade(reagentes[i], 1);
            alertas.push_back(al);
            al->adicionarAlertaBD(db);
        }
    }
    return vencidos;
}

// Carrega todos os laboratórios do banco para a memória
std::vector<Laboratorio *> Laboratorio::listarLaboratorios(Schema *db)
{
    limparLaboratorios();
    Table table = db->getTable("Laboratorio");
    RowResult result = table.select("id", "nome", "departamento").execute();

    Row row;
    // fetchOne() retorna uma linha por vez, quando acaba retorna Row vazia (false)
    while ((row = result.fetchOne()))
    {
        int id = row[0].get<int>();
        std::string nome = row[1].get<std::string>();
        std::string departamento = row[2].get<std::string>();

        // Cria novo laboratório e adiciona na lista estática
        Laboratorio *laboratorio = new Laboratorio(id, nome, departamento, db);
        Laboratorio::laboratorios.push_back(laboratorio);
    }
    return Laboratorio::laboratorios;
}

// Imprime lista formatada de laboratórios
void Laboratorio::imprimirLaboratorios()
{
    std::cout << "\n-----------------------------------------------\n";
    std::cout << "| ID  | Nome                 | Departamento    |\n";
    std::cout << "-----------------------------------------------\n";

    // Percorre todos os laboratórios carregados
    for (size_t i = 0; i < Laboratorio::laboratorios.size(); i++)
    {
        Laboratorio *laboratorio = Laboratorio::laboratorios[i];
        // Usa setw para formatar a tabela bonitinha >-<
        std::cout << "| "
                  << std::setw(3) << laboratorio->getId() << " | "
                  << std::setw(20) << laboratorio->getNome() << " | "
                  << std::setw(15) << laboratorio->getDepartamento()
                  << " |\n";
    }
    std::cout << "-----------------------------------------------\n";
}

// FUNCOES ALERTA

void Laboratorio::carregarAlertasDB()
{
    try
    {
        Alerta *novoAlerta = nullptr;
        SqlResult res = db->getSession().sql("SELECT *, DATE_FORMAT(dataHoraEmissao, '%Y-%m-%d %H:%i:%s') as data_formatada FROM LabUFV.Alerta").execute();
        int achados = res.count();
        for (int i = 0; i < achados; i++)
        {
            Row row = res.fetchOne();
            int id = row[0].get<int>();
            int reagente_id = row[1].get<int>();
            // pula data nao formatada
            int tipo = row[3].get<int>();
            bool situacao = row[4].get<bool>();
            std::string dataHoraEmissao = row[5].get<std::string>();

            Reagente *reagenteEmAlerta = nullptr;

            for (int j = 0; j < reagentes.size(); j++)
            {
                if (reagentes.at(j)->getId() == reagente_id)
                    reagenteEmAlerta = reagentes.at(j);
            }

            if (tipo == 1)
            { // se o tipo for validade
                novoAlerta = new AlertaValidade(id, reagenteEmAlerta, dataHoraEmissao, situacao);
            }
            else if (tipo == 2)
            { // se o tipo for quantidade critica
                novoAlerta = new AlertaQuantidade(id, reagenteEmAlerta, dataHoraEmissao, situacao);
            }

            if (novoAlerta)
            {
                alertas.push_back(novoAlerta);
            }
        }
    }
    catch (mysqlx::Error &e)
    {
        std::cout << "Erro ao carregar Alertas" << std::endl;
    }
}

void Laboratorio::getAlertasGestor()
{
    bool nenhumAlertaAtivo = true;
    for (Alerta *a : alertas)
    {
        if (a->getSituacao() == true)
        {
            a->notificar();
            nenhumAlertaAtivo = false;
        }
    }
    if (nenhumAlertaAtivo)
    {
        std::cout << "Nenhum alerta ativo!\n";
    }
}

void Laboratorio::cadastrarNovoReagente(
    int id, std::string nome, std::string dataValidade, int quantidade,
    int quantidadeCritica, std::string local, int nivelAcesso,
    std::string unidade, std::string marca, std::string codRef,
    int tipo, double densidade, double volume,
    double massa, std::string estadoFisico)
{
    // Cadastro de novo reagente no sistema
    Reagente *novoReagente = nullptr;

    try
    {
        // Verificar o tipo para instanciar a classe correta
        if (tipo == 1)
        { // Liquido
            novoReagente = new ReagenteLiquido(
                id, nome, dataValidade, quantidade, quantidadeCritica,
                local, nivelAcesso, unidade, marca, codRef,
                densidade, volume);
        }
        else if (tipo == 2)
        { // solido
            novoReagente = new ReagenteSolido(
                id, nome, dataValidade, quantidade, quantidadeCritica,
                local, nivelAcesso, unidade, marca, codRef,
                massa, estadoFisico);
        }
        else
        {
            std::cerr << "Erro: Tipo de reagente inválido ao atualizar memória." << std::endl;
            return;
        }
        // adiciona ao vetor de reagentes do laboratorio
        this->reagentes.push_back(novoReagente);

        std::cout << "Memória do laboratório atualizada com sucesso." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro ao criar objeto na memória: " << e.what() << std::endl;
    }
}

void Laboratorio::removerReagenteDaMemoria(int idReagente)
{
    // Percorre o vetor para remover com segurança
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        if (reagentes[i]->getId() == idReagente)
        {

            delete reagentes[i]; // libera memoria do objeto
            // remove o ponteiro do vetor na posicao i
            reagentes.erase(reagentes.begin() + i);

            std::cout << "Reagente removido da memória do sistema!\n";
            return; // sai da funcao pra evitar erro de indice
        }
    }
    std::cout << "Reagente não encontrado na memória local.\n";
}

void Laboratorio::limparLaboratorios()
{
    for (int i = 0; i < laboratorios.size(); i++)
    {
        delete laboratorios[i];
    }
    Laboratorio::laboratorios.clear(); // Limpa lista atual
    return;
}

void Laboratorio::adicionarGestor(Gestor *gestor)
{
    if (gestor == nullptr)
        return;

    // Verificar se o gestor já está na lista
    for (Gestor *g : gestores)
    {
        if (g->getId() == gestor->getId())
        {
            return; // Já está na lista
        }
    }

    // Adicionar à lista
    gestores.push_back(gestor);
}

void Laboratorio::removerGestor(int id)
{
    for (int i = 0; i < gestores.size(); i++)
    {
        if (gestores[i]->getId() == id)
        {
            try
            {
                gestores.erase(gestores.begin() + i);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

void Laboratorio::menuRemoverGestor()
{
    int id;
    char op;
    std::cout << "Id do gestor removido: ";
    std::cin >> id;
    std::cout << "Tem certeza que quer remover o gestor de id = " << id << "? (S/N): ";
    std::cin >> op;
    if (op == 'N')
    {
        std::cout << "Remoção cancelada\n";
        return;
    }
    else if (op == 'S')
    {
        for (int i = 0; i < gestores.size(); i++)
        {
            if (gestores[i]->getId() == id)
            {
                try
                {
                    gestores.erase(gestores.begin() + i);
                    std::cout << "Gestor removido do Laboratório\n";
                    return;
                }
                catch (std::exception &e)
                {
                    std::cout << e.what();
                }
            }
        }
    }
    else
    {
        std::cout << "Opção inválida\n";
        return;
    }
}

// Retorna um vetor de ponterio com todos ussuarios fazendo um upcasting
std::vector<Usuario *> Laboratorio::getUsuarios()
{
    std::vector<Usuario *> todosUsuarios;
    // Adiciona gestores
    for (Gestor *g : gestores)
        todosUsuarios.push_back(static_cast<Usuario *>(g));

    // Adiciona estudantes de graduação
    for (Estudante *e : estudantesGraduacao)
        todosUsuarios.push_back(static_cast<Usuario *>(e));

    // Adiciona estudantes de pós
    for (PosGraduacao *p : estudantesPosGraduacao)
        todosUsuarios.push_back(static_cast<Usuario *>(p));

    return todosUsuarios;
};

std::vector<Estudante *> Laboratorio::getEstudantes()
{
    std::vector<Estudante *> todosEstudantes;
    // Adiciona estudantes de graduação
    for (Estudante *e : this->estudantesGraduacao)
    {
        todosEstudantes.push_back(e);
    }
    // Adiciona estudantes de pós
    for (PosGraduacao *p : this->estudantesPosGraduacao)
    {
        todosEstudantes.push_back(static_cast<Estudante *>(p));
    }

    return todosEstudantes;
};

void Laboratorio::imprimirEstudantes(const std::vector<Estudante *> &estudantes)
{
    std::cout << "\n-------------------------------------------------------------------------------\n";
    std::cout << "| ID  | Matricula     | Nome                     | Email                  | Nivel          |\n";
    std::cout << "-------------------------------------------------------------------------------\n";
    if (estudantes.empty())
    {
        std::cout << "            Não há estudantes cadastrados            \n";
    }
    for (Estudante *e : estudantes)
    {
        std::string tipo;

        if (e->getNivelAcesso() == 3)
            tipo = "Graduação";
        else if (e->getNivelAcesso() == 2)
            tipo = "Pós-Graduação";
        else
            tipo = "Desconhecido";

        std::cout << "| "
                  << std::setw(3) << e->getId() << " | "
                  << std::setw(12) << e->getMatricula() << " | "
                  << std::setw(23) << e->getNome() << " | "
                  << std::setw(20) << e->getEmail() << " | "
                  << std::setw(13) << tipo << " |\n";
    }

    std::cout << "-------------------------------------------------------------------------------\n";
}

void Laboratorio::listarEstudantes()
{
    imprimirEstudantes(getEstudantes());

    int opcao;
    std::cout << "\nO que deseja fazer?\n";
    std::cout << "1. Associar estudante\n";
    std::cout << "2. Desassociar estudante\n";
    std::cout << "0. Voltar\n";
    std::cout << "Escolha: ";
    std::cin >> opcao;

    switch (opcao)
    {
    case 1:
        menuAssociarEstudante();
        break;
    case 2:
        menuDesassociarEstudante();
        break;
    case 0:
        return;
    default:
        std::cout << "Opcao invalida!\n";
        break;
    }
}

std::string Laboratorio::associarEstudante(Estudante *estudante)
{
    if (estudante == nullptr)
        return "Erro: Estudante inválido.";

    int idEstudante = estudante->getId();

    // Verifica se já está associado
    if (estaAssociado(estudante))
        return "Estudante já está associado ao laboratório.";

    // Ler papel
    std::string papel;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer do cin
    std::cout << "\nDigite o papel do estudante no laboratório: ";
    std::getline(std::cin, papel);

    if (papel.empty())
        papel = "Estudante";

    // Inserir no BD
    try
    {
        db->getTable("Associado")
            .insert("estudante_id", "laboratorio_id", "papel")
            .values(idEstudante, this->id, papel)
            .execute();
    }
    catch (const std::exception &e)
    {
        return std::string("Erro ao inserir no BD: ") + e.what();
    }

    // Inserir no vetor do laboratório
    if (estudante->getNivelAcesso() == 3)
    {
        estudantesGraduacao.push_back(estudante);
    }
    else if (estudante->getNivelAcesso() == 2)
    {
        PosGraduacao *pg = dynamic_cast<PosGraduacao *>(estudante);
        if (pg != nullptr)
            estudantesPosGraduacao.push_back(pg);
        else
            estudantesGraduacao.push_back(estudante);
    }

    // Inserir no próprio estudante
    estudante->adicionarLaboratorio(this, papel);

    return "Estudante associado com sucesso!";
}

void Laboratorio::desassociarEstudante(Estudante *estudante)
{
    if (estudante == nullptr)
    {
        std::cout << "Erro: Estudante inválido.\n";
        return;
    }

    int idEstudante = estudante->getId();

    // Verificar no BD se está associado
    try
    {
        Table associado = db->getTable("Associado");
        RowResult r = associado.select("estudante_id")
                          .where("estudante_id = :id AND laboratorio_id = :lab")
                          .bind("id", idEstudante)
                          .bind("lab", this->id)
                          .execute();

        if (r.count() == 0)
        {
            std::cout << "O estudante não está associado a este laboratório.\n";
            return;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Erro ao consultar BD: " << e.what() << "\n";
        return;
    }

    // Remover no BD
    try
    {
        db->getTable("Associado")
            .remove()
            .where("estudante_id = :id AND laboratorio_id = :lab")
            .bind("id", idEstudante)
            .bind("lab", this->id)
            .execute();
    }
    catch (const std::exception &e)
    {
        std::cout << "Erro ao remover no BD: " << e.what() << "\n";
        return;
    }

    // Remover em memória (graduação)
    bool removido = false;

    for (int i = 0; i < (int)estudantesGraduacao.size(); i++)
    {
        if (estudantesGraduacao[i]->getId() == idEstudante)
        {
            estudantesGraduacao.erase(estudantesGraduacao.begin() + i);
            removido = true;
            break;
        }
    }

    //  Remover em memória (pós), se ainda não removido
    if (!removido)
    {
        for (int i = 0; i < (int)estudantesPosGraduacao.size(); i++)
        {
            if (estudantesPosGraduacao[i]->getId() == idEstudante)
            {
                estudantesPosGraduacao.erase(estudantesPosGraduacao.begin() + i);
                break;
            }
        }
    }
    estudante->removerLaboratorio(this);
    // Mensagem final
    std::cout << "Estudante desassociado com sucesso!\n";
}

void Laboratorio::menuDesassociarEstudante()
{
    std::cout << "\n=== Estudantes associados ===\n";

    // pega o vetor explicitamente
    std::vector<Estudante *> associados = getEstudantes();

    if (associados.empty())
    {
        std::cout << "Nenhum estudante associado.\n";
        return;
    }

    // lista os estudantes associados
    for (int i = 0; i < (int)associados.size(); i++)
    {
        Estudante *e = associados[i];
        std::cout << "ID: " << e->getId()
                  << " | Nome: " << e->getNome() << "\n";
    }

    int id;
    std::cout << "\nID para desassociar: ";
    std::cin >> id;

    // procurar estudante pelo ID
    Estudante *escolhido = nullptr;
    for (int i = 0; i < (int)associados.size(); i++)
    {
        Estudante *e = associados[i];
        if (e->getId() == id)
        {
            escolhido = e;
            break;
        }
    }

    if (escolhido == nullptr)
    {
        std::cout << "Estudante não encontrado.\n";
        return;
    }

    // desassocia diretamente (sua função já imprime mensagens)
    desassociarEstudante(escolhido);
}

void Laboratorio::menuAssociarEstudante()
{
    std::cout << "\n=== Estudantes Disponíveis ===\n";

    // Coletar estudantes NÃO associados ao laboratório
    std::vector<Estudante *> estudantesDisponiveis;

    // Coletar todos os estudantes (Graduação)
    for (Estudante *e : Gestor::estudantes)
    {
        // Verifica se o estudante JÁ está associado a ESTE laboratório
        if (!estaAssociado(e))
        {
            estudantesDisponiveis.push_back(e);
        }
    }
    for (PosGraduacao *p : Gestor::posGraduandos)
    {
        // Verifica se o estudante JÁ está associado a ESTE laboratório
        if (!estaAssociado(p))
        {
            estudantesDisponiveis.push_back(p);
        }
    }

    if (estudantesDisponiveis.empty())
    {
        std::cout << "Todos os estudantes já estão associados a este laboratório.\n";
        return;
    }

    std::cout << "-------------------------------------------------------------------------------\n";
    std::cout << "| " << std::left << std::setw(8) << "ID"
              << " | " << std::setw(50) << "Nome"
              << " | " << std::setw(15) << "Nível"
              << " |\n";
    std::cout << "-------------------------------------------------------------------------------\n";

    for (Estudante *e : estudantesDisponiveis)
    {
        std::string nivel = (e->getNivelAcesso() == 3) ? "Graduação" : "Pós-Graduação";
        std::cout << "| " << std::left << std::setw(8) << e->getId()
                  << " | " << std::setw(50) << e->getNome()
                  << " | " << std::setw(15) << nivel
                  << " |\n";
    }
    std::cout << "-------------------------------------------------------------------------------\n";

    // 4. Solicitar ID para associação
    int id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer
    std::cout << "\nDigite o ID do estudante para associar (0 para cancelar): ";
    std::cin >> id;

    if (id == 0)
    {
        std::cout << "Operação cancelada.\n";
        return;
    }

    // 5. Encontrar o estudante selecionado
    Estudante *estudanteSelecionado = nullptr;
    for (Estudante *e : estudantesDisponiveis)
    {
        if (e->getId() == id)
        {
            estudanteSelecionado = e;
            break;
        }
    }

    if (estudanteSelecionado == nullptr)
    {
        std::cout << "Nenhum estudante disponível encontrado com o ID: " << id << ".\n";
        return;
    }
    // 6. Associar
    std::cout << associarEstudante(estudanteSelecionado) << "\n";
}

std::vector<int> Laboratorio::getIdsReagentesDoLaboratorio() const
{
    std::vector<int> idsReagentes;

    try
    {
        Table reagenteTable = db->getTable("Reagente");
        RowResult reagentes = reagenteTable
                                  .select("id")
                                  .where("laboratorio_id = :lab")
                                  .bind("lab", this->id)
                                  .execute();

        for (Row r : reagentes)
        {
            idsReagentes.push_back(r[0].get<int>());
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao consultar reagentes: " << err.what() << std::endl;
    }

    return idsReagentes;
}

void Laboratorio::adicionarEstudante(Estudante *estudante)
{
    if (estudante == nullptr)
    {
        std::cerr << "Erro: estudante inválido.\n";
        return;
    }
    int nivel = estudante->getNivelAcesso(); //
    // -------------------------------
    // Estudante de GRADUAÇÃO
    // -------------------------------
    if (nivel == 3)
    {
        // Verifica duplicata
        for (int i = 0; i < (int)estudantesGraduacao.size(); i++)
        {
            if (estudantesGraduacao[i] == estudante)
            {
                // std::cout << "Estudante de graduação já associado.\n";
                return;
            }
        }
        estudantesGraduacao.push_back(estudante);
        // std::cout << "DEBUG: " << estudante->getNome() << " (ID: " << estudante->getId() << ") adicionado como Graduacao." << std::endl;
        return;
    }
    if (nivel == 2)
    {
        // Converter ponteiro base → PosGraduacao*
        PosGraduacao *pos = dynamic_cast<PosGraduacao *>(estudante);
        if (pos)
        {
            // Verifica duplicata
            for (int i = 0; i < (int)estudantesPosGraduacao.size(); i++)
            {
                if (estudantesPosGraduacao[i] == pos)
                {
                    // std::cout << "Estudante de pós já associado.\n";
                    return;
                }
            }
            estudantesPosGraduacao.push_back(pos);
        }
        else
        {
            // fallback
            for (int i = 0; i < (int)estudantesGraduacao.size(); i++)
            {
                if (estudantesGraduacao[i] == estudante)
                {
                    // std::cout << "Estudante de graduação (fallback) já associado.\n";
                    return;
                }
            }
            estudantesGraduacao.push_back(estudante);
        }
        return;
    }

    // -------------------------------
    // Caso nível inválido
    // -------------------------------
    std::cout << "Nível desconhecido: " << nivel << "\n";
}

// criarLaboratorio: Cria um novo laboratório no banco de dados e na memória
Laboratorio *Laboratorio::criarLaboratorio(Schema *db, const std::string &nome, const std::string &departamento)
{
    if (!db)
    {
        std::cerr << "Erro: Conexão com banco de dados inválida.\n";
        return nullptr;
    }

    if (nome.empty())
    {
        std::cerr << "Erro: Nome do laboratório não pode ser vazio.\n";
        return nullptr;
    }

    try
    {
        // Verificar se já existe laboratório com mesmo nome
        Table laboratorioTable = db->getTable("Laboratorio");
        RowResult resExistente = laboratorioTable.select("id")
                                     .where("nome = :nome")
                                     .bind("nome", nome)
                                     .execute();

        if (resExistente.count() > 0)
        {
            std::cout << "Já existe um laboratório com o nome '" << nome << "'.\n";
            return nullptr;
        }

        // Inserir no banco de dados
        std::string dept = departamento.empty() ? "Não informado" : departamento;
        Result res = laboratorioTable.insert("nome", "departamento")
                         .values(nome, dept)
                         .execute();

        // Obter o ID gerado
        int novoId = res.getAutoIncrementValue();

        // Criar objeto em memória
        Laboratorio *novoLab = new Laboratorio(novoId, nome, dept, db);

        // Adicionar à lista estática de laboratórios
        laboratorios.push_back(novoLab);

        std::cout << "\nLaboratório criado com sucesso!\n";
        std::cout << "ID: " << novoId << "\n";
        std::cout << "Nome: " << nome << "\n";
        std::cout << "Departamento: " << dept << "\n";

        return novoLab;
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro MySQL ao criar laboratório: " << err.what() << "\n";
        return nullptr;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Erro ao criar laboratório: " << ex.what() << "\n";
        return nullptr;
    }
}

// editarLaboratorio
bool Laboratorio::editarLaboratorio(const std::string &novoNome, const std::string &novoDepartamento)
{
    if (!db)
    {
        std::cerr << "Erro: Laboratório não está conectado ao banco.\n";
        return false;
    }

    if (novoNome.empty())
    {
        std::cerr << "Erro: Novo nome não pode ser vazio.\n";
        return false;
    }

    try
    {
        // Verificar se já existe outro laboratório com o novo nome
        Table laboratorioTable = db->getTable("Laboratorio");
        RowResult resExistente = laboratorioTable.select("id")
                                     .where("nome = :nome AND id != :id")
                                     .bind("nome", novoNome)
                                     .bind("id", this->id)
                                     .execute();

        if (resExistente.count() > 0)
        {
            std::cout << "Já existe outro laboratório com o nome '" << novoNome << "'.\n";
            return false;
        }

        // Atualizar no banco de dados
        std::string dept = novoDepartamento.empty() ? "Não informado" : novoDepartamento;
        laboratorioTable.update()
            .set("nome", novoNome)
            .set("departamento", dept)
            .where("id = :id")
            .bind("id", this->id)
            .execute();

        // Atualizar em memória
        this->nome = novoNome;
        this->departamento = dept;

        std::cout << "\nLaboratório atualizado com sucesso!\n";
        std::cout << "ID: " << this->id << "\n";
        std::cout << "Novo nome: " << novoNome << "\n";
        std::cout << "Novo departamento: " << dept << "\n";

        return true;
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro MySQL ao editar laboratório: " << err.what() << "\n";
        return false;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Erro ao editar laboratório: " << ex.what() << "\n";
        return false;
    }
}

// getEstatisticas: Retorna estatísticas detalhadas do laboratório
std::string Laboratorio::getEstatisticas()
{
    std::stringstream estatisticas;

    estatisticas << "\n=== ESTATÍSTICAS DO LABORATÓRIO ===\n";
    estatisticas << "Nome: " << this->nome << "\n";
    estatisticas << "Departamento: " << this->departamento << "\n";
    estatisticas << "ID: " << this->id << "\n\n";

    // Estatísticas de usuários
    estatisticas << "--- USUÁRIOS ---\n";
    estatisticas << "Total de estudantes: " << this->getTotalEstudantes() << "\n";
    estatisticas << "  • Graduação: " << this->estudantesGraduacao.size() << "\n";
    estatisticas << "  • Pós-graduação: " << this->estudantesPosGraduacao.size() << "\n";
    estatisticas << "Total geral de usuários: " << this->getTotalUsuarios() << "\n\n";

    // Estatísticas de reagentes
    estatisticas << "--- REAGENTES ---\n";
    estatisticas << "Total de reagentes: " << this->reagentes.size() << "\n";

    if (!this->reagentes.empty())
    {
        int liquidos = 0;
        int solidos = 0;
        int restritos = 0;
        int criticos = 0;
        int vencidos = 0;
        int quantidadeTotal = 0;

        try
        {
            // Consultar banco para contar líquidos e sólidos
            Table liquidoTable = db->getTable("ReagenteLiquido");
            Table solidoTable = db->getTable("ReagenteSolido");

            std::vector<int> idsLiquidos;
            std::vector<int> idsSolidos;

            // Coletar IDs de líquidos
            RowResult resLiquidos = liquidoTable.select("id").execute();
            for (Row row : resLiquidos)
            {
                idsLiquidos.push_back(row[0].get<int>());
            }

            // Coletar IDs de sólidos
            RowResult resSolidos = solidoTable.select("id").execute();
            for (Row row : resSolidos)
            {
                idsSolidos.push_back(row[0].get<int>());
            }

            // Analisar cada reagente do laboratório
            for (Reagente *r : this->reagentes)
            {
                int reagenteId = r->getId();

                // Verificar se é líquido (está na tabela ReagenteLiquido)
                bool ehLiquido = false;
                for (int id : idsLiquidos)
                {
                    if (id == reagenteId)
                    {
                        ehLiquido = true;
                        liquidos++;
                        break;
                    }
                }

                // Se não é líquido, verificar se é sólido
                if (!ehLiquido)
                {
                    for (int id : idsSolidos)
                    {
                        if (id == reagenteId)
                        {
                            solidos++;
                            break;
                        }
                    }
                }

                // Contar restritos (nível de acesso 1)
                if (r->getNivelAcesso() == 1)
                    restritos++;

                // Contar críticos
                if (r->verificarNivelCritico())
                    criticos++;

                // Contar vencidos
                if (r->estaVencido())
                    vencidos++;

                // Somar quantidade total
                quantidadeTotal += r->getQuantidade();
            }

            estatisticas << "  • Líquidos: " << liquidos << "\n";
            estatisticas << "  • Sólidos: " << solidos << "\n";
            estatisticas << "  • Restritos: " << restritos << "\n";
            estatisticas << "  • Com estoque crítico: " << criticos << "\n";
            estatisticas << "  • Vencidos: " << vencidos << "\n";
            estatisticas << "  • Quantidade total em estoque: " << quantidadeTotal << " unidades\n";
        }
        catch (const mysqlx::Error &err)
        {
            estatisticas << "  • Erro ao consultar tipos de reagentes: " << err.what() << "\n";
        }
    }
    // Resumo de reagentes críticos
    std::vector<Reagente *> criticosLista = this->getReagentesCriticos();
    if (!criticosLista.empty())
    {
        estatisticas << "\n--- REAGENTES COM ESTOQUE CRÍTICO ---\n";
        for (Reagente *r : criticosLista)
        {
            estatisticas << "  • " << r->getNome() << ": "
                         << r->getQuantidade() << " " << r->getUnidadeMedida()
                         << " (mínimo: " << r->getQuantidadeCritica() << ")\n";
        }
    }

    // Resumo de reagentes vencidos
    std::vector<Reagente *> vencidosLista = this->getReagentesVencidos();
    if (!vencidosLista.empty())
    {
        estatisticas << "\n--- REAGENTES VENCIDOS ---\n";
        for (Reagente *r : vencidosLista)
        {
            estatisticas << "  • " << r->getNome() << ": válido até "
                         << r->getDataValidade() << "\n";
        }
    }

    // Data/hora atual
    time_t agora = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", localtime(&agora));
    estatisticas << "\nRelatório gerado em: " << buffer << "\n";
    estatisticas << "====================================\n";

    return estatisticas.str();
}

// Carrega retiradas do banco de dados para a memória
void Laboratorio::carregarRetiradasDoDB()
{
    try
    {
        Table retiradaTable = db->getTable("Retirada");
        std::vector<int> idsReagentes = getIdsReagentesDoLaboratorio();

        if (idsReagentes.empty())
            return;

        for (int reagenteId : idsReagentes)
        {
            RowResult retiradasRows = retiradaTable
                                          .select("*")
                                          .where("reagente_id = :reag_id")
                                          .bind("reag_id", reagenteId)
                                          .execute();

            for (Row row : retiradasRows)
            {
                int retiradaId = row[0].get<int>();
                int usuarioId = row[1].get<int>();

                float quantidade = 0.0f;
                if (!row[4].isNull())
                {
                    try
                    {
                        quantidade = static_cast<float>(row[4].get<double>());
                    }
                    catch (...)
                    {
                        try
                        {
                            quantidade = static_cast<float>(row[4].get<int>());
                        }
                        catch (...)
                        {
                            quantidade = 0.0f;
                        }
                    }
                }

                std::string dataHoraBD = row[3].get<std::string>(); // dataHoraRetirada

                Reagente *reagente = nullptr;
                for (Reagente *r : this->reagentes)
                {
                    if (r->getId() == reagenteId)
                    {
                        reagente = r;
                        break;
                    }
                }
                if (reagente == nullptr)
                    continue;

                Usuario *usuario = nullptr;
                for (Usuario *u : Gestor::usuariosCarregados)
                {
                    if (u->getId() == usuarioId)
                    {
                        usuario = u;
                        break;
                    }
                }
                if (usuario == nullptr)
                    continue;

                // Converter data
                std::string dataFormatada;
                if (!dataHoraBD.empty() && dataHoraBD.length() >= 10)
                {
                    std::string ano = dataHoraBD.substr(0, 4);
                    std::string mes = dataHoraBD.substr(5, 2);
                    std::string dia = dataHoraBD.substr(8, 2);

                    std::string hora = "00:00";
                    if (dataHoraBD.length() >= 16)
                    {
                        hora = dataHoraBD.substr(11, 5);
                    }

                    dataFormatada = dia + "/" + mes + "/" + ano + " " + hora;
                }
                else
                {
                    dataFormatada = dataHoraBD;
                }

                Retirada *novaRetirada = new Retirada(retiradaId, usuario, reagente, quantidade, dataFormatada);
                retiradas.push_back(novaRetirada);
            }
        }

        std::cout << "Carregadas " << retiradas.size() << " retiradas do laboratório.\n";
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao carregar retiradas do DB: " << err.what() << std::endl;
    }
}

// Retorna retiradas dos últimos 7 dias
std::vector<Retirada *> Laboratorio::getRetiradasUltimos7Dias()
{
    std::vector<Retirada *> retiradas7Dias;

    try
    {
        Table retiradaTable = db->getTable("Retirada");

        // Calcular data de 7 dias atrás
        time_t agora = time(nullptr);
        struct tm *tempoInfo = localtime(&agora);
        tempoInfo->tm_mday -= 7;
        mktime(tempoInfo);

        char dataLimite[20];
        strftime(dataLimite, sizeof(dataLimite), "%Y-%m-%d", tempoInfo);

        // Obter IDs dos reagentes deste laboratório
        std::vector<int> idsReagentes = getIdsReagentesDoLaboratorio();
        if (idsReagentes.empty())
        {
            return retiradas7Dias;
        }

        // Para cada reagente do laboratório, buscar retiradas dos últimos 7 dias
        for (int reagenteId : idsReagentes)
        {
            // Buscar retiradas deste reagente dos últimos 7 dias
            RowResult retiradasRows = retiradaTable
                                          .select("*")
                                          .where("reagente_id = :reag_id AND DATE(dataHoraRetirada) >= :data_limite")
                                          .orderBy("dataHoraRetirada DESC")
                                          .bind("reag_id", reagenteId)
                                          .bind("data_limite", std::string(dataLimite))
                                          .execute();

            for (Row row : retiradasRows)
            {
                int retiradaId = row[0].get<int>();
                int usuarioId = row[1].get<int>();

                // Converter quantidade
                float quantidade = 0.0f;
                if (!row[4].isNull())
                {
                    try
                    {
                        // Tentar como double
                        quantidade = static_cast<float>(row[4].get<double>());
                    }
                    catch (...)
                    {
                        // Se falhar, tentar como int
                        try
                        {
                            quantidade = static_cast<float>(row[4].get<int>());
                        }
                        catch (...)
                        {
                            quantidade = 0.0f;
                        }
                    }
                }

                std::string dataHoraBD = row[3].get<std::string>(); // Índice 3: dataHoraRetirada

                // Buscar reagente na memória
                Reagente *reagente = nullptr;
                for (Reagente *r : this->reagentes)
                {
                    if (r->getId() == reagenteId)
                    {
                        reagente = r;
                        break;
                    }
                }
                if (reagente == nullptr)
                    continue;

                // Buscar usuário nas listas carregadas
                Usuario *usuario = nullptr;
                for (Usuario *u : Gestor::usuariosCarregados)
                {
                    if (u->getId() == usuarioId)
                    {
                        usuario = u;
                        break;
                    }
                }
                if (usuario == nullptr)
                    continue;

                // Converter data do formato BD para "DD/MM/AAAA HH:MM"
                std::string dataFormatada;
                if (!dataHoraBD.empty() && dataHoraBD.length() >= 10)
                {
                    // Formato: "2024-12-25 14:30:00" → "25/12/2024 14:30"
                    std::string ano = dataHoraBD.substr(0, 4);
                    std::string mes = dataHoraBD.substr(5, 2);
                    std::string dia = dataHoraBD.substr(8, 2);

                    std::string hora = "00:00";
                    if (dataHoraBD.length() >= 16)
                    {
                        hora = dataHoraBD.substr(11, 5); // Pega "HH:MM"
                    }

                    dataFormatada = dia + "/" + mes + "/" + ano + " " + hora;
                }
                else
                {
                    dataFormatada = dataHoraBD;
                }

                // Criar retirada
                Retirada *novaRetirada = new Retirada(retiradaId, usuario, reagente, quantidade, dataFormatada);
                retiradas7Dias.push_back(novaRetirada);

                // Limitar a 20 resultados
                if (retiradas7Dias.size() >= 20)
                    break;
            }

            // Se já atingiu o limite, para de processar
            if (retiradas7Dias.size() >= 20)
                break;
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao buscar retiradas recentes: " << err.what() << std::endl;
    }

    return retiradas7Dias;
}
