#ifndef CLASSE_GESTOR
#define CLASSE_GESTOR

#include "../Usuario/usuario.h"
#include "../Laboratorio/Laboratorio.h"
#include <string>
#include <mysql-cppconn/mysqlx/xdevapi.h>
#include "../Reagente/reagente.h"
#include "../PosGraduacao/posgraduacao.h"

using namespace mysqlx;

class Gestor : public Usuario {
private:
    Laboratorio * laboratorio; // O laboratorio que este Gestor gerencia
    public:
        //Construtor
        Gestor(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema* db);

        static std::vector<Usuario*>usuariosCarregados;
        static std::vector<Gestor*>gestores;
        static std::vector<Estudante*>estudantes;
        static std::vector<PosGraduacao*>posGraduandos;

    // static PosGraduacao** posGraduandos;
    // static int quantidadePos;
    // static int capacidadePos;

    ~Gestor();


        //Menu principal de gerenciamento do laboratório
    //Getters
    Laboratorio* getLaboratorio();

    //Demais funções
    void cadastrarUsuario();
    void deletarUsuario();
    void listarUsuarios();
    void associarLaboratorio();
    void associarEstudanteAoLaboratorio(Estudante* estudante, int idLaboratorio, const std::string& papel);
    void carregarUsuarios();
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

    //sets
    void setLaboratorio(Laboratorio* lab);

    //cadastra um novo reagente no laboratorio gerenciado por este gestor
    void cadastrarReagente();

    //O Gestor tem acesso total
    void acessarReagentesAlerta();
    void listarReagentesRestritos();
    void menuReagentesRestritos();
    void acessarReagenteRestrito(int idReagente) override;
    bool estaAssociado() const;
    void listarEstudantes();
    void associarEstudantes();
    void cadastrarEstudante();
    void cadastrarGestor();
    void deletarGestor();
    void deletarEstudante();



};

#endif
