#ifndef LABORATORIO_H
#define LABORATORIO_H

#include <vector>
#include <string>
#include "../Usuario/usuario.h"
#include "../Reagente/reagente.h"
#include "../Retirada/Retirada.h"
#include "../Reagente/reagenteLiquido.h"
#include "../Reagente/reagenteSolido.h"

class Usuario; class Retirada;


using namespace mysqlx;

// Para retirar a depedência circular
class Gestor; 
class Estudante;
class PosGraduacao;

using namespace mysqlx;

class Laboratorio{
private:
    Schema *db; // Conexão com o banco de dados
    int id;
    std::string nome;
    std::string departamento;
    std::vector<Reagente *> reagentes;
    std::vector<Estudante *> estudantesGraduacao;
    std::vector<PosGraduacao *> estudantesPosGraduacao;
    std::vector<Retirada *> retiradas;
    std::vector<Gestor *> gestores;


    // Metodos privados
    bool verificarRetiradasPendentes(Usuario *usuario);
    void carregarReagentesDoDB(); // Metodo para carregar dados do DB

public:
    // Constructor e Destructor
    Laboratorio(int id, const std::string &nome, const std::string &departamento, Schema* db);
    ~Laboratorio();

    // // Gerenciamento de Reagentes
    // std::string adicionarReagente(Reagente *reagente);
    // Reagente *buscarReagente(const std::string &nome);
    // std::vector<Reagente *> listarReagentes(const std::string &filtroNome = "");
    // std::vector<Reagente *> listarReagentesPorLocal(const std::string &local);
    //Metodo para criar e salvar o reagente
    void cadastrarNovoReagente(
        std::string nome, std::string dataValidade, int quantidade,
        int quantidadeCritica, std::string local, int nivelAcesso,
        std::string unidade, std::string marca, std::string codRef,
        int tipo, double densidade, double volume,
        double massa, std::string estadoFisico
    );

    // Metodo para Buscar Reagentes
    Reagente *buscarReagente(const std::string &nome);
    std::vector<Reagente *> listarReagentes(const std::string &filtroNome = "");
    std::vector<Reagente *> listarReagentesPorLocal(const std::string &local);

    // // Gerenciamento de Retiradas
    // std::string registrarRetirada(Usuario *usuario, const std::string &nomeReagente, float quantidade);
    // std::vector<Retirada *> listarRetiradasUsuario(Usuario *usuario);
    // std::vector<Retirada *> getHistoricoRecente();

    // // Gerenciamento de Usuarios
    // std::string adicionarUsuario(Usuario *usuario);
    // std::string removerUsuario(Usuario *usuario);

    // // Alertas e Monitoramento
    // std::vector<Reagente *> getReagentesCriticos();
    // std::vector<Reagente *> getReagentesVencidos();
    // std::vector<Reagente *> getReagentesProximoVencimento(int dias = 15);
    // std::string getAlertasGestor();
    // Alertas e Monitoramento
    std::vector<Reagente *> getReagentesCriticos();
    std::vector<Reagente *> getReagentesVencidos();
    // std::vector<Reagente *> getReagentesProximoVencimento(int dias = 15);
    std::string getAlertasGestor();

    // // Informacoes do Laboratorio
    // std::string getEstatisticas();

    // Getters
    std::vector<Usuario *> getVetorUsuarios() { return usuarios; }
    int getId() const { return id; }
    std::string getNome() const { return nome; }
    std::string getDepartamento() const { return departamento; }
    int getTotalReagentes() const { return reagentes.size(); }
    int getTotalGestores() const { return gestores.size(); }
    int getTotalEstudanteGraduacao() const { return estudantesGraduacao.size(); }
    int getTotalEstudantesPosGraducao() const { return estudantesPosGraduacao.size(); }
    int getTotalEstudantes() const { return (estudantesPosGraduacao.size() + estudantesGraduacao.size()); }
    int getTotalUsuarios() const { return (getTotalEstudantes() + getTotalGestores()); }
    int getTotalRetiradas() const { return retiradas.size(); }

    // toString
    std::string toString() const;

    
    // Armazena na classe, os laboratórios instanciados
    static std::vector<Laboratorio *> laboratorios;
    //Lista todos os laboratorios instanciados
    static std::vector<Laboratorio*> listarLaboratorios(Schema* db);
    //Imprime os dados dos laboratórios instanciados
    static void imprimirLaboratorios();
    //Adicionar gestor
    void adicionarGestor(Gestor* gestor);
   // liberar todos os laboratórios do vetor
    static void limparLaboratorios();
    void adicionarEstudante(Estudante* estudante);
    void removerEstudante(Estudante* estudante);
};

#endif
