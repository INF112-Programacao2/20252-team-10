#ifndef CLASSE_ESTUDANTE
#define CLASSE_ESTUDANTE
#include "../Usuario/usuario.h"
#include <string>
#include <mysql-cppconn/mysqlx/xdevapi.h>
#include <vector>

// Evitar dependência circular
class Laboratorio;
class PosGraduacao;
class Estudante : public Usuario{
    protected:
        std::string matricula;
        std::string curso;
        std::string nivel;
        std::vector<std::pair<Laboratorio*, std::string>> laboratorios;
        // Array de laboratório em que os etudnate esta associado

    public:
        //Array de estudantes de graduacao
        static std::vector<Estudante*> estudantesGraduacao;
        //Array de estudantes de Pos Graduacao
        static std::vector<PosGraduacao*> estudantesPosGraduacao;
        // Construtor
        Estudante(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema *db,
                    std::string matricula, std::string curso, std::string nivel);
        // Destrutor
        virtual ~Estudante();

        // Gets
        std::string getMatricula() const;
        std::string getCurso() const;
        std::string getNivel() const;
        std::vector<std::pair<Laboratorio*, std::string>> getLaboratorios() const;

        // Sets
        void setMatricula(const std::string & matricula);
        void setCurso(const std::string & curso);
        void setNivel(const std::string & nivel);

        // Demais metodos
        //Adicionar laboratorio a estudante
        void adicionarLaboratorio(Laboratorio* laboratorio, const std::string& papel);
        // Remove o objeto no laboratorio no objeto estudante
        void removerLaboratorioObjeto(Laboratorio* laboratorio);
        // Remover laboratorio do estduante
        void removerLaboratorio(Laboratorio* laboratorio, Schema* db);
        // Associa estudante ao laboratorio
        void associarLaboratorio(Laboratorio* laboratorio, const std::string& papel);

        void acessarLaboratorios();
        void consultarEstoqueLaboratorio(Laboratorio* lab);
        void consultarEstoque();

        void retirarReagente();
        // esta funçao checa se o nivel de acesso do estudante e suficiente.
        void acessarReagenteRestrito(int idReagente) override;
};

#endif