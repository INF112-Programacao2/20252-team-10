#ifndef ALERTA_VALIADADE_H
#define ALERTA_VALIADADE_H

#include "../Alerta/Alerta.h"

class AlertaValidade : public Alerta
{

public:
    AlertaValidade(Reagente *reagenteEmAlerta, bool situacao);
    AlertaValidade(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao);

    void notificar() override;
    void adicionarAlertaBD(Schema *db) override;
};

#endif
