#ifndef LABORATORIO_H
#define LABORATORIO_H

#include <vector>
#include <string>
#include "../Usuario/usuario.h"
#include "../Reagente/reagente.h"
#include "../Retirada/Retirada.h"
#include "../Reagente/reagenteLiquido.h"
#include "../Reagente/reagenteSolido.h"
#include "../Alerta/Alerta.h"
#include "../Alerta/AlertaQuantidade.h"
#include "../Alerta/AlertaValidade.h"
#include <mysql-cppconn/mysqlx/xdevapi.h>

// Declarações antecipadas
class Usuario;
class Retirada;
class Gestor;
class Estudante;
class PosGraduacao;

using namespace mysqlx;

 // CLASSE LABORATORIO
 // Representa um laboratório no sistema
 // Gerencia reagentes, usuários, retiradas e alertas
class Laboratorio {
private:
    Schema *db;               // Conexão com o banco de dados
    int id;                   // ID único do laboratório
    std::string nome;         // Nome do laboratório
    std::string departamento; // Departamento ao qual pertence

    // Vetores de objetos em memória
    std::vector<Reagente *> reagentes;                  // Lista de reagentes do laboratório
    std::vector<Estudante *> estudantesGraduacao;       // Estudantes de graduação associados
    std::vector<PosGraduacao *> estudantesPosGraduacao; // Estudantes de pós-graduação associados
    std::vector<Retirada *> retiradas;                  // Histórico de retiradas
    std::vector<Gestor *> gestores;                     // Gestores responsáveis pelo laboratório
    std::vector<Alerta *> alertas;                      // Alertas ativos (validade, estoque crítico)

    // Métodos internos
    bool verificarRetiradasPendentes(Usuario *usuario); // Verifica se usuário tem retiradas não finalizadas
    void carregarReagentesDoDB();                       // Carrega reagentes do banco para memória


public:
    // Construtor e destrutor
    Laboratorio(int id, const std::string &nome, const std::string &departamento, Schema *db);
    ~Laboratorio();

    // Métodos para criação e edição de laboratórios
    static Laboratorio* criarLaboratorio(Schema* db, const std::string& nome, const std::string& departamento);
    bool editarLaboratorio(const std::string& novoNome, const std::string& novoDepartamento);

    // Gerenciamento de reagentes
    void cadastrarNovoReagente(int id, std::string nome, std::string dataValidade, int quantidade,
                               int quantidadeCritica, std::string local, int nivelAcesso,
                               std::string unidade, std::string marca, std::string codRef,
                               int tipo, double densidade, double volume,
                               double massa, std::string estadoFisico);
    Reagente *buscarReagente(const std::string &nome);
    std::vector<Reagente *> listarReagentes(const std::string &filtroNome = "");
    std::vector<Reagente *> listarReagentesPorLocal(const std::string &local);
    void removerReagenteDaMemoria(int idReagente);

    // Gerenciamento de retiradas
    std::string registrarRetirada(Usuario *usuario, const std::string &nomeReagente, float quantidade);
    std::vector<Retirada *> listarRetiradasUsuario(Usuario *usuario);
    std::vector<Retirada *> getHistoricoRecente();

    // Gerenciamento de usuários
    std::string adicionarUsuario(Usuario *usuario);
    std::string removerUsuario(Usuario *usuario);
    void adicionarGestor(Gestor *gestor);
    void removerGestor(int id);
    void menuRemoverGestor();
    void adicionarEstudante(Estudante *estudante);
    void removerEstudante(Estudante *estudante);
    void imprimirEstudantes(const std::vector<Estudante *> &estudantes);
    std::vector<Estudante *> getEstudantes();
    void listarEstudantes();
    std::string associarEstudante(Estudante *estudante);
    void desassociarEstudante(Estudante *estudante);
    void menuDesassociarEstudante();
    void menuAssociarEstudante();
    bool estaAssociado(Estudante *estudante);

    // Alertas e monitoramento
    void carregarAlertasDB();
    std::vector<Reagente *> getReagentesCriticos();
    std::vector<Reagente *> getReagentesVencidos();
    void getAlertasGestor();
    std::string getEstatisticas();

    // Getters
    std::vector<Usuario *> getUsuarios();
    int getId() const { return id; }
    std::string getNome() { return nome; }
    std::string getDepartamento() const { return departamento; }
    int getTotalReagentes() const { return reagentes.size(); }
    int getTotalGestores() const { return gestores.size(); }
    int getTotalEstudanteGraduacao() const { return estudantesGraduacao.size(); }
    int getTotalEstudantesPosGraducao() const { return estudantesPosGraduacao.size(); }
    int getTotalEstudantes() const { return (estudantesPosGraduacao.size() + estudantesGraduacao.size()); }
    int getTotalUsuarios() const { return getTotalGestores() + getTotalEstudantes(); }
    int getTotalRetiradas() const { return retiradas.size(); }
    std::vector<int> getIdsReagentesDoLaboratorio() const; // Retorna IDs de todos os reagentes do lab

    // Métodos estáticos para gerenciamento global
    static std::vector<Laboratorio *> laboratorios;      // Lista de todos os laboratórios carregados
    static std::vector<Laboratorio *> listarLaboratorios(Schema *db); // Carrega labs do banco
    static void imprimirLaboratorios();                  // Exibe lista formatada de laboratórios
    static void limparLaboratorios();                    // Libera memória de todos os laboratórios

    std::string toString() const; // Representação em string do laboratório
};

#endif
