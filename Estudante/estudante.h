#ifndef CLASSE_ESTUDANTE
#define CLASSE_ESTUDANTE
#include "../Usuario/usuario.h"
#include <string>
#include <mysql-cppconn/mysqlx/xdevapi.h>

class Estudante : public Usuario{
    private:    
        std::string matricula;
        std::string curso;
        std::string nivel;
    public:
        // Construtor
        Estudante(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema* db,
                std::string matricula, std::string curso, std::string nivel);
        // Destrutor
        ~Estudante();

        //Gets
        std::string getMatricula();
        std::string getCurso();
        std::string getNivel();

        //Sets
        void setMatricula(std::string matricula);
        void setCurso(std::string curso);
        void setNivel(std::string nivel);

};
#endif