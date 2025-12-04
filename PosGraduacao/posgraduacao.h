#ifndef CLASSE_POSGRADUACAO
#define CLASSE_POSGRADUACAO

#include "../Estudante/estudante.h"
#include <string>
using namespace mysqlx;


 // CLASSE POSGRADUACAO
 // Representa um estudante de pós-graduação
 // Herda de Estudante, mantendo a hierarquia existente
 
class PosGraduacao : public Estudante {
public:
    PosGraduacao(); // Construtor vazio

    // Construtor
    PosGraduacao(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema* db,
                 std::string matricula, std::string curso, std::string nivel);

    // Destrutor
    ~PosGraduacao(); 
};

#endif