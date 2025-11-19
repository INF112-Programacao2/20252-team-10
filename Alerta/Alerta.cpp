#include "../Alerta/Alerta.h"
#include <mysql-cppconn/mysqlx/xdevapi.h>
#include "../DatabaseConnection/databaseConnection.h"
#include <iostream>
#include <stdexcept>

// Construtor

Alerta::Alerta(unsigned int tipo, Reagente *reagenteEmAlerta)
{
    this->_tipo = tipo;
    this->_reagenteEmAlerta = reagenteEmAlerta;
    try
    {
        this->situacaoPorTipo(); // preenche a situacao baseado no tipo de alerta
    }
    catch (std::runtime_error &e)
    {
        throw;
    }

    this->setDataEmissao();
}

// Destrutor

Alerta::~Alerta() {}

// Gets

unsigned int Alerta::getId()
{
    return this->_id;
}

std::string Alerta::getDataEmissao()
{
    return this->_dataEmissao;
}

unsigned int Alerta::getTipo()
{
    return this->_tipo;
}

std::string Alerta::getSituacao()
{
    return this->_situacao;
}

// Sets

void Alerta::setId(int id)
{
    this->_id = id;
}

void Alerta::setDataEmissao()
{
    this->_tempoBruto = std::time(nullptr);
    this->_tempoInfo = std::localtime(&_tempoBruto);
    this->_dataEmissao = std::asctime(_tempoInfo);
}

void Alerta::setTipo(int tipo)
{
    this->_tipo = tipo;
}

void Alerta::setSituacao(std::string situacao)
{
    this->_situacao = situacao;
}

// Outros métodos

void Alerta::situacaoPorTipo()
{
    std::string situacao;
    switch (_tipo)
    {
        case 1: // tipo 1 = reagente em quantidade critica
        situacao = "[ALERTA]: O reagente " + _reagenteEmAlerta->getNome() + " atingiu quantidade crítica!\n";
        break;
        case 2:
        situacao = "[ALERTA]: o reagente " + _reagenteEmAlerta->getNome() + " está próximo do vencimento!\n";
        break;
        default:
        throw std::runtime_error("Alerta criado com tipo inválido\n");
        break;
    }
    this->_situacao = situacao;
}

void Alerta::notificar()
{
    if (!(this->_situacao.empty()))
    {
        std::cout << this->_dataEmissao << " : " << this->_situacao;
    }
    else
    {
        throw std::runtime_error("Alerta com string situação vazia\n");
    }
}

// void Alerta::adicionarAlertaBD(){
//     DatabaseConnection conexaoDB;
//     Session* session = nullptr;
//     Schema *db = nullptr;
//     try{
//         //Estabelece a conexão com o banco de dados
//         session = conexaoDB.getSession(); // Obtém a sessão de conexão
//         db = conexaoDB.getSchema(); // Obtém o esquema do banco de dados

//         // Verifica se a conexão e o esquema foram inicializados corretamente
//         if (!session || !db) {
//             throw std::runtime_error("Falha ao inicializar a conexão com o banco de dados.");
//         }} catch(std::runtime_error &e){
//             e.what();
//         }

//         Table tabelaAlerta = db->getTable("Alerta");
//         Result i = tabelaAlerta.insert("gestor_id", "reagente_id", "dataHoraEmissao", "tipo", "situacao")
//         .values(1, this->_reagenteEmAlerta->getId(), "Tipo teste", "situacao teste").execute();
// }
