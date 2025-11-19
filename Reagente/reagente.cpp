#include "reagente.h"
#include "../DatabaseConnection/databaseConnection.h"
#include "../Alerta/Alerta.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

//Construtor
Reagente::Reagente(int id, std::string nome, std::string dataValidade, int quantidade,
    int quantidadeCritica, std::string localArmazenamento, int nivelAcesso,
    std::string unidadeMedida, std::string marca, std::string codigoReferencia)
    {

        this->id = id;
        this->nome = nome;
        this->dataValidade = dataValidade;
        this->quantidade = quantidade;
        this->quantidadeCritica = quantidadeCritica;
        this->localArmazenamento = localArmazenamento;
        this->nivelAcesso = nivelAcesso;
        this->unidadeMedida = unidadeMedida;
        this->marca = marca;
        this->codigoReferencia = codigoReferencia;
    }

    // construtor sem parametros, para criar o objeto a partir do banco
    Reagente::Reagente(){

        DatabaseConnection conexaoDB;
        Session* session = nullptr;
        Schema *db = nullptr;

        try{
            //Estabelece a conexão com o banco de dados
            session = conexaoDB.getSession(); // Obtém a sessão de conexão
            db = conexaoDB.getSchema(); // Obtém o esquema do banco de dados

            // Verifica se a conexão e o esquema foram inicializados corretamente
            if (!session || !db) {
                throw std::runtime_error("Falha ao inicializar a conexão com o banco de dados.");
            }} catch(std::runtime_error &e){
                e.what();
            }
            Table reagentesTabela = db->getTable("Reagente");

            std::cout << "Use o código de referência para encontrar reagente no banco de dados: " << std::endl;
            std::string cod;
            std::cin.ignore(1);
            std::getline(std::cin, cod);



            SqlResult result = session->sql(
                "SELECT *, DATE_FORMAT(validade, '%Y-%m-%d') as data_formatada FROM LabUFV.Reagente WHERE referencia = ?").bind(cod).execute(); // busca o reagente pela referencia, e deixa a validade em formato legivel

                if (result.count() > 0) { // se a referencia for encontrada no banco
                    Row row = result.fetchOne();

                    this->id = row[0].get<int>();
                    this->nome = row[2].get<std::string>();
                    this->quantidade = row[3].get<int>();
                    this->quantidadeCritica = row[4].get<int>();
                    this->codigoReferencia = row[5].get<std::string>();
                    this->marca = row[6].get<std::string>();
                    this->localArmazenamento = row[7].get<std::string>();
                    this->nivelAcesso = row[8].get<int>();
                    this->unidadeMedida = row[9].get<std::string>();
                    // a validade formatada vem na ultima linha
                    this->dataValidade = row[row.colCount() - 1].get<std::string>();
                    std::cout << "Data de Validade: " << this->dataValidade << std::endl;


                }
            }

            //Destrutor
            Reagente::~Reagente()
            {

            }



            bool Reagente::verificarNivelCritico(){
                if(this->quantidade <= this->quantidadeCritica){
                    return true;
                }
                return false;
            }

            void Reagente::acionarAlerta(unsigned int tipo){
                return;
            }

            bool Reagente::estaVencido(){
                if(this->dataValidade == "Desconhecida"){
                    return false;
                }
                time_t tempoBruto;
                tempoBruto = std::time(nullptr);

                tm *validade = {};

                std::istringstream ss(this->dataValidade);
                ss >> std::get_time(validade, "%Y-%m-%d");

                time_t validadeBruta = mktime(validade);

                if(validadeBruta > tempoBruto){
                    return false;
                } else {
                    return true;
                }


            }


            // Gets
            std::string Reagente::getNome() { return this->nome; }
            std::string Reagente::getDataValidade() { return this->dataValidade; }
            int Reagente::getQuantidade() { return this->quantidade; }
            int Reagente::getQuantidadeCritica() { return this->quantidadeCritica; }
            std::string Reagente::getLocalArmazenamento() { return this->localArmazenamento; }
            int Reagente::getNivelAcesso() { return this->nivelAcesso; }
            std::string Reagente::getUnidadeMedida() { return this->unidadeMedida; }
            std::string Reagente::getMarca() { return this->marca; }
            std::string Reagente::getCodigoReferencia() { return this->codigoReferencia; }
            int Reagente::getId() { return this->id; }

            // Sets
            void Reagente::setNome(std::string nome) { this->nome = nome; }
            void Reagente::setDataValidade(std::string dataValidade) { this->dataValidade = dataValidade; }
            void Reagente::setQuantidade(int quantidade) { this->quantidade = quantidade; }
            void Reagente::setQuantidadeCritica(int quantidadeCritica) { this->quantidadeCritica = quantidadeCritica; }
            void Reagente::setLocalArmazenamento(std::string localArmazenamento) { this->localArmazenamento = localArmazenamento; }
            void Reagente::setNivelAcesso(int nivelAcesso) { this->nivelAcesso = nivelAcesso; }
            void Reagente::setUnidadeMedida(std::string unidadeMedida) { this->unidadeMedida = unidadeMedida; }
            void Reagente::setMarca(std::string marca) { this->marca = marca; }
            void Reagente::setCodigoReferencia(std::string codigoReferencia) { this->codigoReferencia = codigoReferencia; }
