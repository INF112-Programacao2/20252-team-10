#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <mysql-cppconn/mysqlx/xdevapi.h>
#include "../Laboratorio/Laboratorio.h"

class Laboratorio;
class Gestor;

using namespace mysqlx;

// Struct criada devido da classe ser abstrata devido o metodo puramente virtual,
//criou-se uma struct auxiliar para contribuir em fazer login do usuário
class Usuario{
    protected:
        Schema *db; // Ponteiro para o esquema do banco de dados
    private:
        std::string senha; // Senha do Usuario
        std::string nome;  // Nome Completo do Usuario
        std::string email; // E-amil do Usuario
        int id;            // ID do Usuario, por normalização, o ID inicial será -1, significa que não foi
        // cadastrado ou encontrado, assim que algumas dessas sentenças for verdadeiras, o BD retorna seu ID
        int nivelAcesso; // Corresponde ao Tipo de Usuário, devido que a depender possui influência no acesso dos Reagentes
        Gestor * cadastradoPor;
    public:
        // Construtor
        Usuario(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema *db); // Com os parametros
        Usuario();                                                                                    // Construtor sem parâmetros
        // Destrutor
        virtual ~Usuario() = default;

        // Gets
        // Retorna nome
        virtual std::string getNome() const;
        // Retorna o email
        virtual std::string getEmail() const;
        // Retorna o ID
        virtual int getId() const;
        // Retorna o nivel de acesso
        virtual int getNivelAcesso() const;
        // Retorna a senha de acesso do usuario
        virtual std::string getSenha() const;
        virtual Gestor* getCadastradoPor() const;

        // Sets
        // Atribui o nome do usuario
        virtual void setNome(const std::string nome);
        // Atribui o email do usuario
        virtual void setEmail(const std::string email);
        // Atribui o ID do usuario
        virtual void setId(int id);
        // Atribui o nivel de acesso do usuario
        virtual void setNivelAcesso(int nivelAcesso);
        // Atribui senha do usuario
        virtual void setSenha(const std::string senha);
        // Atribui o objeto gestor que cadastrou o usuário
        virtual void setCadastradoPor(Gestor * Gestor);

        // Demais
        // Imprime o perfil do Usuario
        virtual void detalharPerfil();
        // Esta funcão tenta autenticar um usuário no banco de dados, utilizando e-mail e senha
        // Tal funcao pertence a classe, assim poderá ser chamada diretamente sem ser instanciada por um objeto
        static bool fazerLogin(Schema *db, const std::string &email, const std::string &senha, Usuario * usuarioLogado);
        static bool validarEmail(const std::string& email);
        static bool validarSenha(const std::string& senha);
        //Consulta e lista os reagentes do banco de dados, respeitando o nivel de acesso do usuario
        virtual void consultarReagente();
        //Metodo virtual puro que obriga todas as classes filhas a implementarem
        virtual void acessarReagenteRestrito(int idReagente);
        virtual void menuPrincipal();
};


#endif
