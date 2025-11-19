#include "../Alerta/AlertaQuantidade.h"

AlertaQuantidade::AlertaQuantidade(Reagente *reagenteEmAlerta,  bool situacao)
    :Alerta(reagenteEmAlerta, situacao) {}

AlertaQuantidade::AlertaQuantidade(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao)
    : Alerta(id, reagenteEmAlerta, dataEmissao, situacao) {}




void AlertaQuantidade::notificar() {
    std::cout << "[ALERTA]: O reagente " << _reagenteEmAlerta->getNome() << " chegou em quantidade crítica!\n";
}

void AlertaQuantidade::adicionarAlertaBD(){
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
        Result r = tabelaAlerta.insert("reagente_id", "dataHoraEmissao", "tipo", "situacao").values(_reagenteEmAlerta->getId(), _dataEmissao, 2, 1).execute();
}
