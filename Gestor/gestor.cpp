#include "../Usuario/usuario.h"
#include "../Gestor/gestor.h"
#include "../Estudante/estudante.h"
#include "../PosGraduacao/posgraduacao.h"
#include "../Laboratorio/Laboratorio.h"
#include <string>
#include <limits>
#include <iomanip>
#include <iostream>


Usuario** Gestor::usuariosCarregados = new Usuario*[10];
int Gestor::quantidadeUsuarios = 0;
int Gestor::capacidadeUsuarios = 10;

Gestor** Gestor::gestores = new Gestor*[10];
int Gestor::quantidadeGestores = 0;
int Gestor::capacidadeGestores = 10;

Estudante** Gestor::estudantes = new Estudante*[10];
int Gestor::quantidadeEstudantes = 0;
int Gestor::capacidadeEstudantes = 10;

PosGraduacao** Gestor::posGraduandos = new PosGraduacao*[10];
int Gestor::quantidadePos = 0;
int Gestor::capacidadePos = 10;


// Construtor
Gestor::Gestor(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema* db) :
    Usuario(nome, email, senha, nivelAcesso, db) {

    //Inicializa o ponteiro como nulo
    this->laboratorio = nullptr;
    }

// Destrutor
Gestor::~Gestor() {
    for (int i = 0; i < quantidadeUsuarios; i++) {
        delete usuariosCarregados[i];
    }
    delete[] usuariosCarregados;
    delete[] gestores;
    delete[] estudantes;
    delete[] posGraduandos;

    quantidadeUsuarios = 0;
    quantidadeGestores = 0;
    quantidadeEstudantes = 0;
    quantidadePos = 0;
    usuariosCarregados = nullptr;
    gestores = nullptr;
    estudantes = nullptr;
    posGraduandos = nullptr;

}

// Demais funções
void Gestor::cadastrarUsuario() {
    // Pega os dados básicos do novo usuário
    std::cout << "Digite o tipo de usuario a cadastrar:\n";
    std::cout << "1. Gestor\n2. Pos-Graduando\n3. Aluno de Graduacao\n";
    int nivelAcesso; // Nivel de acesso aos Reagentes, 1 = Gestor, 2 = Graduacao e 3 = pos Graduacao
    std::cin >> nivelAcesso;

    std::string nome, email, senha;
    std::string matricula, curso, nivel;
    int nivelInt; // Nivel do estudante (Graducao ou Pos-Graduacao)

    std::cout << "Digite o nome do usuário: ";
    std::cin >> nome;
    std::cout << "Digite o email do usuário: ";
    std::cin >> email;
    std::cout << "Digite a senha do usuário: ";
    std::cin >> senha;

    if (nivelAcesso == 2 || nivelAcesso == 3) {
        std::cout << "Digite a matricula do usuário " << nome << " :";
        std::cin >> matricula;
        std::cout << "Digite o curso do usuário " << nome << " :";
        std::cin >> curso;
        std::cout << "Digite o numero correspondente ao nivel(1-Graduacao, 2-Mestrado, 3-Doutorado, 4-PosDoutorado): ";
        std::cin >> nivelInt;
        switch(nivelInt){
            case 1: nivel = "Graduacao"; break;
            case 2: nivel = "Mestrado"; break;
            case 3: nivel = "Doutorado"; break;
            case 4: nivel = "PosDoutorado"; break;
            default: nivel = "Desconhecido"; break;
        }
    }
    try {
        Table usuarioTable = db->getTable("Usuario"); // Insire na tabela Usuario (base)
        Result res = usuarioTable.insert("nome", "email", "senha", "nivelAcesso") //Insere os dados básicos
            .values(nome, email, senha, nivelAcesso).execute(); // Define os valores e executa a inserção

        // Recupera o ID do usuário recém-inserido
        int usuarioId = res.getAutoIncrementValue();

        // Insere nas tabelas especializadas conforme o nivelAcesso
        if (nivelAcesso == 1) { // Gestor
            db->getTable("Gestor") // Insere na tabela Gestor
                .insert("id", "cadastrado_por_gestor_id") // Id do gestor e quem cadastrou
                .values(usuarioId, getId()) // quem cadastrou é o gestor atual
                .execute();
            std::cout << "Gestor " << nome << " cadastrado com sucesso!\n"; // Mensagem de sucesso

        } else if (nivelAcesso == 2 || nivelAcesso == 3) { // Pos-Graduando ou Aluno de Graduacao
            db->getTable("Estudante") // Insere na tabela Estudante
                .insert("id", "matricula", "curso", "nivel", "cadastrado_por_gestor_id") // Dados do estudante
                .values(usuarioId, matricula, curso, nivel, getId()) // quem cadastrou é o gestor atual
                .execute(); // Executa a inserção

            if (nivelAcesso == 2) { // Pos-Graduando
                db->getTable("PosGraduacao") // Insere na tabela PosGraduacao
                    .insert("id") // Apenas o ID do usuário
                    .values(usuarioId) // Define o valor do ID
                    .execute(); // Executa a inserção
                std::cout << "Pós-Graduando " << nome << " cadastrado com sucesso!\n"; // Mensagem de sucesso
            } else {
                std::cout << "Aluno de Graduação " << nome << " cadastrado com sucesso!\n"; // Mensagem de sucesso
            }
        }

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao cadastrar usuário: " << err.what() << std::endl;
    }
}

//função que carrega os usuários do banco de dados para o array dinâmico
void Gestor::carregarUsuarios() {
    try{
        Table usuarioTable = db->getTable("Usuario"); //pega a tabela usuario do banco
        RowResult resultado = usuarioTable.select("id", "nome", "email", "senha", "nivelAcesso").execute();//seleciona os campos necessários

        for (Row row : resultado) {
            int nivel = row[4].get<int>();//pega o nivel de acesso do usuario
            Usuario* u = nullptr;//ponteiro para objeto do tipo Usuario

            //cria o objeto do tipo correto baseado no nivel de acesso
            if (nivel == 1) {

                u = new Gestor(row[1].get<std::string>(), row[2].get<std::string>(), row[3].get<std::string>(), nivel, db);
            }
            else if (nivel == 2) {
                Table estudanteTable = db->getTable("Estudante");
                RowResult r = estudanteTable.select("matricula", "curso", "nivel").where("id = :id").bind("id",row[0]).execute();
                Row estudanteRow = r.fetchOne();
                u = new Estudante(row[1].get<std::string>(), row[2].get<std::string>(), row[3].get<std::string>(), nivel, db, estudanteRow[1].get<std::string>(), estudanteRow[2].get<std::string>(), estudanteRow[3].get<std::string>());
            }
            else if (nivel == 3) {
                Table estudanteTable = db->getTable("Estudante");
                RowResult r = estudanteTable.select("matricula", "curso", "nivel").where("id = :id").bind("id",row[0]).execute();
                Row estudanteRow = r.fetchOne();
                u = new PosGraduacao(row[1].get<std::string>(), row[2].get<std::string>(), row[3].get<std::string>(), nivel, db, estudanteRow[1].get<std::string>(), estudanteRow[2].get<std::string>(), estudanteRow[3].get<std::string>());
            }

            u->setId(row[0].get<int>());//define o id do usuario

            // Adiciona no vetor geral
            if (quantidadeUsuarios == capacidadeUsuarios) {
                int novaCapacidade = capacidadeUsuarios * 2;
                Usuario** novo = new Usuario*[novaCapacidade];
                for (int i = 0; i < quantidadeUsuarios; i++) novo[i] = usuariosCarregados[i];
                delete[] usuariosCarregados;
                usuariosCarregados = novo;
                capacidadeUsuarios = novaCapacidade;
            }
            usuariosCarregados[quantidadeUsuarios++] = u;//adiciona o usuario no array geral

            // Adiciona no vetor específico
            if (nivel == 1) {
                if (quantidadeGestores == capacidadeGestores) {
                    int novaCapacidade = capacidadeGestores * 2;
                    Gestor** novo = new Gestor*[novaCapacidade];
                    for (int i = 0; i < quantidadeGestores; i++) novo[i] = gestores[i];
                    delete[] gestores;
                    gestores = novo;
                    capacidadeGestores = novaCapacidade;
                }
                gestores[quantidadeGestores++] = (Gestor*)u;//adiciona o gestor no array de gestores
            } else if (nivel == 2) {
                if (quantidadeEstudantes == capacidadeEstudantes) {
                    int novaCapacidade = capacidadeEstudantes * 2;
                    Estudante** novo = new Estudante*[novaCapacidade];
                    for (int i = 0; i < quantidadeEstudantes; i++) novo[i] = estudantes[i];
                    delete[] estudantes;
                    estudantes = novo;
                    capacidadeEstudantes = novaCapacidade;
                }
                estudantes[quantidadeEstudantes++] = (Estudante*)u;//adiciona o estudante no array de estudantes
            } else if (nivel == 3) {
                if (quantidadePos == capacidadePos) {
                    int novaCapacidade = capacidadePos * 2;
                    PosGraduacao** novo = new PosGraduacao*[novaCapacidade];
                    for (int i = 0; i < quantidadePos; i++) novo[i] = posGraduandos[i];
                    delete[] posGraduandos;
                    posGraduandos = novo;
                    capacidadePos = novaCapacidade;
                }
                posGraduandos[quantidadePos++] = (PosGraduacao*)u;//adiciona o pos graduando no array de pos graduandos
            }

        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao carregar usuários: " << err.what() << std::endl;
    }
}
//lista os usuários do laboratório gerenciado por este gestor
void Gestor::listarUsuarios() {
    //verifica se o gestor está vinculado a um laboratorio
    if(laboratorio == nullptr) {
        std::cout << "Este Gestor não está vinculado a um laboratorio." << std::endl;
        return;
    }

    //cabeçalho da tabela
    std::cout << "\n=== LISTANDO USUÁRIOS (via Gestor) ===" << std::endl;
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(20) << "Nome"
              << std::setw(25) << "Email"
              << std::setw(15) << "Tipo"
              << "\n";
    std::cout << std::string(65, '-') << "\n";
    //laco que percorre o vetor de usuarios do laboratorio
    for (Usuario* u : laboratorio->getVetorUsuarios()) {
        std::string tipo;
        if (u->getNivelAcesso() == 1) tipo = "Gestor";
        else if (u->getNivelAcesso() == 2) tipo = "Graduacao";
        else if (u->getNivelAcesso() == 3) tipo = "Pos-Graduacao";
        else tipo = "Desconhecido";

        std::cout << std::left
                  << std::setw(5)  << u->getId()
                  << std::setw(20) << u->getNome()
                  << std::setw(25) << u->getEmail()
                  << std::setw(15) << tipo
                  << "\n";
    }
    std::cout << std::string(65, '-') << "\n";
}


void Gestor::deletarUsuario() {
    std::cout << "Digite o email do usuário a ser deletado: ";
    std::string email;
    std::cin >> email;

    try{
        //Busca o usuário no banco de dados
        Table usuarioTable = db->getTable("Usuario");
        RowResult result = usuarioTable.select("id", "nivelAcesso")
                                .where("email = :email")
                                .bind("email", email)
                                .execute();

        //Caso não encontre o usuário com o email fornecido
        if (result.count() == 0) {
            std::cout << "Usuário com email " << email << " não encontrado." << std::endl;
            return;
        }
        Row row = result.fetchOne();//pega a primeira linha do resultado
        int usuarioId = row[0];// ID do usuário a ser deletado
        int nivelAcesso = row[1];// Nível de acesso do usuário a ser deletado

        //Deleta das tabelas especializadas conforme o tipo de usuário
        if (nivelAcesso == 1) { // Gestor
            db->getTable("Gestor")
                .remove()
                .where("id = :id")
                .bind("id", usuarioId)
                .execute();
        } else if (nivelAcesso == 2 || nivelAcesso == 3) { // Pos-Graduando ou Aluno de Graduacao
            db->getTable("Estudante")
                .remove()
                .where("id = :id")
                .bind("id", usuarioId)
                .execute();

            if (nivelAcesso == 2) { // Pos-Graduando
                db->getTable("PosGraduacao")
                    .remove()
                    .where("id = :id")
                    .bind("id", usuarioId)
                    .execute();
            }
        }
        //Deleta da tabela base Usuario
        usuarioTable.remove()
                    .where("id = :id")
                    .bind("id", usuarioId)
                    .execute();

        std::cout << "Usuário deletado com sucesso!" << std::endl;// Mensagem de sucesso
    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao deletar usuário: " << err.what() << std::endl;//mensagem de erro
    }
}

// void Gestor::listarUsuarios() {
//     std::cout << "\n=== LISTANDO USUÁRIOS (via Gestor) ===" << std::endl;
//     std::cout << "Nome do Gestor: " << getNome() << std::endl;
//     std::cout << "Email do Gestor: " << getEmail() << std::endl;
//     std::cout << "Nível de Acesso: " << getNivelAcesso() << std::endl;
//     std::cout << "========================================\n" << std::endl;
// }

// Associa gestor ao laboratorio
void Gestor::associarLaboratorio(){
    // Verifica se o gestor já está associado a um laboratório
    if (this->laboratorio != nullptr) {
        std::cout << "O gestor já está associado em laboratório: "
                    << this->laboratorio->getNome() << std::endl; // E informa qual
        return;
    }

    //Verifica se no DB esta associado
    Table table = this->db->getTable("Gestor"); // Obtém a tabela "Gestor"
    RowResult result = table.select("laboratorio_id") // Seleciona a coluna "laboratorio_id"
                            .where("id = :id")       // Filtra pelo ID do gestor
                            .bind("id", this->getId()) // Substitui o parâmetro ":id"
                            .execute();              // Executa a consulta
    Row row = result.fetchOne(); // Busca a primeira linha do resultado

    if (!row.isNull()) {  // existe laboratório no banco
        int idLaboratorioBD = row[0];
        // Caso memória não esteja sincronizada, for divergente do bd e do objeto
        if (this->laboratorio == nullptr || this->laboratorio->getId() != idLaboratorioBD) {
             // Percorre todos os laboratórios carregados em memória
            for (int i = 0; i < Laboratorio::laboratorios.size(); i++) {
                if (Laboratorio::laboratorios[i]->getId() == idLaboratorioBD) {
                    this->laboratorio = Laboratorio::laboratorios[i]; // Atualiza objeto em memória
                    break; // Sai do loop após encontrar
                }
            }
        }
        // Imprime informação sobre a associação existente
        std::cout << "O gestor já está associado a um laboratório no banco de dados (ID: "
                    << idLaboratorioBD << "- "<< this->laboratorio->getNome()  << ").\n";
        return;   // Sai do método, não associa novamente
    }


    //Verifica se há laboratorios instanciados
    if (Laboratorio::laboratorios.empty()) {
        std::cout << "Nenhum laboratório carregado. \n";
        return;
    }
    //Imprime os laboratórios cadastrados
    Laboratorio::imprimirLaboratorios();

    //Usuario escolhe qual laboratorio associar
    int id;
    std::cout << "\nDigite o ID do laboratório que deseja gerenciar: ";
    std::cin >> id;

    //Variavel armazena o laboratorio escolhido pelo gestor
    Laboratorio* escolhido = nullptr;
    //Busca em laboratorios cadastrados baseado no ID
    for (int i = 0; i < Laboratorio::laboratorios.size(); i++) {
        Laboratorio* laboratorio = Laboratorio::laboratorios[i]; // armazena o ponteiro do laboratorio na posicao i
        if (laboratorio->getId() == id) { // Quando o id do escolhido for igual dos armazenado
            escolhido = laboratorio; //armazena o ponteiro
            break; //Para a iteração
        }
    }
    if (escolhido == nullptr) {
        std::cout << "Laboratório não encontrado.\n";
        return;
    }
    // Associa ao gestor, armazena no objeto
    this->laboratorio = escolhido;
    //Adiciona o gestor dentro do laboratorio (armazena no vetor de gestores)
    escolhido->adicionarGestor(this);

    // Atualiza no DB
    table = this->db->getTable("Gestor"); // Obtém a tabela novamente
    table.update()
         .set("laboratorio_id", id)       // Define o novo ID do laboratório
         .where("id = :id")               // Aplica a atualização ao gestor correto
         .bind("id", this->getId())       // Substitui o parâmetro ":id"
         .execute();                      // Executa a atualização
    //Confirmação para o usuário
    std::cout << "\nGestor gerencia o laboratório: "
                << escolhido->getNome()
                << std::endl;

}

void Gestor::associarEstudanteAoLaboratorio(Estudante* estudante, int idLaboratorio, const std::string& papel){
    Laboratorio* escolhido = nullptr; // Variavel que armazena o laboratorio escolhido
    for (int i = 0; i < Laboratorio::laboratorios.size(); i++) {
        if (Laboratorio::laboratorios[i]->getId() == idLaboratorio) {
            escolhido = Laboratorio::laboratorios[i];
            break;
        }
    }
    if (!escolhido) {
        std::cout << "Laboratório com ID " << idLaboratorio << " não encontrado.\n";
        return;
    }

    Table tableAssociado = this->db->getTable("Associado");
    RowResult result = tableAssociado.select("*")
                                        .where("estudante_id = :e_id AND laboratorio_id = :l_id")
                                        .bind("e_id", estudante->getId())
                                        .bind("l_id", idLaboratorio)
                                        .execute();
    if (!result.fetchOne().isNull()) {
        std::cout << "Estudante ID " << estudante->getId()
                    << " já está associado a este laboratório.\n";
        return;
    }
    tableAssociado.insert("estudante_id", "laboratorio_id", "papel")
                    .values(estudante->getId(), idLaboratorio, papel)
                    .execute();

    estudante->adicionarLaboratorio(escolhido);
}
// Metodo para cadastrar reagente
void Gestor::cadastrarReagente() {
    //Verifica se o gestor esta vinculado a um laboratorio
    if(this->laboratorio == nullptr) {
        std::cerr << "ERRO: O Gestor nao esta vinculado a um laboratorio." << std::endl;
        return;
    }

//Verifica se tem conexao com o banco
if(this->db == nullptr){
    std::cerr << "ERRO: Gestor nao esta conectado ao banco de dados." << std::endl;
    return;
}

    // Variaveis para guardar os dados da tabela base Reagente
    std::string nome, dataValidade, local, unidade, marca, codRef;
    int quantidade, quantidadeCritica, nivelAcesso;

    std::cout << "Cadastro de Novo Reagente \n";
    std::cout << "Nome: ";
    std::cin.ignore(); // Ignora o 'Enter' anterior
    std::getline(std::cin, nome);
    std::cout << "Data de Validade (AAAA-MM-DD): ";
    std::cin >> dataValidade;
    std::cout << "Quantidade: ";
    std::cin >> quantidade;
    std::cout << "Quantidade Critica: ";
    std::cin >> quantidadeCritica;
    std::cout << "Local de Armazenamento: ";
    std::cin.ignore();
    std::getline(std::cin, local);
    std::cout << "Nivel de Acesso (1, 2 ou 3): ";
    std::cin >> nivelAcesso;
    std::cout << "Unidade de Medida (ex: 'ml', 'g'): ";
    std::cin >> unidade;
    std::cout << "Marca: ";
    std::cin.ignore();
    std::getline(std::cin, marca);
    std::cout << "Codigo de Referencia: ";
    std::cin >> codRef;

    int tipo;
    //verificação de entrada valida
    while (true) {
        try {
            std::cout << "Digite o tipo (1 = Liquido, 2 = Solido): ";

            if (!(std::cin >> tipo)) {
            // cin falhou → jogamos uma exceção manualmente
                throw std::invalid_argument("Entrada invalida");
            }

            if (tipo != 1 && tipo != 2) {
                throw std::out_of_range("Tipo deve ser 1 ou 2");
            }

            break; // entrada correta → sai do while
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";

        // limpa erro do cin
            std::cin.clear();

        // descarta o lixo do buffer
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Declara as variaveis de tipo
    //densidade e volume so serao usadas se tipo == 1
    // massa e estadoFisico so serao usadas se tipo == 2
    double densidade = 0.0;
    double volume = 0.0;
    double massa = 0.0;
    std::string estadoFisico;

    if (tipo == 1) { // Liquido
        std::cout << "Densidade: ";
        std::cin >> densidade;
        std::cout << "Volume: ";
        std::cin >> volume;

    } else if (tipo == 2) { // Solido
        std::cout << "Massa: ";
        std::cin >> massa;

        // Pergunta o estado fisico que é um atributo da classe ReagenteSolido
        std::cout << "Estado Fisico (ex: 'po', 'cristal'): ";
        std::cin.ignore(); // Ignora o 'Enter' da leitura da massa
        std::getline(std::cin, estadoFisico);
    }
    // Salva no Banco de Dados
    try {
        // Insere na tabela base "Reagente"
        // O 'db' e herdado de Usuario e esta disponivel aqui
        Table reagenteTable = db->getTable("Reagente");

        // Insere dados comuns
        Result res = reagenteTable.insert(
            "nome", "dataValidade", "quantidade", "quantidadeCritica",
            "localArmazenamento", "nivelAcesso", "unidadeMedida", "marca", "codigoReferencia"
        ).values(nome, dataValidade, quantidade, quantidadeCritica,
                 local, nivelAcesso, unidade, marca, codRef)
         .execute(); // Executa a insercao no DB

        // Pega o ID do reagente que acabou de ser criado
        // (Precisamos desse ID para ligar com a tabela Liquido/Solido)
        int reagenteId = res.getAutoIncrementValue();

        // Insere nas tabelas especializadas (Liquido ou Solido)
        if (tipo == 1) {
            // Se for liquido, insere na tabela 'ReagenteLiquido'
            db->getTable("ReagenteLiquido")
                .insert("id", "densidade", "volume")
                .values(reagenteId, densidade, volume)
                .execute();
            // Imprime a confirmacao para o usuario
            std::cout << "Reagente Liquido '" << nome << "' cadastrado com sucesso!\n";
        }
        else if(tipo == 2) {
            // Se for solido, insere na tabela 'ReagenteSolido'
            db->getTable("ReagenteSolido")
                .insert("id", "massa", "estadoFisico")
                .values(reagenteId, massa, estadoFisico)
                .execute();
            // Imprime a confirmacao para o usuario
            std::cout << "Reagente Solido '" << nome << "' cadastrado com sucesso!\n";
        }

        // Atualiza memoria do laboratorio
        // Delega a tarefa de cadastrar regaente para o laboratorio
        laboratorio->cadastrarNovoReagente(
            reagenteId,nome, dataValidade, quantidade, quantidadeCritica,
            local, nivelAcesso, unidade, marca, codRef, tipo,
            densidade, volume, massa, estadoFisico
        );
     
    } catch (const mysqlx::Error &err) {
        // Se qualquer operacao do 'try' falhar, captura o erro
        // (Ex: se o banco estiver offline ou a tabela nao existir)
        std::cerr << "Erro ao cadastrar reagente: " << err.what() << std::endl;
    }
}

    // Metodo para vincular o gestor a um laboratorio
    void Gestor::setLaboratorio(Laboratorio* lab) {
        this->laboratorio = lab;
    }

    // Implementação da função virtual. O Gestor ignora a checagem de nível.
    void Gestor::acessarReagenteRestrito(int idReagente) {

        std::cout << "\n(Gestor) Acessando Reagente ID: " << idReagente << "\n";
        if (db == nullptr) {
            std::cerr << "ERRO: Gestor não está conectado ao banco." << std::endl;
            return;
        }

        try {
            Table reagenteTable = db->getTable("Reagente");

            // Busca o reagente pelo ID
            RowResult res = reagenteTable.select(
                "id", "nome", "quantidade", "unidadeMedida",
                "localArmazenamento", "dataValidade", "nivelAcesso"
            ).where("id = :id").bind("id", idReagente).execute();

            if (res.count() == 0) {
                std::cout << "Reagente com ID " << idReagente << " não encontrado." << std::endl;
                return;
            }

            // Pega os detalhes
            Row row = res.fetchOne();

            // O Gestor imprime tudo (não há checagem de nível)
            std::cout << "Acesso Permitido (Nível Gestor):\n";
            std::cout << "Nome:    " << row[1].get<std::string>() << "\n";
            std::cout << "Qtde:    " << row[2].get<int>() << " " << row[3].get<std::string>() << "\n";
            std::cout << "Local:   " << row[4].get<std::string>() << "\n";
            std::cout << "Validade: " << row[5].get<std::string>() << "\n";
            std::cout << "(Nível do reagente: " << row[6].get<int>() << ")\n";
            std::cout << "-------------------------------------\n";

        } catch (const mysqlx::Error &err) {
            std::cerr << "Erro ao acessar reagente: " << err.what() << std::endl;
        }
    }
