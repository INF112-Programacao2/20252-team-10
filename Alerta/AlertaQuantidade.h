#ifndef ALERTA_QUANTIDADE_H
#define ALERTA_QUANTIDADE_H

#include "../Alerta/Alerta.h"

class AlertaQuantidade : public Alerta
{

public:
    AlertaQuantidade(Reagente *reagenteEmAlerta, bool situacao);
    AlertaQuantidade(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao);

    void notificar() override;
    void adicionarAlertaBD(Schema *db) override;
};

#endif
