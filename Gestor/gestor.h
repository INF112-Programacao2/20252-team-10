#ifndef CLASSE_GESTOR
#define CLASSE_GESTOR
#include <mysql-cppconn/mysqlx/xdevapi.h>
#include "../Usuario/usuario.h"
#include "../Laboratorio/Laboratorio.h"
#include <string>
#include "../Reagente/reagente.h"
#include "../PosGraduacao/posgraduacao.h"

using namespace mysqlx;

class Gestor : public Usuario
{
private:
    Laboratorio *laboratorio; // O laboratorio que este Gestor gerencia
public:
    // Construtor
    Gestor(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema *db);

    // Vetores estáticos dos usuários
    static std::vector<Usuario *> usuariosCarregados;
    static std::vector<Gestor *> gestores;
    static std::vector<Estudante *> estudantes;
    static std::vector<PosGraduacao *> posGraduandos;
    // Destrutor
    ~Gestor();

    // Getters
    Laboratorio *getLaboratorio();

    // Demais funções
    void cadastrarUsuario();
    void deletarUsuario();
    void listarUsuarios();
    void associarLaboratorio();
    void associarEstudanteAoLaboratorio(Estudante *estudante, int idLaboratorio, const std::string &papel);
    static void carregarUsuarios(Schema *db);
    void retirarReagente();
    void historicoRetiradas();
    void sairLaboratorio();
    void desassociarEstudantes();
    void listarReagentesDoLaboratorio();
    void editarReagente();
    void excluirReagente();
    void filtrarReagentes();

    // Menu principal de gerenciamento do laboratório
    void gerenciarLaboratorio();

    // sets
    void setLaboratorio(Laboratorio *lab);

    // cadastra um novo reagente no laboratorio gerenciado por este gestor
    void cadastrarReagente();

    // O Gestor tem acesso total
    void acessarReagentesAlerta();
    void listarReagentesRestritos();
    void menuReagentesRestritos();
    void acessarReagenteRestrito(int idReagente) override;
    // Verifica se o usuário está associado a algum laboratório
    bool estaAssociado() const;
    void listarEstudantes();
    void associarEstudantes();
    void deletarGestor();
    void deletarEstudante();
    static Gestor *getGestorById(int id);
    static void carregarAssociacoes(Schema *db);

    static void limparUsuarios();
};

#endif
