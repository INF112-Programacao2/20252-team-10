#ifndef ReagenteSolido_H
#define ReagenteSolido_H
#include "../Reagente/reagente.h" 
#include <string>

class ReagenteSolido : public Reagente {
private:
    double massa;
    std::string estadoFisico;

public:
    //Construtor
    ReagenteSolido(int id, std::string nome, std::string dataValidade, int quantidade, 
                   int quantidadeCritica, std::string localArmazenamento, int nivelAcesso, 
                   std::string unidadeMedida, std::string marca, std::string codigoReferencia,
                   double massa, std::string estadoFisico); 

    //Destrutor
    ~ReagenteSolido() override; //sobrescrevendo o metodo virtual

    // Gets
    double getMassa();
    std::string getEstadoFisico();

    //Sets
    void setMassa(double massa);
    void setEstadoFisico(std::string estadoFisico);
};

#endif