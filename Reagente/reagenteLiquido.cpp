#include "reagenteLiquido.h"

//Construtor
ReagenteLiquido::ReagenteLiquido(int id, std::string nome, std::string dataValidade, int quantidade,
                                 int quantidadeCritica, std::string localArmazenamento, int nivelAcesso,
                                 std::string unidadeMedida, std::string marca, std::string codigoReferencia,
                                 double densidade, double volume)

    // Chama o construtor de Reagente
    : Reagente(id, nome, dataValidade, quantidade, quantidadeCritica,
               localArmazenamento, nivelAcesso, unidadeMedida, marca, codigoReferencia)
{
    // O construtor inicializa os atributos que sao dele
    this->densidade = densidade;
    this->volume = volume;
}

//Destrutor
ReagenteLiquido::~ReagenteLiquido()
{

}

// Gets
double ReagenteLiquido::getDensidade() { return this->densidade; }
double ReagenteLiquido::getVolume() { return this->volume; }

// Sets
void ReagenteLiquido::setDensidade(double densidade) { this->densidade = densidade; }
void ReagenteLiquido::setVolume(double volume) { this->volume = volume; }
