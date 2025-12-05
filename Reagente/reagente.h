#ifndef Reagente_H
#define Reagente_H

#include <string>
#include <vector>

// Classe que representa um Reagente quimico no sistema
class Reagente
{
private:
    // Atributos privados da classe
    std::string nome;
    std::string dataValidade;
    int quantidade;
    int quantidadeCritica;
    std::string localArmazenamento;
    int nivelAcesso;
    std::string unidadeMedida;
    std::string marca;
    std::string codigoReferencia;
    int id;

public:
    // Construtor: Inicializa o objeto com todos os atributos informados
    Reagente(int id, std::string nome, std::string dataValidade, int quantidade,
             int quantidadeCritica, std::string localArmazenamento, int nivelAcesso,
             std::string unidadeMedida, std::string marca, std::string codigoReferencia);
    // Construtor vazio: Utilizado para criar o objeto buscando dados do banco
    Reagente();

    // Destrutor virtual, garante que o destrutor da classe filha seja chamado primeiro
    virtual ~Reagente();

    // Gets - Retornam os valores atuais dos atributos
    std::string getNome();
    std::string getDataValidade();
    int getQuantidade();
    int getQuantidadeCritica();
    std::string getLocalArmazenamento();
    int getNivelAcesso();
    std::string getUnidadeMedida();
    std::string getMarca();
    std::string getCodigoReferencia();
    int getId();

    // Sets
    // Alteram os valores dos atributos
    void setNome(std::string nome);
    void setDataValidade(std::string dataValidade);
    void setQuantidade(int quantidade);
    void setQuantidadeCritica(int quantidadeCritica);
    void setLocalArmazenamento(std::string localArmazenamento);
    void setNivelAcesso(int nivelAcesso);
    void setUnidadeMedida(std::string unidadeMedida);
    void setMarca(std::string marca);
    void setCodigoReferencia(std::string codigoReferencia);

    // Outros metodos
    bool estaVencido();
    void acionarAlerta(unsigned int tipo);
    bool verificarNivelCritico();
};

#endif
