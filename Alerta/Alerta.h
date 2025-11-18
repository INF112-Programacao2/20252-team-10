#ifndef ALERTA_H
#define ALERTA_H
#include <ctime>
#include <string>
#include "../Reagente/reagente.h"

class Alerta
{
private:
    unsigned int _id;
    time_t _tempoBruto;       // tempo em unix time stamp
    struct tm *_tempoInfo;    // struct que guarda informacoes do tempo atual
    std::string _dataEmissao; // tempo formatado para leitura
    unsigned int _tipo;
    std::string _situacao; // conteúdo do alerta
    Reagente * _reagenteEmAlerta;

public:
    // Construtor
    Alerta(unsigned int tipo, Reagente *reagenteEmAlerta);

    // Destrutor
    ~Alerta();

    // Gets
    unsigned int getId();
    std::string getDataEmissao();
    unsigned int getTipo();
    std::string getSituacao();

    // Sets
    void setId(int id);
    void setDataEmissao();
    void setTipo(int tipo);
    void setSituacao(std::string situacao);

    // Outros métodos
    // void adicionarAlertaBD();
    // void fecharAlertaBD();
    void situacaoPorTipo();
    void notificar();
};

#endif
