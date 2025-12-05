#include "reagenteSolido.h"

//Construtor
ReagenteSolido::ReagenteSolido(int id, std::string nome, std::string dataValidade, int quantidade,
                               int quantidadeCritica, std::string localArmazenamento, int nivelAcesso,
                               std::string unidadeMedida, std::string marca, std::string codigoReferencia,
                               double massa, std::string estadoFisico)

    // Chama o construtor de Reagente
    : Reagente(id, nome, dataValidade, quantidade, quantidadeCritica,
               localArmazenamento, nivelAcesso, unidadeMedida, marca, codigoReferencia)
{
    // O construtor inicializa os atributos que sao dele
    this->massa = massa;
    this->estadoFisico = estadoFisico;
}

//Destrutor
ReagenteSolido::~ReagenteSolido()
{

}

// Gets
double ReagenteSolido::getMassa() { return this->massa; }
std::string ReagenteSolido::getEstadoFisico() { return this->estadoFisico; }

// Sets
void ReagenteSolido::setMassa(double massa) { this->massa = massa; }
void ReagenteSolido::setEstadoFisico(std::string estadoFisico) { this->estadoFisico = estadoFisico; }
