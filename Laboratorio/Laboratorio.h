#ifndef LABORATORIO_H
#define LABORATORIO_H

#include <vector>
#include <string>
#include "../Usuario/usuario.h"
#include "../Reagente/reagente.h"
#include "../Retirada/Retirada.h"
#include "../Reagente/reagenteLiquido.h"
#include "../Reagente/reagenteSolido.h"


//Declaração das classes
class Usuario; class Retirada;
class Gestor;
class Estudante;
class PosGraduacao;

using namespace mysqlx;

class Laboratorio{
private:
    Schema *db;  //Ponteiro para o banco de dados
    int id;      //D único do laboratório
    std::string nome;  //Nome do laboratório
    std::string departamento;  //Departamento ao qual pertence

    //Vetores para armazenar os objetos em memória
    std::vector<Reagente *> reagentes;  //Lista de reagentes do lab
    std::vector<Usuario *> usuarios;    //Lista de usuários do lab
    std::vector<Estudante *> estudantesGraduacao;  //Estudantes de graduação
    std::vector<PosGraduacao *> estudantesPosGraduacao;  //Estudantes de pós
    std::vector<Retirada *> retiradas;  //Histórico de retiradas
    std::vector<Gestor *> gestores;     //Gestores do laboratório

    //Método para verificar se usuário tem retiradas pendentes
    bool verificarRetiradasPendentes(Usuario *usuario);

    //Método para carregar reagentes do banco para a memória
    void carregarReagentesDoDB();

public:
    //Construtor
    Laboratorio(int id, const std::string &nome, const std::string &departamento, Schema* db);

    //Destrutor
    ~Laboratorio();

    //Cadastra novo reagente no sistema (banco e memória)
    void cadastrarNovoReagente(
        std::string nome, std::string dataValidade, int quantidade,
        int quantidadeCritica, std::string local, int nivelAcesso,
        std::string unidade, std::string marca, std::string codRef,
        int tipo, double densidade, double volume,
        double massa, std::string estadoFisico
    );

    //Busca reagente pelo nome
    Reagente *buscarReagente(const std::string &nome);

    //Lista reagentes, com filtro por nome
    std::vector<Reagente *> listarReagentes(const std::string &filtroNome = "");

    //Lista reagentes por local de armazenamento
    std::vector<Reagente *> listarReagentesPorLocal(const std::string &local);

    //Registra uma nova retirada de reagente
    std::string registrarRetirada(Usuario *usuario, const std::string &nomeReagente, float quantidade);

    //Lista todas as retiradas de um usuário específico
    std::vector<Retirada *> listarRetiradasUsuario(Usuario *usuario);

    //Pega as 10 retiradas mais recentes
    std::vector<Retirada *> getHistoricoRecente();

    //Adiciona usuário ao laboratório
    std::string adicionarUsuario(Usuario *usuario);

    //Remove usuário do laboratório (se não tiver retiradas pendentes)
    std::string removerUsuario(Usuario *usuario);

    //Métodos de alerta
    std::vector<Reagente *> getReagentesCriticos();  // Estoque baixo
    std::vector<Reagente *> getReagentesVencidos();  // Data de validade passada
    std::string getAlertasGestor();  // Relatório consolidado para gestores
    std::string getEstatisticas();   // Estatísticas do laboratório

    //Getters
    std::vector<Usuario *> getVetorUsuarios() { return usuarios; }
    int getId() const { return id; }
    std::string getNome() const { return nome; }
    std::string getDepartamento() const { return departamento; }
    int getTotalReagentes() const { return reagentes.size(); }
    int getTotalGestores() const { return gestores.size(); }
    int getTotalEstudanteGraduacao() const { return estudantesGraduacao.size(); }
    int getTotalEstudantesPosGraducao() const { return estudantesPosGraduacao.size(); }
    int getTotalEstudantes() const { return (estudantesPosGraduacao.size() + estudantesGraduacao.size()); }
    int getTotalUsuarios() const { return usuarios.size(); }
    int getTotalRetiradas() const { return retiradas.size(); }

    //Representação em string do laboratório
    std::string toString() const;

    //lista de todos os laboratórios carregados
    static std::vector<Laboratorio *> laboratorios;

    //Métodos para gerenciar os laboratórios
    static std::vector<Laboratorio*> listarLaboratorios(Schema* db);  // Carrega do BD
    static void imprimirLaboratorios();  // Mostra lista formatada
    static void limparLaboratorios();    // Libera memória de todos os labs

    //Métodos para adicionar/remover pessoas
    void adicionarGestor(Gestor* gestor);
    void adicionarEstudante(Estudante* estudante);
    void removerEstudante(Estudante* estudante);
};

#endif
