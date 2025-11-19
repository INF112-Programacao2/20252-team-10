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

        static Usuario** usuariosCarregados;
        static int quantidadeUsuarios;
        static int capacidadeUsuarios;

        static Gestor** gestores;
        static int quantidadeGestores;
        static int capacidadeGestores;

        static Estudante** estudantes;
        static int quantidadeEstudantes;
        static int capacidadeEstudantes;

        static PosGraduacao** posGraduandos;
        static int quantidadePos;
        static int capacidadePos;

        // Destrutor
        ~Gestor();

        //Demais funções
        void cadastrarUsuario();
        void deletarUsuario();
        void listarUsuarios();
        void associarLaboratorio();
        void associarEstudanteAoLaboratorio(Estudante* estudante, int idLaboratorio, const std::string& papel);
        void carregarUsuarios();

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
};

#endif
