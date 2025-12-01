#include "../Alerta/AlertaQuantidade.h"

AlertaQuantidade::AlertaQuantidade(Reagente *reagenteEmAlerta, bool situacao)
    : Alerta(reagenteEmAlerta, situacao) {}

AlertaQuantidade::AlertaQuantidade(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao)
    : Alerta(id, reagenteEmAlerta, dataEmissao, situacao) {}

void AlertaQuantidade::notificar()
{
    std::cout << "[ALERTA]: O reagente " << _reagenteEmAlerta->getNome() << " chegou em quantidade crítica!\n";
}

void AlertaQuantidade::adicionarAlertaBD(Schema *db)
{

    Table tabelaAlerta = db->getTable("Alerta");
    RowResult b = tabelaAlerta.select("reagente_id", "tipo").where("reagente_id =: id AND tipo =: t").bind("id", _reagenteEmAlerta->getId()).bind("t", 2).execute();
    if (b.count() != 0)
    {
        return;
    }
    else
    {
        Result r = tabelaAlerta.insert("reagente_id", "dataHoraEmissao", "tipo", "situacao").values(_reagenteEmAlerta->getId(), _dataEmissao, 2, 1).execute();
    }
}
