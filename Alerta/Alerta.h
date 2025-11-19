#ifndef ALERTA_H
#define ALERTA_H
#include <ctime>
#include <string>
#include "../Reagente/reagente.h"
#include "../DatabaseConnection/databaseConnection.h"
#include <mysql-cppconn/mysqlx/xdevapi.h>

class Alerta
{
protected:
    int _id;
    time_t _tempoBruto;             // tempo em unix time stamp
    struct tm *_tempoInfo;          // struct que guarda informacoes do tempo atual
    std::string _dataEmissao;       // tempo formatado para leitura
    bool _situacao;                 // aberto ou fechado
    Reagente * _reagenteEmAlerta;

public:
    // Construtor
    Alerta(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao);
    Alerta(Reagente *reagenteEmAlerta, bool situacao);

    // Destrutor
    ~Alerta();

    // Gets
    int getId();
    std::string getDataEmissao();
    bool getSituacao();

    // Sets
    void setId(int id);
    void setDataEmissao();
    void setSituacao(bool situacao);

    // Outros métodos
    virtual void adicionarAlertaBD() = 0;
    void fecharAlertaBD();
    virtual void notificar() = 0;
};

#endif
