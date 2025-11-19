#include "../Alerta/Alerta.h"


// Construtor

Alerta::Alerta(Reagente *reagenteEmAlerta,  bool situacao)
    : _reagenteEmAlerta(reagenteEmAlerta), _situacao(situacao) {
        time_t *agora;
        time(agora);
        _dataEmissao = ctime(agora);
    }



Alerta::Alerta(int id, Reagente *reagenteEmAlerta, std::string dataEmissao, bool situacao)
    : _id(id), _reagenteEmAlerta(reagenteEmAlerta), _dataEmissao(dataEmissao), _situacao(situacao) {}



// Destrutor

Alerta::~Alerta() {}

// Gets

int Alerta::getId()
{
    return this->_id;
}

std::string Alerta::getDataEmissao()
{
    return this->_dataEmissao;
}

bool Alerta::getSituacao()
{
    return this->_situacao;
}

// Sets

void Alerta::setId(int id)
{
    this->_id = id;
}

void Alerta::setDataEmissao()
{
    this->_tempoBruto = std::time(nullptr);
    this->_tempoInfo = std::localtime(&_tempoBruto);
    this->_dataEmissao = std::asctime(_tempoInfo);
}

void Alerta::setSituacao(bool situacao)
{
    this->_situacao = situacao;
}

void Alerta::fecharAlertaBD(){
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
            tabelaAlerta.update()
                .set("situacao", 0)
                .where("id = :id")
                .bind("id", _id)
                .execute();

}
