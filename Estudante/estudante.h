#ifndef CLASSE_ESTUDANTE
#define CLASSE_ESTUDANTE

#include "../Usuario/usuario.h"
#include <string>
#include <mysql-cppconn/mysqlx/xdevapi.h>
#include <vector>

// Declaração antecipada para evitar dependência circular
class Laboratorio;
class PosGraduacao;

// CLASSE ESTUDANTE
// Representa um estudante (graduação ou pós-graduação) no sistema.
// Herda de Usuario e gerencia associações a laboratórios.

class Estudante : public Usuario
{
protected:
    std::string matricula;                                           // Matrícula do estudante
    std::string curso;                                               // Curso (ex: Engenharia Química)
    std::string nivel;                                               // Nível acadêmico (ex: Graduação, Mestrado)
    std::vector<std::pair<Laboratorio *, std::string>> laboratorios; // Lista de laboratórios associados e seus respectivos papéis

public:
    // Construtor
    Estudante(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema *db,
              std::string matricula, std::string curso, std::string nivel);

    // Destrutor
    virtual ~Estudante();

    // Getters
    std::string getMatricula() const;
    std::string getCurso() const;
    std::string getNivel() const;
    std::vector<std::pair<Laboratorio *, std::string>> getLaboratorios() const;

    // Setters
    void setMatricula(const std::string &matricula);
    void setCurso(const std::string &curso);
    void setNivel(const std::string &nivel);

    // Métodos de gerenciamento de laboratórios
    void adicionarLaboratorio(Laboratorio *laboratorio, const std::string &papel); // Adiciona laboratório à lista do estudante
    void removerLaboratorioObjeto(Laboratorio *laboratorio);                       // Remove laboratório apenas da memória do objeto
    void removerLaboratorio(Laboratorio *laboratorio);                             // Remove laboratório (banco + memória)
    void associarLaboratorio(Laboratorio *laboratorio, const std::string &papel);  // Associa estudante a laboratório (banco + memória)
    void acessarLaboratorios();                                                    // Menu principal do estudante
    void consultarEstoqueLaboratorio(Laboratorio *lab);                            // Lista reagentes de um laboratório específico
    void consultarEstoque();                                                       // Lista reagentes de todos os laboratórios associados
    void retirarReagente();                                                        // Realiza retirada de reagente

    //  Controle de acesso a reagentes restritos
    void acessarReagenteRestrito(int idReagente) override; // Verifica permissão antes de exibir dados
};

#endif
