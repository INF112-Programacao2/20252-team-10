#include "../Alerta/AlertaValidade.h"

AlertaValidade::AlertaValidade(Reagente *reagenteEmAlerta, bool situacao)
    : Alerta(reagenteEmAlerta, situacao) {}

AlertaValidade::AlertaValidade(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao)
    : Alerta(id, reagenteEmAlerta, dataEmissao, situacao) {}

void AlertaValidade::notificar()
{
    std::cout << "[ALERTA]: O reagente " << _reagenteEmAlerta->getNome() << " passou da data de validade!\n";
}

void AlertaValidade::adicionarAlertaBD(Schema *db)
{
    Table tabelaAlerta = db->getTable("Alerta");
    RowResult b = tabelaAlerta.select("reagente_id", "tipo").where("reagente_id =: id AND tipo =: t").bind("id", _reagenteEmAlerta->getId()).bind("t", 1).execute();
    if (b.count() != 0)
    {
        return;
    }
    else
    {
        Result r = tabelaAlerta.insert("reagente_id", "dataHoraEmissao", "tipo", "situacao").values(_reagenteEmAlerta->getId(), _dataEmissao, 1, 1).execute();
    }
}
