#include "../Alerta/AlertaValidade.h"


AlertaValidade::AlertaValidade(Reagente *reagenteEmAlerta,  bool situacao)
    :Alerta(reagenteEmAlerta, situacao) {}

AlertaValidade::AlertaValidade(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao)
    :Alerta(id, reagenteEmAlerta, dataEmissao, situacao) {}

void AlertaValidade::notificar() {
    std::cout << "[ALERTA]: O reagente " << _reagenteEmAlerta->getNome() << " passou da data de validade!\n";
}

void AlertaValidade::adicionarAlertaBD(){
    DatabaseConnection conexaoDB;
    Session* session = nullptr;
    Schema *db = nullptr;
    try{
        //Estabelece a conexão com o banco de dados
        session = conexaoDB.getSession(); // Obtém a sessão de conexão
        db = conexaoDB.getSchema(); // Obtém o esquema do banco de dados

        // Verifica se a conexão e o esquema foram inicializados corretamente
        if (!session || !db) {
            throw std::runtime_error("Falha ao inicializar a conexão com o banco de dados.");
        }} catch(std::runtime_error &e){
            e.what();
        }

        Table tabelaAlerta = db->getTable("Alerta");
        Result r = tabelaAlerta.insert("reagente_id", "dataHoraEmissao", "tipo", "situacao").values(_reagenteEmAlerta->getId(), _dataEmissao, 1, 1).execute();
}
