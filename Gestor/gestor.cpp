#include "../Usuario/usuario.h"
#include "../Gestor/gestor.h"
#include "../Estudante/estudante.h"
#include "../PosGraduacao/posgraduacao.h"
#include "../Laboratorio/Laboratorio.h"
#include <string>
#include <limits>
#include <iomanip>
#include <iostream>

using namespace mysqlx;


bool confirmacao() {
    std::cout << "Tem certeza em realizar esta ação? Digite S para Sim e N para Não: ";
    char resposta;
    std::cin >> resposta;
    return (resposta == 'S' || resposta == 's');
}

std::vector<Usuario*> Gestor::usuariosCarregados;
std::vector<Gestor*> Gestor::gestores;
std::vector<Estudante*> Gestor::estudantes;
std::vector<PosGraduacao*> Gestor::posGraduandos;

// Construtor
Gestor::Gestor(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema* db) :
    Usuario(nome, email, senha, nivelAcesso, db) {
        //Inicializa o ponteiro como nulo
        this->laboratorio = nullptr;
    }
// Destrutor
Gestor::~Gestor() {}

//Getters

//Retorna o laboratório que o gestor está alocado
Laboratorio* Gestor::getLaboratorio(){
    return this->laboratorio;
};

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
    bool confirma = confirmacao();
    if(confirma){
        try {
            Table usuarioTable = db->getTable("Usuario"); // Insere na tabela Usuario (base)
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
}

//função que carrega os usuários do banco de dados para o array dinâmico
void Gestor::carregarUsuarios() {

    usuariosCarregados.clear();
    gestores.clear();
    estudantes.clear();
    posGraduandos.clear();
    try{
        Table usuarioTable = db->getTable("Usuario"); //pega a tabela usuario do banco
        RowResult resultado = usuarioTable.select("id", "nome", "email", "senha", "nivelAcesso").execute();//seleciona os campos necessários

        for (Row row : resultado) {
            int id = row[0].get<int>();
        std::string nome = row[1].get<std::string>();
        std::string email = row[2].get<std::string>();
        std::string senha = row[3].get<std::string>();
        int nivel = row[4].get<int>();

        Usuario* u = nullptr;

        if (nivel == 1) {
            auto* g = new Gestor(nome, email, senha, nivel, db);
            g->setId(id);
            gestores.push_back(g);
            u = g;
        }
        else if (nivel == 2) { // estudante
            auto* e = new Estudante(nome, email, senha, nivel, db, "", "", "grad");
            e->setId(id);
            estudantes.push_back(e);
            u = e;
        }
        else if (nivel == 3) { // estudante pos
            auto* p = new PosGraduacao(nome, email, senha, nivel, db, "", "", "pos");
            p->setId(id);
            posGraduandos.push_back(p);
            u = p;
        }

        usuariosCarregados.push_back(u);
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
    for (Usuario* u : laboratorio->getUsuarios()) {
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
    // Pede o email dentro da função
    std::string email;
    std::cout << "Digite o email do usuário a ser deletado: ";
    std::cin >> email;

    try {
        // Verifica Gestores
        for (size_t i = 0; i < gestores.size(); ++i) {
            if (gestores[i]->getEmail() == email) {
                if (!confirmacao()) {
                    std::cout << "Operação cancelada.\n";
                    return;
                }

                int id = gestores[i]->getId();

                db->getTable("Gestor")
                  .remove()
                  .where("id = :id")
                  .bind("id", id)
                  .execute();

                db->getTable("Usuario")
                  .remove()
                  .where("id = :id")
                  .bind("id", id)
                  .execute();

                for (size_t j = 0; j < usuariosCarregados.size(); ++j) {
                    if (usuariosCarregados[j]->getEmail() == email) {
                        delete usuariosCarregados[j];
                        usuariosCarregados.erase(usuariosCarregados.begin() + j);
                        break;
                    }
                }

                delete gestores[i];
                gestores.erase(gestores.begin() + i);

                std::cout << "Gestor deletado!\n";
                return;
            }
        }

        // Verifica Estudantes
        for (size_t i = 0; i < estudantes.size(); ++i) {
            if (estudantes[i]->getEmail() == email) {
                if (!confirmacao()) {
                    std::cout << "Operação cancelada.\n";
                    return;
                }

                int id = estudantes[i]->getId();

                db->getTable("Estudante")
                  .remove()
                  .where("id = :id")
                  .bind("id", id)
                  .execute();

                db->getTable("Usuario")
                  .remove()
                  .where("id = :id")
                  .bind("id", id)
                  .execute();

                for (size_t j = 0; j < usuariosCarregados.size(); ++j) {
                    if (usuariosCarregados[j]->getEmail() == email) {
                        delete usuariosCarregados[j];
                        usuariosCarregados.erase(usuariosCarregados.begin() + j);
                        break;
                    }
                }

                delete estudantes[i];
                estudantes.erase(estudantes.begin() + i);

                std::cout << "Estudante deletado!\n";
                return;
            }
        }

        // Verifica Pós-Graduacao
        for (size_t i = 0; i < posGraduandos.size(); ++i) {
            if (posGraduandos[i]->getEmail() == email) {
                if (!confirmacao()) {
                    std::cout << "Operação cancelada.\n";
                    return;
                }

                int id = posGraduandos[i]->getId();

                db->getTable("PosGraduacao")
                  .remove()
                  .where("id = :id")
                  .bind("id", id)
                  .execute();

                db->getTable("Usuario")
                  .remove()
                  .where("id = :id")
                  .bind("id", id)
                  .execute();

                for (size_t j = 0; j < usuariosCarregados.size(); ++j) {
                    if (usuariosCarregados[j]->getEmail() == email) {
                        delete usuariosCarregados[j];
                        usuariosCarregados.erase(usuariosCarregados.begin() + j);
                        break;
                    }
                }

                delete posGraduandos[i];
                posGraduandos.erase(posGraduandos.begin() + i);

                std::cout << "Pós-Graduando deletado!\n";
                return;
            }
        }

        std::cout << "Usuário com email '" << email << "' não encontrado.\n";

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao deletar usuário: " << err.what() << std::endl;
    }
}

void Gestor::cadastrarGestor() {
    std::string nome, email, senha;

    std::cout << "\n=== CADASTRAR GESTOR ===\n";
    std::cout << "Digite o nome do gestor: ";
    std::cin >> nome;
    std::cout << "Digite o email do gestor: ";
    std::cin >> email;
    std::cout << "Digite a senha do gestor: ";
    std::cin >> senha;

    try {
        // Insere na tabela Usuario (base)
        Table usuarioTable = db->getTable("Usuario");
        Result res = usuarioTable.insert("nome", "email", "senha", "nivelAcesso")
            .values(nome, email, senha, 1) // 1 = Gestor
            .execute();

        // Recupera o ID do gestor recém-criado
        int gestorId = res.getAutoIncrementValue();

        // Insere na tabela Gestor
        db->getTable("Gestor")
            .insert("id", "cadastrado_por_gestor_id") // ID do gestor e quem cadastrou
            .values(gestorId, getId()) // quem cadastrou é o gestor atual
            .execute();

        std::cout << "Gestor " << nome << " cadastrado com sucesso!\n";

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao cadastrar gestor: " << err.what() << std::endl;
    }
}
void Gestor::cadastrarEstudante() {
    std::string nome, email, senha, matricula, curso, nivel;
    int nivelInt;
    int nivelAcesso;

    std::cout << "Digite o nome do estudante: ";
    std::cin >> nome;
    std::cout << "Digite o email do estudante: ";
    std::cin >> email;
    std::cout << "Digite a senha do estudante: ";
    std::cin >> senha;
    std::cout << "Digite a matrícula do estudante: ";
    std::cin >> matricula;
    std::cout << "Digite o curso do estudante: ";
    std::cin >> curso;
    std::cout << "Digite o numero correspondente ao nível (1-Graduacao, 2-Mestrado, 3-Doutorado, 4-PosDoutorado): ";
    std::cin >> nivelInt;

    switch(nivelInt) {
        case 1: nivel = "Graduacao"; nivelAcesso = 2; break;
        case 2: nivel = "Mestrado"; nivelAcesso = 3; break;
        case 3: nivel = "Doutorado"; nivelAcesso = 3; break;
        case 4: nivel = "PosDoutorado"; nivelAcesso = 3; break;
        default: nivel = "Desconhecido"; nivelAcesso = 2; break;
    }
    try {
        // Inserir na tabela Usuario
        Table usuarioTable = db->getTable("Usuario");
        Result res = usuarioTable.insert("nome", "email", "senha", "nivelAcesso")
                                  .values(nome, email, senha, nivelAcesso)
                                  .execute();

        int usuarioId = res.getAutoIncrementValue();

        Estudante* estudante = new Estudante(nome, email, senha, nivelAcesso, db, matricula, curso, nivel);
        estudante->setId(usuarioId);
        this->estudantes.push_back(estudante);

        // Inserir na tabela Estudante
        db->getTable("Estudante")
          .insert("id", "matricula", "curso", "nivel", "cadastrado_por_gestor_id")
          .values(usuarioId, matricula, curso, nivel, getId())
          .execute();


        std::cout << "Estudante " << nome << " cadastrado com sucesso!\n";

        // Se o gestor tem laboratório associado, associa automaticamente
        if (this->laboratorio != nullptr) {
            this->associarEstudanteAoLaboratorio(estudante, this->laboratorio->getId(), "Aluno");
            std::cout << "Estudante associado automaticamente ao laboratório: "
                      << this->laboratorio->getNome() << std::endl;
        }

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao cadastrar estudante: " << err.what() << std::endl;
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
    if (row && !row[0].isNull()) {  // existe laboratório no banco
        int idLaboratorioBD = row[0].get<int>();
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

    // Confirmação antes de associar
    if (!confirmacao()) {
        std::cout << "Associação cancelada pelo usuário.\n";
        return;
    }

    // Associa ao gestor, armazena no objeto
    this->laboratorio = escolhido;
    //Adiciona o gestor dentro do laboratorio (armazena no vetor de gestores)
    escolhido->adicionarGestor(this);

    // Atualiza no DB
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
    std::cout << "Nivel de Acesso:\n 1 - Restrito (Apenas Gestores)\n 2 - Livre (Graduação)\n 3 - Pós-Graduação\nDigite a opção: ";
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
            "nome", "validade", "quantidade", "quantidadeCritica",
            "localArmazenamento", "nivelAcesso", "unidadeMedida", "marca", "referencia", "laboratorio_id"
        ).values(nome, dataValidade, quantidade, quantidadeCritica,
                 local, nivelAcesso, unidade, marca, codRef, this->laboratorio->getId())
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

void Gestor::acessarReagentesAlerta(){
    laboratorio->getAlertasGestor();
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

void Gestor::listarReagentesRestritos(){
        if (db == nullptr) {
            std::cerr << "ERRO: Gestor não está conectado ao banco." << std::endl;
            return;
        }

        try {
            Table reagenteTable = db->getTable("Reagente");

            // Busca o reagente pelo ID
            // RowResult res = reagenteTable.select(
            //     "id", "nome", "quantidade", "unidadeMedida",
            //     "localArmazenamento", "validade", "nivelAcesso"
            // ).where("nivelAcesso =: nivel").bind("nivel", 1).execute();

            SqlResult res = db->getSession().sql("SELECT id, nome, quantidade, unidadeMedida, localArmazenamento, DATE_FORMAT(validade, '%Y-%m-%d') as data_formatada FROM LabUFV.Reagente WHERE nivelAcesso = 1").execute();

            if (res.count() == 0) {
                std::cout << "Reagentes restritos não encontrados\n" << std::endl;
                return;
            }
            int size = res.count();
            for(int i = 0; i < size; i++){
            std::cout << res.count() << std::endl;
            // Pega os detalhes
            Row row = res.fetchOne();

            // O Gestor imprime tudo (não há checagem de nível)
            std::cout << "Id:      " << row[0].get<int>() << "\n";
            std::cout << "Nome:    " << row[1].get<std::string>() << "\n";
            std::cout << "Qtde:    " << row[2].get<int>() << " " << row[3].get<std::string>() << "\n";
            std::cout << "Local:   " << row[4].get<std::string>() << "\n";
            if(!row[5].isNull())
            std::cout << "Validade: " << row[5].get<std::string>() << "\n";
            std::cout << "-------------------------------------\n";
            }

        } catch (const mysqlx::Error &err) {
            std::cerr << "Erro ao acessar reagente: " << err.what() << std::endl;
        }
}

void Gestor::menuReagentesRestritos(){
        int opcao = 0;
    do {

                std::cout << "===== Menu Reagentes Restritos =====\n";
                std::cout << "1. Listar reagentes restritos\n";
                std::cout << "2. Retirar reagente\n";
                std::cout << "3. Registrar reagente\n";
                std::cout << "0. Sair do sistema\n";
                std::cout << "Escolha uma opção: ";
                std::cin >> opcao;
                switch(opcao) {
                case 1: {
                    this->listarReagentesRestritos();
                    break; }
                case 2: {
                    std::cout << "Digite o nome do reagente: (Encontra nome parcial)\n";
                    std::string nomeBuscado;
                    float quantidadeRetirada;
                    std::cin.ignore();
                    std::getline(std::cin, nomeBuscado);
                    std::cout << "Digite a quantidade retirada: \n";
                    std::cin >> quantidadeRetirada;
                    if(confirmacao()){
                    this->laboratorio->registrarRetirada(this, nomeBuscado, quantidadeRetirada); }
                     else std::cout << "Ação cancelada pelo usuário\n";
                    break; }
                case 3: {
                    this->cadastrarReagente();
                    break; }
                case 0: {
                    std::cout << "Saindo...\n";
                    break; }
                default: {
                    std::cout << "Opção inválida! Tente novamente.\n"; }
            }

    } while(opcao != 0);
}

bool Gestor::estaAssociado() const {
    return laboratorio != nullptr;
}

void Gestor::listarReagentesDoLaboratorio(){
    if(laboratorio==nullptr) return;

    //Busca todos os reagentes (string vazia retorna todos)
    std::vector<Reagente*> lista = laboratorio->listarReagentes("");

    if(lista.empty()){
        std::cout << "Nenhum reagente cadastrado neste laboratório\n";
        return;
    }

    std::cout << "\n Lista de Reagentes:";
    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(25) << "Nome"
              << std::setw(15) << "Qtd/Unidade"
              << std::setw(15) << "Validade"
              << std::setw(10) << "Nível" << "\n";
    std::cout << std::string(70,'-') << "\n";

   for (Reagente* r : lista) {
        std::cout << std::left
                  << std::setw(5) << r->getId()
                  << std::setw(25) << r->getNome()
                  << std::setw(15) << (std::to_string(r->getQuantidade()) + " " + r->getUnidadeMedida())
                  << std::setw(15) << r->getDataValidade()
                  << std::setw(10) << r->getNivelAcesso() << "\n";
    }
    std::cout << std::string(70, '-') << "\n";
}

void Gestor::editarReagente() {
    std::cout << "\n Editar Reagente \n";
    std::cout << "Digite o nome do reagente que deseja editar: ";
    std::string nomeBusca; //variavel local para guardar temporariamente o nome do reagente
    std::cin.ignore(); // Limpa buffer antes de ler string
    std::getline(std::cin, nomeBusca);

    // Busca no laboratorio
    Reagente* reagente = laboratorio->buscarReagente(nomeBusca);

    if (reagente == nullptr) {
        std::cout << "Reagente não encontrado.\n";
        return;
    }

    // Mostra dados atuais
    std::cout << "Reagente encontrado: " << reagente->getNome() << " (ID: " << reagente->getId() << ")\n";
    std::cout << "O que deseja alterar?\n";
    std::cout << "1. Quantidade (Atual: " << reagente->getQuantidade() << ")\n";
    std::cout << "2. Local de Armazenamento (Atual: " << reagente->getLocalArmazenamento() << ")\n";
    std::cout << "3. Data de Validade (Atual: " << reagente->getDataValidade() << ")\n";
    std::cout << "4. Corrigir Nome (Atual: " << reagente->getNome() << ")\n";
    std::cout << "5. Corrigir Nível de Acesso (Atual: " << reagente->getNivelAcesso() << ")\n";
    std::cout << "6. Corrigir Dados Específicos (Densidade/Volume ou Massa/Estado)\n";
    std::cout << "0. Cancelar\n";
    std::cout << "Opção: ";

    int subOpcao;
    std::cin >> subOpcao;

    if (subOpcao == 0) return;

    try {
        Table table = db->getTable("Reagente");

        if (subOpcao == 1) { // Quantidade
            int novaQtd;
            std::cout << "Nova Quantidade: ";
            std::cin >> novaQtd;

            if (!confirmacao()) return;

            table.update().set("quantidade", novaQtd).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setQuantidade(novaQtd);
            std::cout << "Quantidade atualizada com sucesso!\n";

        } else if (subOpcao == 2) { // Local
            std::string novoLocal;
            std::cout << "Novo Local: ";
            std::cin.ignore();
            std::getline(std::cin, novoLocal);

            if (!confirmacao()) return;

            table.update().set("localArmazenamento", novoLocal).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setLocalArmazenamento(novoLocal);
            std::cout << "Local atualizado com sucesso!\n";

        } else if (subOpcao == 3) { // Validade
            std::string novaValidade;
            std::cout << "Nova Validade (AAAA-MM-DD): ";
            std::cin >> novaValidade;

            if (!confirmacao()) return;

            table.update().set("dataValidade", novaValidade).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setDataValidade(novaValidade);
            std::cout << "Validade atualizada com sucesso!\n";

        } else if (subOpcao == 4) { // Nome
            std::string novoNome;
            std::cout << "Novo Nome Correto: ";
            std::cin.ignore();
            std::getline(std::cin, novoNome);

            if (!confirmacao()) return;

            table.update().set("nome", novoNome).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setNome(novoNome);
            std::cout << "Nome corrigido com sucesso!\n";

        } else if (subOpcao == 5) { // Nivel
            int novoNivel;
            std::cout << "Novo Nível (1-Restrito, 2-Livre, 3-Pós): ";
            std::cin >> novoNivel;

            if (!confirmacao()) return;

            table.update().set("nivelAcesso", novoNivel).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setNivelAcesso(novoNivel);
            std::cout << "Nível de acesso atualizado com sucesso!\n";

        } else if (subOpcao == 6) {
            std::cout << "Qual o tipo deste reagente?\n";
            std::cout << "1. Líquido\n";
            std::cout << "2. Sólido\n";
            std::cout << "Escolha: ";
            int tipoEscolhido;
            std::cin >> tipoEscolhido;

            if (tipoEscolhido == 1) {
                // Converte manualmente o ponteiro para Liquido
                ReagenteLiquido* liq = static_cast<ReagenteLiquido*>(reagente);

                double novaDensidade, novoVolume;
                std::cout << "Editando Líquido\n";
                std::cout << "Nova Densidade (Atual: " << liq->getDensidade() << "): ";
                std::cin >> novaDensidade;
                std::cout << "Novo Volume (Atual: " << liq->getVolume() << "): ";
                std::cin >> novoVolume;

                if (!confirmacao()) return;

                db->getTable("ReagenteLiquido").update()
                    .set("densidade", novaDensidade)
                    .set("volume", novoVolume)
                    .where("id = :id").bind("id", reagente->getId()).execute();

                liq->setDensidade(novaDensidade);
                liq->setVolume(novoVolume);
                std::cout << "Dados do líquido atualizados!\n";

            } else if (tipoEscolhido == 2) {
                // Converte manualmente o ponteiro para Solido
                ReagenteSolido* sol = static_cast<ReagenteSolido*>(reagente);

                double novaMassa;
                std::string novoEstado;

                std::cout << "Editando Sólido \n";
                std::cout << "Nova Massa (Atual: " << sol->getMassa() << "): ";
                std::cin >> novaMassa;
                std::cout << "Novo Estado Físico (Atual: " << sol->getEstadoFisico() << "): ";
                std::cin.ignore();
                std::getline(std::cin, novoEstado);

                if (!confirmacao()) return;

                db->getTable("ReagenteSolido").update()
                    .set("massa", novaMassa)
                    .set("estadoFisico", novoEstado)
                    .where("id = :id").bind("id", reagente->getId()).execute();

                sol->setMassa(novaMassa);
                sol->setEstadoFisico(novoEstado);
                std::cout << "Dados do sólido atualizados!\n";
            } else {
                std::cout << "Tipo inválido selecionado.\n";
            }
        }

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao atualizar reagente no banco: " << err.what() << std::endl;
    }
}

void Gestor::excluirReagente() {
    std::cout << "\n Excluir Reagente \n";
    std::cout << "Digite o nome do reagente a excluir: ";
    std::string nomeBusca;
    std::cin.ignore();
    std::getline(std::cin, nomeBusca);

    Reagente* reagente = laboratorio->buscarReagente(nomeBusca);

    if (reagente == nullptr) {
        std::cout << "Reagente não encontrado.\n";
        return;
    }

    std::cout << "ATENÇÃO: Você está prestes a excluir: " << reagente->getNome() << "\n";
    if (!confirmacao()) {
        std::cout << "Operação cancelada.\n";
        return;
    }

    int idParaRemover = reagente->getId();

    try {
        // 1-Remove das tabelas filhas
        db->getTable("ReagenteLiquido").remove().where("id = :id").bind("id", idParaRemover).execute();
        db->getTable("ReagenteSolido").remove().where("id = :id").bind("id", idParaRemover).execute();

        // 2-Remove da tabela principal
        db->getTable("Reagente").remove().where("id = :id").bind("id", idParaRemover).execute();

        // 3-Remove da memoria do laboratorio
        laboratorio->removerReagenteDaMemoria(idParaRemover);

        std::cout << "Reagente excluído com sucesso do banco e da memória.\n";

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao excluir do banco de dados: " << err.what() << std::endl;
    }
}

void Gestor::filtrarReagentes() {
    std::cout << "\nFiltrar Reagentes\n";
    std::cout << "1. Por Categoria (Sólido/Líquido)\n";
    std::cout << "2. Por Restrição (Restritos/Livres)\n";
    std::cout << "Opção: ";
    int op;
    std::cin >> op;

    // Pega lista completa
    std::vector<Reagente*> todos = laboratorio->listarReagentes("");
    bool encontrouAlgum = false;

    std::cout << "\n";
    std::cout << std::left << std::setw(25) << "Nome" << std::setw(15) << "Detalhe" << "\n";
    std::cout << "\n";

    if (op == 1) {
        std::cout << "Digite 1 para Líquidos ou 2 para Sólidos: ";
        int tipo;
        std::cin >> tipo;

        for (Reagente* r : todos) {
            // Verifica se o reagente atual é liquido ou solido
            ReagenteLiquido* liq = dynamic_cast<ReagenteLiquido*>(r);
            ReagenteSolido* sol = dynamic_cast<ReagenteSolido*>(r);

            if (tipo == 1 && liq != nullptr) {
                std::cout << std::left << std::setw(25) << r->getNome() << "Líquido\n";
                encontrouAlgum = true;
            } else if (tipo == 2 && sol != nullptr) {
                std::cout << std::left << std::setw(25) << r->getNome() << "Sólido\n";
                encontrouAlgum = true;
            }
        }
    } else if (op == 2) {
        // Filtro por Restricao (Restrito vs Livre)
        std::cout << "Digite 1 para ver Reagentes Restritos\n";
        std::cout << "Digite 2 para ver Reagentes Livres\n";
        std::cout << "Opção: ";
        int escolha;
        std::cin >> escolha;

        for (Reagente* r : todos) {
            bool ehRestrito = (r->getNivelAcesso() == 1);

            if (escolha == 1) {
                // O usuario quer ver restritos.
                if (ehRestrito) {
                     std::cout << std::left << std::setw(25) << r->getNome() << "RESTRITO\n";
                     encontrouAlgum = true;
                }
            } else if (escolha == 2) {
                // O usuario quer ver os livres
                if (!ehRestrito) {
                     std::cout << std::left << std::setw(25) << r->getNome() << "Livre\n";
                     encontrouAlgum = true;
                }
            }
        }
    }

    if (!encontrouAlgum) {
        std::cout << "Nenhum reagente encontrado com este filtro.\n";
    }
    std::cout << "\n";
}

//gerenciarLaboratorio
void Gestor::gerenciarLaboratorio(){
    if(this->laboratorio == nullptr){
        std::cout << "Erro: Gestor não está vinculado a nenhum laboratório.\n";
        return;
    }

    int opcao = 0;
    do{
        std::cout << "\n=== GERENCIAMENTO DO LABORATÓRIO ===\n";
        std::cout << "Laboratório: " << laboratorio->getNome() << "\n";
        std::cout << "1. Cadastrar Reagente\n";
        std::cout << "2. Listar Reagentes\n";
        std::cout << "3. Editar Reagente\n";
        std::cout << "4. Excluir Reagente\n";
        std::cout << "5. Filtrar Reagentes (Categoria/nivel)\n";
        std::cout << "6. Retirar Reagente\n";
        std::cout << "7. Histórico de Retiradas\n";
        std::cout << "8. Desassociar Estudante\n";
        std::cout << "9. Sair do Laboratório\n";
        std::cout << "0. Voltar ao menu anterior\n";
        std::cout << "Escolha uma opção: ";
        std::cin >> opcao;

        switch (opcao){
            case 1: this->cadastrarReagente(); break;
            case 2: this->listarReagentesDoLaboratorio(); break;
            case 3: this->editarReagente(); break;
            case 4: this->excluirReagente(); break;
            case 5: this->filtrarReagentes(); break;
            case 6: this->retirarReagente(); break;
            case 7: this->historicoRetiradas(); break;
            case 8: this->desassociarEstudantes(); break;
            case 9: this->sairLaboratorio(); break;
            case 0: std::cout << "Retornando..\n"; break;
            default: std::cout << "Opção inválida. Tente novamente.\n";
        }
    } while(opcao != 0);
}

// retirarReagente
void Gestor::retirarReagente() {
    if (this->laboratorio == nullptr) {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== RETIRADA DE REAGENTE ===\n";

    // Listar reagentes disponíveis
    std::vector<Reagente*> reagentes = laboratorio->listarReagentes("");
    if (reagentes.empty()) {
        std::cout << "Nenhum reagente cadastrado no laboratório.\n";
        return;
    }

    std::cout << "Reagentes disponíveis:\n";
    for (size_t i = 0; i < reagentes.size(); i++) {
        std::cout << i+1 << ". " << reagentes[i]->getNome()
                  << " (ID: " << reagentes[i]->getId()
                  << ") - Quantidade: " << reagentes[i]->getQuantidade()
                  << " " << reagentes[i]->getUnidadeMedida()
                  << " - Local: " << reagentes[i]->getLocalArmazenamento() << std::endl;
    }

    int escolha;
    double quantidade;
    std::cout << "\nEscolha o número do reagente: ";
    std::cin >> escolha;

    if (escolha < 1 || escolha > static_cast<int>(reagentes.size())) {
        std::cout << "Escolha inválida.\n";
        return;
    }

    Reagente* reagenteEscolhido = reagentes[escolha - 1];

    // Verificar se há quantidade suficiente
    if (reagenteEscolhido->getQuantidade() <= 0) {
        std::cout << "Este reagente está com estoque zerado.\n";
        return;
    }

    std::cout << "Quantidade a retirar (" << reagenteEscolhido->getUnidadeMedida()
              << ") - Disponível: " << reagenteEscolhido->getQuantidade() << ": ";
    std::cin >> quantidade;

    // Validar quantidade
    if (quantidade <= 0) {
        std::cout << "Quantidade deve ser maior que zero.\n";
        return;
    }

    if (quantidade > reagenteEscolhido->getQuantidade()) {
        std::cout << "Quantidade indisponível. Disponível: "
                  << reagenteEscolhido->getQuantidade() << "\n";
        return;
    }

    // Confirmação
    std::cout << "\n--- CONFIRMAÇÃO DE RETIRADA ---\n";
    std::cout << "Reagente: " << reagenteEscolhido->getNome() << std::endl;
    std::cout << "Quantidade: " << quantidade << " " << reagenteEscolhido->getUnidadeMedida() << std::endl;
    std::cout << "Local: " << reagenteEscolhido->getLocalArmazenamento() << std::endl;
    std::cout << "Gestor: " << this->getNome() << std::endl;

    if (!confirmacao()) {
        std::cout << "Retirada cancelada.\n";
        return;
    }

    try {
        // Atualizar quantidade no banco
        Table reagenteTable = db->getTable("Reagente");
        int novaQuantidade = reagenteEscolhido->getQuantidade() - quantidade;

        reagenteTable.update()
            .set("quantidade", novaQuantidade)
            .where("id = :id")
            .bind("id", reagenteEscolhido->getId())
            .execute();

        // Atualizar na memória
        reagenteEscolhido->setQuantidade(novaQuantidade);

        // Registrar a retirada (se a tabela existir)
        try {
            Table retiradaTable = db->getTable("Retirada");
            retiradaTable.insert("reagente_id", "usuario_id", "quantidadeRetirada", "dataHoraRetirada")
                .values(reagenteEscolhido->getId(), this->getId(), quantidade, "NOW()")
                .execute();
        } catch (...) {
            // Tabela Retirada pode não existir, continuar normalmente
        }

        std::cout << "Retirada registrada com sucesso!\n";
        std::cout << "Nova quantidade: " << novaQuantidade << " " << reagenteEscolhido->getUnidadeMedida() << std::endl;

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao registrar retirada: " << err.what() << std::endl;
    }
}

void Gestor::sairLaboratorio() {
    if (this->laboratorio == nullptr) {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== SAIR DO LABORATÓRIO ===\n";
    std::cout << "Laboratório atual: " << laboratorio->getNome() << std::endl;
    std::cout << "Departamento: " << laboratorio->getDepartamento() << std::endl;
    std::cout << "Gestor: " << this->getNome() << std::endl;

    if (!confirmacao()) {
        std::cout << "Operação cancelada.\n";
        return;
    }

    try {
        // Atualizar no banco de dados - remover associação
        Table gestorTable = db->getTable("Gestor");
        gestorTable.update()
            .set("laboratorio_id", mysqlx::nullvalue)
            .where("id = :id")
            .bind("id", this->getId())
            .execute();

        this->laboratorio = nullptr;

        std::cout << "Gestor removido do laboratório com sucesso!\n";
    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao sair do laboratório: " << err.what() << std::endl;
    }
}

void Gestor::historicoRetiradas() {
    if (this->laboratorio == nullptr) {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== HISTÓRICO DE RETIRADAS ===\n";

    try {
        std::vector<int> idsReagentes = laboratorio->getIdsReagentesDoLaboratorio();
        
        if (idsReagentes.empty()) {
            std::cout << "Nenhum reagente encontrado neste laboratório.\n";
            return;
        }
        std::string filtro = "";
        for (int i = 0; i < idsReagentes.size(); i++) {
            filtro += "reagente_id = " + std::to_string(idsReagentes[i]);
            if (i < idsReagentes.size() - 1)
                filtro += " OR ";
        }

        Table retiradaTable = db->getTable("Retirada");
        RowResult res = retiradaTable
        .select("reagente_id", "usuario_id", "quantidadeRetirada", "dataHoraRetirada")
        .where(filtro)
        .orderBy("dataHoraRetirada DESC")
        .execute();

        std::cout << std::left
                  << std::setw(25) << "Reagente"
                  << std::setw(20) << "Usuário"
                  << std::setw(12) << "Quantidade"
                  << std::setw(20) << "Data/Hora"
                  << "\n";
        std::cout << std::string(77, '-') << "\n";

        for (Row row : res) {
            int idReagente = row[0].get<int>();
            int idUsuario = row[1].get<int>();
            double qtd = row[2].get<double>();
            std::string dataHora = row[3].get<std::string>();

            // Buscar nome e unidade do reagente
            Table reagenteTable = db->getTable("Reagente");
            RowResult r1 = reagenteTable
                .select("nome", "unidadeMedida")
                .where("id = :id")
                .bind("id", idReagente)
                .execute();

            std::string nomeReagente = "N/A";
            std::string unidade = "";

            if (r1.count() > 0) {
                Row rx = r1.fetchOne();
                nomeReagente = rx[0].get<std::string>();
                unidade = rx[1].get<std::string>();
            }

            // Buscar nome do usuário
            Table usuarioTable = db->getTable("Usuario");
            RowResult r2 = usuarioTable
                .select("nome")
                .where("id = :id")
                .bind("id", idUsuario)
                .execute();

            std::string nomeUsuario = "N/A";

            if (r2.count() > 0) {
                Row ux = r2.fetchOne();
                nomeUsuario = ux[0].get<std::string>();
            }

            // Imprimir linha
            std::cout << std::left
                << std::setw(25) << nomeReagente
                << std::setw(20) << nomeUsuario
                << std::setw(12) << (std::to_string(qtd) + " " + unidade)
                << std::setw(20) << dataHora
                << "\n";
        }

        std::cout << std::string(77, '-') << "\n";
        std::cout << "Total de retiradas: " << res.count() << "\n";

    } catch (const mysqlx::Error& err) {
        std::cerr << "Erro ao consultar histórico: " << err.what() << std::endl;
    }
}


// desassociarEstudantes
void Gestor::desassociarEstudantes() {
    if (this->laboratorio == nullptr) {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== DESASSOCIAR ESTUDANTE ===\n";

    // Buscar estudantes associados a este laboratório
    std::vector<Estudante*> estudantesAssociados;

    try {
        Table associadoTable = db->getTable("Associado");
        RowResult result = associadoTable.select("estudante_id")
            .where("laboratorio_id = :lab_id")
            .bind("lab_id", laboratorio->getId())
            .execute();

        for (Row row : result) {
            int estudanteId = row[0].get<int>();

            // Buscar estudante
            for (int i = 0; i < estudantes.size(); i++) {
                if (estudantes[i]->getId() == estudanteId) {
                    estudantesAssociados.push_back(estudantes[i]);
                    break;
                }
            }
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao buscar estudantes associados: " << err.what() << std::endl;
        return;
    }

    if (estudantesAssociados.empty()) {
        std::cout << "Nenhum estudante associado a este laboratório.\n";
        return;
    }

    // Listar estudantes
    std::cout << "Estudantes associados:\n";
    std::cout << std::left
              << std::setw(5) << "Nº"
              << std::setw(20) << "Nome"
              << std::setw(25) << "Email"
              << std::setw(15) << "Curso"
              << "\n";
    std::cout << std::string(65, '-') << "\n";

    for (size_t i = 0; i < estudantesAssociados.size(); i++) {
        std::cout << std::left
                  << std::setw(5) << (i+1)
                  << std::setw(20) << estudantesAssociados[i]->getNome()
                  << std::setw(25) << estudantesAssociados[i]->getEmail()
                  << std::setw(15) << estudantesAssociados[i]->getCurso()
                  << "\n";
    }
    std::cout << std::string(65, '-') << "\n";

    int escolha;
    std::cout << "\nEscolha o número do estudante para desassociar: ";
    std::cin >> escolha;

    if (escolha < 1 || escolha > static_cast<int>(estudantesAssociados.size())) {
        std::cout << "Escolha inválida.\n";
        return;
    }

    Estudante* estudanteEscolhido = estudantesAssociados[escolha - 1];

    // Confirmação
    std::cout << "\n--- CONFIRMAÇÃO DE DESASSOCIAÇÃO ---\n";
    std::cout << "Estudante: " << estudanteEscolhido->getNome() << std::endl;
    std::cout << "Email: " << estudanteEscolhido->getEmail() << std::endl;
    std::cout << "Curso: " << estudanteEscolhido->getCurso() << std::endl;
    std::cout << "Laboratório: " << laboratorio->getNome() << std::endl;

    if (!confirmacao()) {
        std::cout << "Desassociação cancelada.\n";
        return;
    }

    try {
        // Remover da tabela Associado
        Table associadoTable = db->getTable("Associado");
        associadoTable.remove()
            .where("estudante_id = :est_id AND laboratorio_id = :lab_id")
            .bind("est_id", estudanteEscolhido->getId())
            .bind("lab_id", laboratorio->getId())
            .execute();

        std::cout << "Estudante desassociado com sucesso!\n";

    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro ao desassociar estudante: " << err.what() << std::endl;
    }
}
