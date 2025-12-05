#ifndef ReagenteLiquido_H
#define ReagenteLiquido_H
#include "../Reagente/reagente.h" 
#include <string>

class ReagenteLiquido : public Reagente {
private:
    double densidade;
    double volume;

public:
    //Construtor
    ReagenteLiquido(int id, std::string nome, std::string dataValidade, int quantidade, 
                    int quantidadeCritica, std::string localArmazenamento, int nivelAcesso, 
                    std::string unidadeMedida, std::string marca, std::string codigoReferencia,
                    double densidade, double volume); 
    
    //Destrutor
    ~ReagenteLiquido() override; // subrescrevendo o metodo virtual

    // Gets
    double getDensidade();
    double getVolume();

    //Sets
    void setDensidade(double densidade);
    void setVolume(double volume);
};

#endif 