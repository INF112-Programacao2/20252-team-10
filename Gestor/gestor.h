#ifndef CLASSE_GESTOR
#define CLASSE_GESTOR

#include <mysql-cppconn/mysqlx/xdevapi.h>
#include "../Usuario/usuario.h"
#include "../Laboratorio/Laboratorio.h"
#include <string>
#include "../Reagente/reagente.h"
#include "../PosGraduacao/posgraduacao.h"

using namespace mysqlx;

 // CLASSE GESTOR
 // Representa um gestor no sistema (nível de acesso 1).
 // Herda de Usuario e gerencia laboratórios, usuários e reagentes.

class Gestor : public Usuario {
private:
    Laboratorio *laboratorio; // Laboratório que este gestor gerencia
public:
    // Construtor
    Gestor(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema *db);

    // Vetores estáticos para armazenamento global de usuários
    static std::vector<Usuario *> usuariosCarregados;  // Todos os usuários
    static std::vector<Gestor *> gestores;             // Apenas gestores
    static std::vector<Estudante *> estudantes;        // Apenas estudantes (graduação)
    static std::vector<PosGraduacao *> posGraduandos;  // Apenas pós-graduandos

    ~Gestor(); // Destrutor

    // Getters
    Laboratorio *getLaboratorio();
    
    // Gerenciamento de usuários
    void cadastrarUsuario();                         // Cadastra novo usuário (gestor/estudante)
    void deletarUsuario();                           // Remove usuário do sistema
    void listarUsuarios();                           // Lista todos os usuários cadastrados
    static void carregarUsuarios(Schema *db);        // Carrega usuários do banco para memória
    static void carregarAssociacoes(Schema *db);     // Carrega associações (gestor-lab, estudante-lab)
    static void limparUsuarios();                    // Libera memória dos vetores de usuários
    static Gestor *getGestorById(int id);            // Busca gestor pelo ID
    static Estudante* getEstudanteById(int id);      // Busca estudante pelo ID

    // Gerenciamento de laboratórios
    void associarLaboratorio();                      // Associa gestor a um laboratório
    void sairLaboratorio();                          // Remove associação com laboratório
    bool estaAssociado() const;                      // Verifica se gestor está associado a um lab
    void gerenciarLaboratorio();                     // Menu principal de gerenciamento do lab
    void setLaboratorio(Laboratorio *lab);           // Define laboratório manualmente
    void criarLaboratorio();                         // criar novo lab
    void editarLaboratorio();                        // editar lab existente

    // Gerenciamento de estudantes no laboratório
    void associarEstudanteAoLaboratorio(Estudante *estudante, int idLaboratorio, const std::string &papel);
    void desassociarEstudantes();                    // funçao para desassociar estudante
    void listarEstudantes();                         // Lista estudantes do laboratório

    // Gerenciamento de reagentes
    void cadastrarReagente();                        // Cadastra novo reagente no laboratório
    void listarReagentesDoLaboratorio();             // Lista reagentes do laboratório
    void editarReagente();                           // Edita dados de um reagente
    void excluirReagente();                          // Remove reagente do sistema
    void filtrarReagentes();                         // Filtra reagentes por categoria/restrição
    void retirarReagente();                          // Realiza retirada de reagente
    void historicoRetiradas();                       // Exibe histórico de retiradas recentes

<<<<<<< HEAD
    // Controle de acesso a reagentes restritos
    void acessarReagentesAlerta();                          // Exibe reagentes em alerta (crítico/vencido)
    void listarReagentesRestritos();                        // Lista apenas reagentes restritos
    void menuReagentesRestritos();                          // Menu de gestão de reagentes restritos
    void acessarReagenteRestrito(int idReagente) override;  // Acesso total (sem verificação de nível)
=======
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
    static Gestor *getGestorById(int id);
    static Estudante* getEstudanteById(int id);
    static void carregarAssociacoes(Schema *db);

    static void limparUsuarios();
>>>>>>> a6a11f496ce95241ae2921c093c3f19909f84fd7
};

#endif