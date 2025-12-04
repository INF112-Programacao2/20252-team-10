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

bool confirmacao()
{
    std::cout << "Tem certeza em realizar esta ação? Digite S para Sim e N para Não: ";
    char resposta;
    std::cin >> resposta;
    return (resposta == 'S' || resposta == 's');
}

std::vector<Usuario *> Gestor::usuariosCarregados; // Usuario
std::vector<Gestor *> Gestor::gestores;            // Gestor
std::vector<Estudante *> Gestor::estudantes;       // Estudante
std::vector<PosGraduacao *> Gestor::posGraduandos; // Pos

// Construtor
Gestor::Gestor(std::string nome, std::string email, std::string senha, int nivelAcesso, Schema *db) : Usuario(nome, email, senha, nivelAcesso, db)
{
    // Inicializa o ponteiro como nulo
    this->laboratorio = nullptr;
}

// Destrutor
Gestor::~Gestor()
{
}

// Getters
// Retorna o laboratório que o gestor está alocado
Laboratorio *Gestor::getLaboratorio()
{
    return this->laboratorio;
};

// Demais funções
void Gestor::cadastrarUsuario()
{
    // Pega os dados básicos do novo usuário
    std::cout << "\nQual tipo de usuario deseja cadastrar:\n";
    std::cout << "1. Gestor\n2. Pos-Graduando\n3. Aluno de Graduacao\n";
    int nivelAcesso; // Nivel de acesso aos Reagentes, 1 = Gestor, 2 = Graduacao e 3 = pos Graduacao
    // Loop para verificar se o numero é aceito

    while (true)
    {
        std::cout << "Digite o tipo numérico do usuário (1 a 3): ";
        std::cin >> nivelAcesso;
        // Verifica se entrada é inválida (letra, símbolo, etc.)
        if (std::cin.fail())
        {
            std::cin.clear(); // limpa erro
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada inválida! Digite apenas números entre 1 e 3.\n";
            continue; // Roda o loop novamente
        }
        // Verifica se está no intervalo
        if (nivelAcesso >= 1 && nivelAcesso <= 3)
        {
            break; // sai do loop
        }
        std::cout << "Valor fora do intervalo! Digite apenas 1, 2 ou 3.\n";
    }
    // Limpa o buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\n====== INFORMAÇÕES DO NOVO USUÁRIO =======";
    std::string nome, email, senha;
    std::string matricula, curso, nivel;
    int nivelInt; // Nivel do estudante (Graducao ou Pos-Graduacao)

    // Solicita os dados do usuario
    // Solicita o nome
    while (true)
    {
        std::cout << "\nDigite o nome do usuário: ";
        std::getline(std::cin, nome);
        // Se estiver vazio, retorna o erro
        if (nome.empty())
        {
            std::cout << "O nome não pode estar vazio. Tente novamente.\n";
            continue;
        }
        break; // Nome válido
    }

    // Solicita o email e valida
    while (true)
    {
        std::cout << "Digite o email do usuário: ";
        std::getline(std::cin, email);
        try
        {
            Usuario::validarEmail(email); // Pode lançar exceção
            break;                        // Email válido
        }
        catch (const std::exception &e)
        {
            std::cout << "Email inválido: " << e.what() << "\nTente novamente.\n";
        }
    }

    // Solicita a senha e valida
    while (true)
    {
        std::cout << "Digite a senha do usuário: ";
        std::getline(std::cin, senha);
        try
        {
            Usuario::validarSenha(senha); // Pode lançar exceção
            break;                        // Senha válida
        }
        catch (const std::exception &e)
        {
            std::cout << "Senha inválida: " << e.what() << "\nTente novamente.\n";
        }
    }
    if (nivelAcesso == 2 || nivelAcesso == 3)
    {
        std::cout << "Digite a matricula do usuário " << nome << " :";
        std::getline(std::cin, matricula);
        while (matricula.empty())
        {
            std::cout << "A matrícula não pode estar vazia. Digite novamente: ";
            std::getline(std::cin, matricula);
        }

        std::cout << "Digite o curso do usuário " << nome << " :";
        std::getline(std::cin, curso);

        while (curso.empty())
        {
            std::cout << "O curso não pode estar vazio. Digite novamente: ";
            std::getline(std::cin, curso);
        }

        if (nivelAcesso == 2)
        {
            while (true)
            {
                std::cout << "Informe o nivel de formação:\n";
                std::cout << "1 - Mestrado\n";
                std::cout << "2 - Doutorado\n";
                std::cout << "3 - Pos-Doutorado\n";
                std::cout << "Opcao: ";
                std::cin >> nivelInt;

                // Verifica erro de entrada (quando não é número)
                if (std::cin.fail())
                {
                    std::cout << "Entrada invalida! Digite apenas numeros.\n\n";
                    std::cin.clear();                                                   // limpa o erro
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // descarta lixo
                    continue;
                }

                switch (nivelInt)
                {
                case 1:
                    nivel = "Mestrado";
                    break;
                case 2:
                    nivel = "Doutorado";
                    break;
                case 3:
                    nivel = "PosDoutorado";
                    break;
                default:
                    std::cout << "Opção inválida! Tente novamente\n\n";
                    continue; // volta para o loop
                }
                break; // se é valido, sai do loop
            }
        }
    }
    bool confirma = confirmacao();
    if (confirma)
    {
        try
        {
            Table usuarioTable = db->getTable("Usuario");                             // Insere na tabela Usuario (base)
            Result res = usuarioTable.insert("nome", "email", "senha", "nivelAcesso") // Insere os dados básicos
                             .values(nome, email, senha, nivelAcesso)
                             .execute(); // Define os valores e executa a inserção

            // Recupera o ID do usuário recém-inserido
            int usuarioId = res.getAutoIncrementValue();

            // Insere nas tabelas especializadas conforme o nivelAcesso
            if (nivelAcesso == 1)
            {                                                 // Gestor
                db->getTable("Gestor")                        // Insere na tabela Gestor
                    .insert("id", "cadastrado_por_gestor_id") // Id do gestor e quem cadastrou
                    .values(usuarioId, getId())               // quem cadastrou é o gestor atual
                    .execute();
                Gestor *g = new Gestor(nome, email, senha, nivelAcesso, db);
                g->setId(usuarioId);
                g->setCadastradoPor(this);
                gestores.push_back(g);
                usuariosCarregados.push_back(g);
                std::cout << "Gestor " << nome << " cadastrado com sucesso!\n"; // Mensagem de sucesso
            }
            else if (nivelAcesso == 2 || nivelAcesso == 3)
            {                                                                                // Pos-Graduando ou Aluno de Graduacao
                db->getTable("Estudante")                                                    // Insere na tabela Estudante
                    .insert("id", "matricula", "curso", "nivel", "cadastrado_por_gestor_id") // Dados do estudante
                    .values(usuarioId, matricula, curso, "Graduacao", getId())               // quem cadastrou é o gestor atual
                    .execute();                                                              // Executa a inserção

                if (nivelAcesso == 2)
                {                                // Pos-Graduando
                    db->getTable("PosGraduacao") // Insere na tabela PosGraduacao
                        .insert("id")            // Apenas o ID do usuário
                        .values(usuarioId)       // Define o valor do ID
                        .execute();              // Executa a inserção
                    PosGraduacao *p = new PosGraduacao(nome, email, senha, nivelAcesso, db, matricula, curso, nivel);
                    p->setId(usuarioId);
                    p->setCadastradoPor(this);
                    posGraduandos.push_back(p);
                    usuariosCarregados.push_back(p);
                    std::cout << "Pós-Graduando " << nome << " cadastrado com sucesso!\n"; // Mensagem de sucesso
                }
                else
                {
                    Estudante *grad = new Estudante(nome, email, senha, nivelAcesso, db, matricula, curso, nivel);
                    grad->setId(usuarioId);
                    grad->setCadastradoPor(this);
                    estudantes.push_back(grad);
                    usuariosCarregados.push_back(grad);
                    std::cout << "Aluno de Graduação " << nome << " cadastrado com sucesso!\n"; // Mensagem de sucesso
                }
            }
        }
        catch (const mysqlx::Error &err)
        {
            std::cerr << "Erro ao cadastrar usuário: " << err.what() << std::endl;
        }
    }
}

Gestor *Gestor::getGestorById(int id)
{
    for (auto *g : gestores)
    {
        if (g->getId() == id)
            return g;
    }
    return nullptr;
}
// função que carrega os usuários do banco de dados para o array dinâmico
void Gestor::carregarUsuarios(Schema *db)
{

    usuariosCarregados.clear();
    gestores.clear();
    estudantes.clear();
    posGraduandos.clear();

    try
    {
        Table usuarioTable = db->getTable("Usuario");

        RowResult usuarios = usuarioTable
                                 .select("id", "nome", "email", "senha", "nivelAcesso")
                                 .execute();

        // -------------------------------------------------------
        // 1º PASSO — Carregar todos os gestores (OBJETOS)
        // -------------------------------------------------------
        for (Row row : usuarios)
        {
            int id = row[0].get<int>();
            std::string nome = row[1].get<std::string>();
            std::string email = row[2].get<std::string>();
            std::string senha = row[3].get<std::string>();
            int nivel = row[4].get<int>();

            if (nivel == 1)
            {
                Gestor *g = new Gestor(nome, email, senha, nivel, db);
                g->setId(id);
                gestores.push_back(g);
                usuariosCarregados.push_back(g);
            }
        }

        // -------------------------------------------------------
        //  Preencher cadastradoPor dos gestores
        // -------------------------------------------------------
        usuarios = usuarioTable
                       .select("id")
                       .where("nivelAcesso = 1")
                       .execute();

        Table gestorTable = db->getTable("Gestor");

        for (Row row : usuarios)
        {

            int id = row[0].get<int>();

            RowResult gestores = gestorTable
                                     .select("cadastrado_por_gestor_id", "laboratorio_id")
                                     .where("id = :id")
                                     .bind("id", id)
                                     .execute();

            if (gestores.count() == 0)
                continue;

            Row gestor = gestores.fetchOne();
            Gestor *gestorObj = getGestorById(id);
            if (!(gestor[0].isNull()))
            {
                int cadastradoPorId = gestor[0].get<int>();
                Gestor *gestorCriador = getGestorById(cadastradoPorId);
                if (gestorObj)
                    gestorObj->setCadastradoPor(gestorCriador);
            }
            // if (!(gestor[1].isNull()))
            // {
            //     gestorObj->associarLaboratorio();
            // }
        }

        // -------------------------------------------------------
        //  Carregar estudantes e pós
        // -------------------------------------------------------
        usuarios = usuarioTable
                       .select("id", "nome", "email", "senha", "nivelAcesso")
                       .execute();

        for (Row row : usuarios)
        {

            int id = row[0].get<int>();
            std::string nome = row[1].get<std::string>();
            std::string email = row[2].get<std::string>();
            std::string senha = row[3].get<std::string>();
            int nivel = row[4].get<int>();

            if (nivel == 1)
                continue;

            Table estudanteTable = db->getTable("Estudante");

            RowResult rowsEstudante = estudanteTable
                                          .select("matricula", "curso", "nivel", "cadastrado_por_gestor_id")
                                          .where("id = :id")
                                          .bind("id", id)
                                          .execute();

            if (rowsEstudante.count() == 0)
                continue;

            Row estudante = rowsEstudante.fetchOne();

            std::string matricula = estudante[0].get<std::string>();
            std::string curso = estudante[1].get<std::string>();
            std::string nivelEst = estudante[2].get<std::string>();
            int cadastradoPorId = estudante[3].get<int>();

            Gestor *gestorCriador = getGestorById(cadastradoPorId);

            if (nivel == 2)
            {
                PosGraduacao *pg = new PosGraduacao(
                    nome, email, senha, nivel, db,
                    matricula, curso, nivelEst);
                pg->setId(id);
                pg->setCadastradoPor(gestorCriador);
                posGraduandos.push_back(pg);
                usuariosCarregados.push_back(pg);
            }
            else
            {
                Estudante *est = new Estudante(
                    nome, email, senha, nivel, db,
                    matricula, curso, nivelEst);
                est->setId(id);
                est->setCadastradoPor(gestorCriador);
                estudantes.push_back(est); // AGORA USA O VECTOR CORRETO
                usuariosCarregados.push_back(est);
            }
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao carregar usuários: " << err.what() << std::endl;
    }
}

// lista os usuários do sistema
void Gestor::listarUsuarios()
{

    // cabeçalho da tabela
    std::cout << "\n=============== USUÁRIOS ===============" << std::endl;
    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(20) << "Nome"
              << std::setw(25) << "Email"
              << std::setw(15) << "Tipo"
              << "\n";
    std::cout << std::string(65, '-') << "\n";
    // laco que percorre o vetor de usuarios do laboratorio
    for (Usuario *u : Gestor::usuariosCarregados)
    {
        std::string tipo;
        if (u->getNivelAcesso() == 1)
            tipo = "Gestor";
        else if (u->getNivelAcesso() == 2)
            tipo = "Pos-Graduação";
        else if (u->getNivelAcesso() == 3)
            tipo = "Graduação";
        else
            tipo = "Desconhecido";
        std::cout << std::left
                  << std::setw(5) << u->getId()
                  << std::setw(20) << u->getNome()
                  << std::setw(25) << u->getEmail()
                  << std::setw(15) << tipo
                  << "\n";
    }
    std::cout << std::string(65, '-') << "\n";
}

void Gestor::deletarUsuario()
{
    // Pede o email dentro da função
    std::string email;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer
    std::cout << "Digite o email do usuário a ser deletado: ";
    std::getline(std::cin, email);
    try
    {
        // Verifica Gestores
        for (size_t i = 0; i < gestores.size(); ++i)
        {
            if (gestores[i]->getEmail() == email)
            {
                if (!confirmacao())
                {
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

                for (size_t j = 0; j < usuariosCarregados.size(); ++j)
                {
                    if (usuariosCarregados[j]->getEmail() == email)
                    {
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
        for (size_t i = 0; i < estudantes.size(); ++i)
        {
            if (estudantes[i]->getEmail() == email)
            {
                if (!confirmacao())
                {
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

                for (size_t j = 0; j < usuariosCarregados.size(); ++j)
                {
                    if (usuariosCarregados[j]->getEmail() == email)
                    {
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
        for (size_t i = 0; i < posGraduandos.size(); ++i)
        {
            if (posGraduandos[i]->getEmail() == email)
            {
                if (!confirmacao())
                {
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

                for (size_t j = 0; j < usuariosCarregados.size(); ++j)
                {
                    if (usuariosCarregados[j]->getEmail() == email)
                    {
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
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao deletar usuário: " << err.what() << std::endl;
    }
}

// Associa gestor ao laboratorio
void Gestor::associarLaboratorio()
{
    // Verifica se o gestor já está associado a um laboratório
    if (this->laboratorio != nullptr)
    {
        std::cout << "O gestor já está associado em laboratório: "
                  << this->laboratorio->getNome() << std::endl; // E informa qual
        return;
    }

    // Verifica se no DB esta associado
    Table table = this->db->getTable("Gestor");       // Obtém a tabela "Gestor"
    RowResult result = table.select("laboratorio_id") // Seleciona a coluna "laboratorio_id"
                           .where("id = :id")         // Filtra pelo ID do gestor
                           .bind("id", this->getId()) // Substitui o parâmetro ":id"
                           .execute();                // Executa a consulta
    Row row = result.fetchOne();                      // Busca a primeira linha do resultado
    if (row && !(row[0].isNull()))
    { // existe laboratório no banco
        int idLaboratorioBD = row[0].get<int>();
        // Caso memória não esteja sincronizada, for divergente do bd e do objeto
        if (this->laboratorio == nullptr || this->laboratorio->getId() != idLaboratorioBD)
        {
            this->laboratorio = nullptr; // Garante que não haja um lab antigo se não encontrar o novo
            // Percorre todos os laboratórios carregados em memória
            for (int i = 0; i < Laboratorio::laboratorios.size(); i++)
            {
                if (Laboratorio::laboratorios[i]->getId() == idLaboratorioBD)
                {
                    this->laboratorio = Laboratorio::laboratorios[i]; // Atualiza objeto em memória
                    break;                                            // Sai do loop após encontrar
                }
            }
        }

        if (this->laboratorio)
        {
            // Imprime informação sobre a associação existente
            std::cout << "O gestor já está associado a um laboratório no banco de dados (ID: "
                      << idLaboratorioBD << " - " << this->laboratorio->getNome() << ").\n";
        }
        else
        {
            std::cout << "AVISO: O gestor (ID: " << this->getId() << ") está associado a um laboratório (ID: " << idLaboratorioBD
                      << ") que não foi encontrado na memória. A associação não foi carregada.\n";
        }
        return; // Sai do método, não associa novamente
    }

    // Verifica se há laboratorios instanciados
    if (Laboratorio::laboratorios.empty())
    {
        std::cout << "Nenhum laboratório carregado. \n";
        return;
    }
    // Imprime os laboratórios cadastrados
    Laboratorio::imprimirLaboratorios();

    // Usuario escolhe qual laboratorio associar
    int id;
    std::cout << "\nDigite o ID do laboratório que deseja gerenciar: ";
    std::cin >> id;

    // Variavel armazena o laboratorio escolhido pelo gestor
    Laboratorio *escolhido = nullptr;
    // Busca em laboratorios cadastrados baseado no ID
    for (int i = 0; i < Laboratorio::laboratorios.size(); i++)
    {
        Laboratorio *laboratorio = Laboratorio::laboratorios[i]; // armazena o ponteiro do laboratorio na posicao i
        if (laboratorio->getId() == id)
        {                            // Quando o id do escolhido for igual dos armazenado
            escolhido = laboratorio; // armazena o ponteiro
            break;                   // Para a iteração
        }
    }
    if (escolhido == nullptr)
    {
        std::cout << "Laboratório não encontrado.\n";
        return;
    }

    // Confirmação antes de associar
    if (!confirmacao())
    {
        std::cout << "Associação cancelada pelo usuário.\n";
        return;
    }

    // Associa ao gestor, armazena no objeto
    this->laboratorio = escolhido;
    // Adiciona o gestor dentro do laboratorio (armazena no vetor de gestores)
    escolhido->adicionarGestor(this);

    // Atualiza no DB
    table.update()
        .set("laboratorio_id", id) // Define o novo ID do laboratório
        .where("id = :id")         // Aplica a atualização ao gestor correto
        .bind("id", this->getId()) // Substitui o parâmetro ":id"
        .execute();                // Executa a atualização
    // Confirmação para o usuário
    std::cout << "\nGestor gerencia o laboratório: "
              << escolhido->getNome()
              << std::endl;
}

void Gestor::associarEstudanteAoLaboratorio(Estudante *estudante, int idLaboratorio, const std::string &papel)
{
    Laboratorio *escolhido = nullptr;
    for (int i = 0; i < (int)Laboratorio::laboratorios.size(); i++)
    {
        if (Laboratorio::laboratorios[i]->getId() == idLaboratorio)
        {
            escolhido = Laboratorio::laboratorios[i];
            break;
        }
    }

    if (!escolhido)
    {
        std::cout << "Laboratório com ID " << idLaboratorio << " não encontrado.\n";
        return;
    }

    Table tableAssociado = this->db->getTable("Associado");
    RowResult result = tableAssociado.select("*")
                           .where("estudante_id = :e_id AND laboratorio_id = :l_id")
                           .bind("e_id", estudante->getId())
                           .bind("l_id", idLaboratorio)
                           .execute();

    if (!result.fetchOne().isNull())
    {
        std::cout << "Estudante ID " << estudante->getId()
                  << " já está associado a este laboratório.\n";
        return;
    }

    tableAssociado.insert("estudante_id", "laboratorio_id", "papel")
        .values(estudante->getId(), idLaboratorio, papel)
        .execute();

    estudante->adicionarLaboratorio(escolhido, papel);
    escolhido->adicionarEstudante(estudante);
}
// Metodo para cadastrar reagente
void Gestor::cadastrarReagente()
{
    // Verifica se o ponteiro de laboratorio nao e nulo
    // Isso garante que o gestor esteja vinculado a um laboratorio antes de cadastrar
    if (this->laboratorio == nullptr)
    {
        std::cerr << "ERRO Gestor sem laboratorio associado" << std::endl;
        return;
    }

    // Verifica se o ponteiro do banco de dados nao e nulo
    if (this->db == nullptr)
    {
        std::cerr << "ERRO Sem conexao com banco de dados" << std::endl;
        return;
    }

    // Declaracao das variaveis locais que irao armazenar os dados do reagente
    std::string nome, dataValidade, local, unidade, marca, codRef;
    int quantidade, quantidadeCritica, nivelAcesso;

    std::cout << "\n Cadastro de Novo Reagente \n";

    // Limpa o buffer de entrada removendo o enter anterior
    std::cin.ignore();

    // Inicio do loop para validacao do Nome
    // O loop continua indefinidamente ate que um nome valido seja inserido
    while (true)
    {
        std::cout << "Nome: ";
        // Le a linha inteira digitada pelo usuario inclusive espacos
        std::getline(std::cin, nome);

        // Verifica se a string nao esta vazia
        // Se contiver texto sai do loop com break
        if (!nome.empty())
        {
            break;
        }
        // Se estiver vazia exibe mensagem de erro e repete o loop
        std::cout << "O nome nao pode ser vazio Tente novamente\n";
    }

    // Declaracao de variaveis para a validacao da data
    int ano, mes, dia;
    char sep1, sep2; // Variaveis para capturar os caracteres separadores hifens

    // Inicio do loop para validacao da Data de Validade
    while (true)
    {
        std::cout << "Escreva a data de Validade (AAAA-MM-DD), na ordem ano-mês-dia: ";

        // Tenta ler a entrada no formato exato inteiro char inteiro char inteiro
        std::cin >> ano >> sep1 >> mes >> sep2 >> dia;

        // Verifica se houve falha na leitura dos numeros, exemplo o usuario digitar letras
        if (std::cin.fail())
        {
            std::cout << "Erro Digite apenas numeros para ano mes e dia\n";
            std::cin.clear();  // Limpa o estado de erro do cin
            std::cin.ignore(); // Limpa o buffer
            continue;          // Retorna ao inicio do loop
        }

        // Verifica se os caracteres separadores sao hifens
        if (sep1 != '-' || sep2 != '-')
        {
            std::cout << "Erro formato incorreto use hifens ex 2025-12-31\n";
            // Limpa o resto da linha caso tenha sobrado lixo no buffer
            std::cin.ignore();
            continue;
        }

        // Verifica os limites logicos do Ano e do Mes
        if (ano < 1900 || ano > 2100)
        {
            std::cout << "Erro Ano deve ser entre 1900 e 2100\n";
            continue;
        }
        if (mes < 1 || mes > 12)
        {
            std::cout << "Erro Mes deve ser entre 1 e 12\n";
            continue;
        }

        // Verifica limite minimo dos Dias
        if (dia < 1)
        {
            std::cout << "Erro Dia invalido\n";
            continue;
        }

        // Logica especifica para verificar dias do mes de Fevereiro
        if (mes == 2)
        {
            // Calcula se o ano e bissexto regra divisivel por 4 e nao por 100 ou divisivel por 400 (prog 1)
            bool bissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
            // Define o limite de dias com base no ano bissexto
            int limite = bissexto ? 29 : 28;

            if (dia > limite)
            {
                std::cout << "Erro Dia invalido para Fevereiro neste ano\n";
                continue;
            }
        }
        // Verifica limites para meses com 30 dias Abril Junho Setembro Novembro
        else if (mes == 4 || mes == 6 || mes == 9 || mes == 11)
        {
            if (dia > 30)
            {
                std::cout << "Erro Este mes tem apenas 30 dias\n";
                continue;
            }
        }
        // Verifica limites para os demais meses que tem 31 dias
        else
        {
            if (dia > 31)
            {
                std::cout << "Erro Dia invalido Maximo 31\n";
                continue;
            }
        }

        // Reconstrutroi a string Data para salvar no banco de dados posteriormente
        // Converte os numeros para texto
        std::string sMes = std::to_string(mes);
        std::string sDia = std::to_string(dia);

        // Se o mes for menor que 10 ex 5 adiciona um 0 antes vira 05
        if (mes < 10)
        {
            sMes = "0" + sMes;
        }

        // Se o dia for menor que 10 ex 9 adiciona um 0 antes vira 09
        if (dia < 10)
        {
            sDia = "0" + sDia;
        }

        // Junta tudo no formato final AAAA-MM-DD
        dataValidade = std::to_string(ano) + "-" + sMes + "-" + sDia;
        // Adiciona as partes na variavel final
        dataValidade = std::to_string(ano) + "-" + sMes + "-" + sDia;

        break; // Sai do loop pois a data esta correta
    }

    // Inicio do loop para validacao da Quantidade
    while (true)
    {
        std::cout << "Quantidade (numero inteiro maior que 0): ";
        std::cin >> quantidade;

        // Verifica se a ultima tentativa de leitura do cin falhou
        if (std::cin.fail())
        {

            // Reseta os estados de erro internos do cin
            std::cin.clear();

            std::cin.ignore(); // Limpa sobra de lixo

            // Informa ao usuario que o dado digitado nao e valido
            std::cout << "Entrada invalida Digite apenas numeros\n";
        }
        // Verifica se o valor e positivo
        else if (quantidade <= 0)
        {
            std::cout << "A quantidade deve ser maior que zero\n";
        }
        else
        {
            break; // Entrada valida
        }
    }

    // Inicio do loop para validacao da Quantidade Critica
    while (true)
    {
        std::cout << "Quantidade Critica (aviso de estoque baixo): ";
        std::cin >> quantidadeCritica;

        // Verifica erro de entrada
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Entrada invalida Digite apenas numeros\n";
        }
        // Verifica se o valor e positivo
        else if (quantidadeCritica <= 0)
        {
            std::cout << "A quantidade critica deve ser maior que zero\n";
        }
        else
        {
            break; // Entrada valida
        }
    }

    std::cin.ignore(); // Limpa sobra de lixo

    // Inicio do loop para validacao do Local de Armazenamento
    while (true)
    {
        std::cout << "Local de Armazenamento: ";
        std::getline(std::cin, local);
        // Verifica se string nao esta vazia
        if (!local.empty())
            break;
        std::cout << "O local nao pode ser vazio\n";
    }

    // Inicio do loop para validacao do Nivel de Acesso
    while (true)
    {
        std::cout << "Nivel de Acesso (1-Restrito 2-Livre 3-Pos): ";
        std::cin >> nivelAcesso;

        // Verifica erro de entrada nao numerica
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Entrada invalida Digite um numero\n";
        }
        // Verifica se esta dentro das opcoes validas 1 2 ou 3
        else if (nivelAcesso < 1 || nivelAcesso > 3)
        {
            std::cout << "Opcao invalida Digite 1 2 ou 3\n";
        }
        else
        {
            break; // Entrada valida
        }
    }

    // Limpa buffer apos leitura de inteiro
    std::cin.ignore();

    // Validacao da Unidade de Medida
    while (true)
    {
        std::cout << "Unidade de Medida (ex ml g): ";
        std::getline(std::cin, unidade);
        if (!unidade.empty())
            break;
        std::cout << "Unidade nao pode ser vazia\n";
    }

    // Validacao da Marca
    while (true)
    {
        std::cout << "Marca: ";
        std::getline(std::cin, marca);
        if (!marca.empty())
            break;
        std::cout << "Marca nao pode ser vazia\n";
    }

    // Validacao do Codigo de Referencia
    while (true)
    {
        std::cout << "Codigo de Referencia: ";
        std::getline(std::cin, codRef);
        if (!codRef.empty())
            break;
        std::cout << "Codigo nao pode ser vazio\n";
    }

    // Inicio do loop para validacao do Tipo do reagente
    int tipo;
    while (true)
    {
        std::cout << "Digite o tipo (1 = Liquido  2 = Solido): ";
        std::cin >> tipo;

        // Verifica entrada invalida
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Entrada invalida\n";
        }
        // Verifica se a opcao digitada e 1 ou 2
        else if (tipo != 1 && tipo != 2)
        {
            std::cout << "Tipo invalido Digite 1 ou 2\n";
        }
        else
        {
            break; // Entrada valida
        }
    }

    // Variaveis especificas que serao preenchidas dependendo do tipo
    double densidade = 0.0;
    double volume = 0.0;
    double massa = 0.0;
    std::string estadoFisico;

    // Bloco logico para Reagente Liquido
    if (tipo == 1)
    {
        // Validacao da Densidade
        while (true)
        {
            std::cout << "Densidade: ";
            std::cin >> densidade;
            // Verifica entrada e valor positivo
            if (!std::cin.fail() && densidade > 0)
                break;

            // Tratamento de erro
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Densidade invalida Digite numero maior que 0\n";
        }
        // Validacao do Volume
        while (true)
        {
            std::cout << "Volume: ";
            std::cin >> volume;
            // Verifica entrada e valor positivo
            if (!std::cin.fail() && volume > 0)
                break;

            // Tratamento de erro
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Volume invalido Digite numero maior que 0\n";
        }
    }
    // Bloco logico para Reagente Solido
    else if (tipo == 2)
    {
        // Validacao da Massa
        while (true)
        {
            std::cout << "Massa: ";
            std::cin >> massa;
            // Verifica entrada e valor positivo
            if (!std::cin.fail() && massa > 0)
                break;

            // Tratamento de erro
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Massa invalida Digite numero maior que 0\n";
        }

        // Limpa buffer antes de ler string
        std::cin.ignore();

        // Validacao do Estado Fisico
        while (true)
        {
            std::cout << "Estado Fisico (ex po cristal): ";
            std::getline(std::cin, estadoFisico);
            if (!estadoFisico.empty())
                break;
            std::cout << "Estado fisico nao pode ser vazio\n";
        }
    }

    // Bloco de Insercao no Banco de Dados protegido por try catch
    try
    {
        // Obtem a tabela Reagente do banco
        Table reagenteTable = db->getTable("Reagente");

        // Executa a insercao dos dados comuns na tabela pai
        Result res = reagenteTable.insert(
                                      "nome", "validade", "quantidade", "quantidadeCritica",
                                      "localArmazenamento", "nivelAcesso", "unidadeMedida", "marca", "referencia", "laboratorio_id")
                         .values(nome, dataValidade, quantidade, quantidadeCritica,
                                 local, nivelAcesso, unidade, marca, codRef, this->laboratorio->getId())
                         .execute();

        // Recupera o ID gerado automaticamente pelo banco para o novo reagente
        int reagenteId = res.getAutoIncrementValue();

        // Insere nas tabelas filhas especificas conforme o tipo
        if (tipo == 1)
        {
            // Insercao na tabela de Liquidos
            db->getTable("ReagenteLiquido")
                .insert("id", "densidade", "volume")
                .values(reagenteId, densidade, volume)
                .execute();
            std::cout << "Reagente Liquido cadastrado com sucesso\n";
        }
        else if (tipo == 2)
        {
            // Insercao na tabela de Solidos
            db->getTable("ReagenteSolido")
                .insert("id", "massa", "estadoFisico")
                .values(reagenteId, massa, estadoFisico)
                .execute();
            std::cout << "Reagente Solido cadastrado com sucesso\n";
        }

        // Atualiza a memoria do sistema instanciando o objeto e adicionando ao vetor
        laboratorio->cadastrarNovoReagente(
            reagenteId, nome, dataValidade, quantidade, quantidadeCritica,
            local, nivelAcesso, unidade, marca, codRef, tipo,
            densidade, volume, massa, estadoFisico);
    }
    catch (const mysqlx::Error &err)
    {
        // Captura excecoes do MySQL e exibe mensagem de erro
        std::cerr << "Erro ao cadastrar reagente no banco " << err.what() << std::endl;
    }
}

void Gestor::acessarReagentesAlerta()
{
    laboratorio->getAlertasGestor();
}

// Metodo para vincular o gestor a um laboratorio
void Gestor::setLaboratorio(Laboratorio *lab)
{
    this->laboratorio = lab;
}

// Implementação da função virtual. O Gestor ignora a checagem de nível.
void Gestor::acessarReagenteRestrito(int idReagente)
{
    std::cout << "\n(Gestor) Acessando Reagente ID: " << idReagente << "\n";
    if (db == nullptr)
    {
        std::cerr << "ERRO: Gestor não está conectado ao banco." << std::endl;
        return;
    }

    try
    {
        Table reagenteTable = db->getTable("Reagente");

        // Busca o reagente pelo ID
        RowResult res = reagenteTable.select(
                                         "id", "nome", "quantidade", "unidadeMedida",
                                         "localArmazenamento", "dataValidade", "nivelAcesso")
                            .where("id = :id")
                            .bind("id", idReagente)
                            .execute();

        if (res.count() == 0)
        {
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
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao acessar reagente: " << err.what() << std::endl;
    }
}

void Gestor::listarReagentesRestritos()
{
    if (db == nullptr)
    {
        std::cerr << "ERRO: Gestor não está conectado ao banco." << std::endl;
        return;
    }

    try
    {
        Table reagenteTable = db->getTable("Reagente");

        // Busca o reagente pelo ID
        // RowResult res = reagenteTable.select(
        //     "id", "nome", "quantidade", "unidadeMedida",
        //     "localArmazenamento", "validade", "nivelAcesso"
        // ).where("nivelAcesso =: nivel").bind("nivel", 1).execute();

        SqlResult res = db->getSession().sql("SELECT id, nome, quantidade, unidadeMedida, localArmazenamento, DATE_FORMAT(validade, '%Y-%m-%d') as data_formatada FROM LabUFV.Reagente WHERE nivelAcesso = 1").execute();

        if (res.count() == 0)
        {
            std::cout << "Reagentes restritos não encontrados\n"
                      << std::endl;
            return;
        }
        int size = res.count();
        for (int i = 0; i < size; i++)
        {
            // Pega os detalhes
            Row row = res.fetchOne();

            // O Gestor imprime tudo (não há checagem de nível)
            std::cout << "Id:      " << row[0].get<int>() << "\n";
            std::cout << "Nome:    " << row[1].get<std::string>() << "\n";
            std::cout << "Qtde:    " << row[2].get<int>() << " " << row[3].get<std::string>() << "\n";
            std::cout << "Local:   " << row[4].get<std::string>() << "\n";
            if (!row[5].isNull())
                std::cout << "Validade: " << row[5].get<std::string>() << "\n";
            std::cout << "-------------------------------------\n";
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao acessar reagente: " << err.what() << std::endl;
    }
}

void Gestor::menuReagentesRestritos()
{
    int opcao = 0;
    do
    {

        std::cout << "===== Menu Reagentes Restritos =====\n";
        std::cout << "1. Listar reagentes restritos\n";
        std::cout << "2. Retirar reagente\n";
        std::cout << "3. Registrar reagente\n";
        std::cout << "0. Sair do sistema\n";
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;

        // Bloco de protecao contra letras no menu
        if (std::cin.fail())
        {
            std::cin.clear(); // Limpa o estado de erro
            std::cin.ignore(); // Ignora o caractere invalido
            opcao = -1; // Forca cair no default
            std::cout << "Entrada invalida Digite apenas numeros\n";
        }
        switch (opcao)
        {
        case 1:
        {
            this->listarReagentesRestritos();
            break;
        }
        case 2:
        {
            if (confirmacao())
            {
                this->retirarReagente();
            }
            else
                std::cout << "Ação cancelada pelo usuário\n";
            break;
        }
        case 3:
        {
            this->cadastrarReagente();
            break;
        }
        case 0:
        {
            std::cout << "Saindo...\n";
            break;
        }
        default:
        {
            std::cout << "Opção inválida! Tente novamente.\n";
        }
        }

    } while (opcao != 0);
}

bool Gestor::estaAssociado() const
{
    return laboratorio != nullptr;
}

void Gestor::listarReagentesDoLaboratorio()
{
    if (laboratorio == nullptr)
        return;

    // Busca todos os reagentes (string vazia retorna todos)
    std::vector<Reagente *> lista = laboratorio->listarReagentes("");

    if (lista.empty())
    {
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
    std::cout << std::string(70, '-') << "\n";

    for (Reagente *r : lista)
    {
        std::cout << std::left
                  << std::setw(5) << r->getId()
                  << std::setw(25) << r->getNome()
                  << std::setw(15) << (std::to_string(r->getQuantidade()) + " " + r->getUnidadeMedida())
                  << std::setw(15) << r->getDataValidade()
                  << std::setw(10) << r->getNivelAcesso() << "\n";
    }
    std::cout << std::string(70, '-') << "\n";
}

void Gestor::editarReagente()
{
    std::cout << "\n Editar Reagente \n";
    std::string nomeBusca;
    
    // Limpa o buffer de entrada para evitar pular a leitura do nome
    std::cin.ignore();
    
    // Inicio do loop para validacao do nome de busca
    while(true) {
        std::cout << "Digite o nome do reagente que deseja editar: ";
        // Le a linha inteira digitada pelo usuario
        std::getline(std::cin, nomeBusca);
        // Verifica se a string nao esta vazia
        if(!nomeBusca.empty()) break;
        std::cout << "Nome invalido\n";
    }

    // Busca o reagente na memoria do laboratorio usando o nome fornecido
    Reagente *reagente = laboratorio->buscarReagente(nomeBusca);

    // Verifica se o reagente foi encontrado ou se retornou nulo
    if (reagente == nullptr)
    {
        std::cout << "Reagente nao encontrado\n";
        return;
    }

    // Exibe os dados atuais do reagente encontrado para o usuario conferir
    std::cout << "Reagente encontrado: " << reagente->getNome() << " (ID: " << reagente->getId() << ")\n";
    std::cout << "O que deseja alterar?\n";
    std::cout << "1. Quantidade (Atual: " << reagente->getQuantidade() << ")\n";
    std::cout << "2. Local de Armazenamento (Atual: " << reagente->getLocalArmazenamento() << ")\n";
    std::cout << "3. Data de Validade (Atual: " << reagente->getDataValidade() << ")\n";
    std::cout << "4. Corrigir Nome (Atual: " << reagente->getNome() << ")\n";
    std::cout << "5. Corrigir Nivel de Acesso (Atual: " << reagente->getNivelAcesso() << ")\n";
    std::cout << "6. Corrigir Dados Especificos (Tipo/Densidade/Massa)\n";
    std::cout << "0. Cancelar\n";
    std::cout << "Opcao: ";

    int subOpcao;
    // Inicio do loop para validacao da opcao do menu
    while(true) {
        std::cin >> subOpcao;
        
        // Verifica se a entrada falhou caso digite letras
        if(!std::cin.fail()) break;
        
        // Limpa o estado de erro do cin
        std::cin.clear();
        // Limpa o buffer de entrada simples
        std::cin.ignore();
        std::cout << "Entrada invalida Digite um numero\n";
    }

    // Se escolheu 0 cancela a operacao e sai da funcao imediatamente
    if (subOpcao == 0) return;

    // Bloco try catch para tratamento de erros de banco de dados
    try
    {
        // Obtem a referencia da tabela Reagente no banco
        Table table = db->getTable("Reagente");

        // Opcao 1 Editar Quantidade
        if (subOpcao == 1) 
        { 
            int novaQtd;
            // Loop de validacao para garantir numero positivo
            while(true) {
                std::cout << "Nova Quantidade: ";
                std::cin >> novaQtd;
                
                // Verifica erro de input e se valor e maior ou igual a zero
                if(!std::cin.fail() && novaQtd >= 0) break;
                
                std::cin.clear(); 
                std::cin.ignore();
                std::cout << "Quantidade invalida Digite numero maior ou igual a 0\n";
            }

            // Pede confirmacao antes de salvar as alteracoes
            if (!confirmacao()) return;

            // Atualiza no banco de dados filtrando pelo ID
            table.update().set("quantidade", novaQtd).where("id = :id").bind("id", reagente->getId()).execute();
            // Atualiza na memoria RAM para manter consistencia
            reagente->setQuantidade(novaQtd);
            std::cout << "Quantidade atualizada com sucesso\n";
        }
        // Opcao 2 Editar Local de Armazenamento
        else if (subOpcao == 2) 
        { 
            std::string novoLocal;
            // Limpa buffer obrigatorio antes de ler string com getline
            std::cin.ignore();
            
            // Loop de validacao de string nao vazia
            while(true) {
                std::cout << "Novo Local: ";
                std::getline(std::cin, novoLocal);
                if(!novoLocal.empty()) break;
                std::cout << "Local nao pode ser vazio\n";
            }

            if (!confirmacao()) return;

            // Atualiza banco e memoria
            table.update().set("localArmazenamento", novoLocal).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setLocalArmazenamento(novoLocal);
            std::cout << "Local atualizado com sucesso\n";
        }
        // Opcao 3 Editar Data de Validade
        else if (subOpcao == 3) 
        {
            int ano, mes, dia;
            char sep1, sep2; 
            std::string novaValidade;

            // Loop de validacao robusta da data igual ao cadastro
            while (true)
            {
                std::cout << "Nova Validade (AAAA-MM-DD): ";
                // Tenta ler no formato exato Numero Char Numero Char Numero
                std::cin >> ano >> sep1 >> mes >> sep2 >> dia;

                // Verifica erro de leitura se digitou texto onde era numero
                if (std::cin.fail()) 
                {
                    std::cout << "Erro Digite apenas numeros\n";
                    std::cin.clear(); 
                    std::cin.ignore(); 
                    continue; 
                }

                // Verifica se usou hifens como separador
                if (sep1 != '-' || sep2 != '-')
                {
                    std::cout << "Erro Formato incorreto Use hifens\n";
                    std::cin.ignore();
                    continue;
                }

                // Verifica limites logicos de ano mes e dia
                if (ano < 1900 || ano > 2100) { std::cout << "Ano invalido\n"; continue; }
                if (mes < 1 || mes > 12) { std::cout << "Mes invalido\n"; continue; }
                if (dia < 1 || dia > 31) { std::cout << "Dia invalido\n"; continue; }

                // Verificacao especifica para Fevereiro e ano bissexto
                if (mes == 2) {
                    bool bissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
                    int limite = bissexto ? 29 : 28;
                    if (dia > limite) { std::cout << "Dia invalido para Fevereiro\n"; continue; }
                }
                // Verificacao para meses com 30 dias
                else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
                    if (dia > 30) { std::cout << "Mes tem apenas 30 dias\n"; continue; }
                }

                // Conversao segura dos numeros para string
                std::string sMes = std::to_string(mes);
                std::string sDia = std::to_string(dia);
                
                // Adiciona zero a esquerda se necessario para manter formato
                if (mes < 10) sMes = "0" + sMes;
                if (dia < 10) sDia = "0" + sDia;
                
                // Monta a string final da data
                novaValidade = std::to_string(ano) + "-" + sMes + "-" + sDia;
                break; 
            }

            if (!confirmacao()) return;

            // Atualiza banco e memoria
            table.update().set("dataValidade", novaValidade).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setDataValidade(novaValidade);
            std::cout << "Validade atualizada com sucesso\n";
        }
        // Opcao 4 Editar Nome
        else if (subOpcao == 4) 
        { 
            std::string novoNome;
            std::cin.ignore(); // Limpa buffer
            while(true) {
                std::cout << "Novo Nome Correto: ";
                std::getline(std::cin, novoNome);
                if(!novoNome.empty()) break;
                std::cout << "Nome invalido\n";
            }
            if (!confirmacao()) return;
            
            table.update().set("nome", novoNome).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setNome(novoNome);
            std::cout << "Nome corrigido com sucesso\n";
        }
        // Opcao 5 Editar Nivel de Acesso
        else if (subOpcao == 5) 
        { 
            int novoNivel;
            while(true) {
                std::cout << "Novo Nivel (1-3): ";
                std::cin >> novoNivel;
                // Valida se e numero e se esta entre 1 e 3
                if(!std::cin.fail() && novoNivel >= 1 && novoNivel <= 3) break;
                std::cin.clear(); 
                std::cin.ignore();
                std::cout << "Nivel invalido\n";
            }
            if (!confirmacao()) return;
            
            table.update().set("nivelAcesso", novoNivel).where("id = :id").bind("id", reagente->getId()).execute();
            reagente->setNivelAcesso(novoNivel);
            std::cout << "Nivel atualizado\n";
        }
        // Opcao 6 Editar Dados Especificos
        // Aqui verificamos se o tipo informado bate com o tipo no banco
        // Se nao bater permitimos a conversao do tipo deletando a tabela antiga e criando na nova
        else if (subOpcao == 6)
        {
            int tipoEscolhido;
            bool precisaConverter = false;

            // Loop para validar o tipo de reagente desejado
            while (true)
            {
                std::cout << "Qual o tipo deste reagente (1-Liquido 2-Solido): ";
                std::cin >> tipoEscolhido;

                // Verifica erro de entrada
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Entrada invalida\n";
                    continue;
                }
                
                // Verifica intervalo
                if (tipoEscolhido != 1 && tipoEscolhido != 2) {
                    std::cout << "Tipo invalido\n";
                    continue;
                }

                // Verifica consistencia com o objeto atual em memoria
                if (tipoEscolhido == 1) {
                    // Tenta converter o ponteiro para liquido
                    // Se falhar retorna null o que significa que e solido
                    ReagenteLiquido *testeLiq = dynamic_cast<ReagenteLiquido *>(reagente);
                    if (testeLiq == nullptr) {
                        std::cout << "\nAVISO O reagente cadastrado atualmente e SOLIDO\n";
                        std::cout << "Deseja converter para LIQUIDO? (S/N): ";
                        char resp;
                        std::cin >> resp;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        
                        if (resp == 'S' || resp == 's') {
                            precisaConverter = true;
                            break; 
                        } else {
                            continue; 
                        }
                    }
                }
                else if (tipoEscolhido == 2) {
                    // Tenta converter o ponteiro para solido
                    // Se falhar retorna null o que significa que e liquido
                    ReagenteSolido *testeSol = dynamic_cast<ReagenteSolido *>(reagente);
                    if (testeSol == nullptr) {
                        std::cout << "\nAVISO O reagente cadastrado atualmente e LIQUIDO\n";
                        std::cout << "Deseja converter para SOLIDO? (S/N): ";
                        char resp;
                        std::cin >> resp;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        if (resp == 'S' || resp == 's') {
                            precisaConverter = true;
                            break; 
                        } else {
                            continue; 
                        }
                    }
                }
                break;
            }

            // CASO 1 CONVERSAO DE TIPO Troca de tabelas
            if (precisaConverter)
            {
                int idOriginal = reagente->getId();
                std::string nome = reagente->getNome();

                if (tipoEscolhido == 1) // Converter para Liquido
                {
                    double dens, vol;
                    // Coleta novos dados
                    while(true) { 
                        std::cout << "Nova Densidade: "; std::cin >> dens; 
                        if(!std::cin.fail() && dens > 0) break; 
                        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalido\n"; 
                    }
                    while(true) { 
                        std::cout << "Novo Volume: "; std::cin >> vol; 
                        if(!std::cin.fail() && vol > 0) break; 
                        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalido\n"; 
                    }

                    // 1 Insere na tabela nova Liquido
                    // Se der erro aqui o catch captura e nada e perdido pois a antiga ainda existe
                    db->getTable("ReagenteLiquido").insert("id", "densidade", "volume").values(idOriginal, dens, vol).execute();

                    // 2 Se deu certo remove da tabela antiga Solido
                    db->getTable("ReagenteSolido").remove().where("id = :id").bind("id", idOriginal).execute();

                    // Atualiza Memoria Remove o objeto antigo e cria um novo do tipo certo
                    laboratorio->removerReagenteDaMemoria(idOriginal);
                    laboratorio->cadastrarNovoReagente(
                        idOriginal, nome, reagente->getDataValidade(), reagente->getQuantidade(), 
                        reagente->getQuantidadeCritica(), reagente->getLocalArmazenamento(), 
                        reagente->getNivelAcesso(), reagente->getUnidadeMedida(), 
                        reagente->getMarca(), reagente->getCodigoReferencia(), 
                        1, dens, vol, 0.0, ""
                    );
                    std::cout << "Convertido para Liquido com sucesso\n";
                }
                else // Converter para Solido
                {
                    double mass;
                    std::string est;
                    // Coleta novos dados
                    while(true) { 
                        std::cout << "Nova Massa: "; std::cin >> mass; 
                        if(!std::cin.fail() && mass > 0) break; 
                        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalido\n"; 
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    while(true) { 
                        std::cout << "Novo Estado Fisico: "; std::getline(std::cin, est); 
                        if(!est.empty()) break; std::cout << "Vazio\n"; 
                    }

                    // 1 Insere na tabela nova Solido
                    // Se der erro aqui o catch captura e nada e perdido pois a antiga ainda existe
                    db->getTable("ReagenteSolido").insert("id", "massa", "estadoFisico").values(idOriginal, mass, est).execute();

                    // 2 Se deu certo remove da tabela antiga Liquido
                    db->getTable("ReagenteLiquido").remove().where("id = :id").bind("id", idOriginal).execute();

                    // Atualiza Memoria Remove antigo e recria novo
                    laboratorio->removerReagenteDaMemoria(idOriginal);
                    laboratorio->cadastrarNovoReagente(
                        idOriginal, nome, reagente->getDataValidade(), reagente->getQuantidade(), 
                        reagente->getQuantidadeCritica(), reagente->getLocalArmazenamento(), 
                        reagente->getNivelAcesso(), reagente->getUnidadeMedida(), 
                        reagente->getMarca(), reagente->getCodigoReferencia(), 
                        2, 0.0, 0.0, mass, est
                    );
                    std::cout << "Convertido para Solido com sucesso\n";
                }
                return; // Sai da funcao pois o ponteiro antigo agora e invalido
            }

            // CASO 2 EDICAO NORMAL Mesmo tipo apenas atualiza valores
            if (tipoEscolhido == 1) // Liquido
            {
                // Cast manual ja que validamos o tipo antes
                ReagenteLiquido *liq = (ReagenteLiquido*) reagente;
                double novaDensidade, novoVolume;
                
                while(true) {
                    std::cout << "Nova Densidade: "; std::cin >> novaDensidade;
                    if(!std::cin.fail() && novaDensidade > 0) break;
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalido\n";
                }
                while(true) {
                    std::cout << "Novo Volume: "; std::cin >> novoVolume;
                    if(!std::cin.fail() && novoVolume > 0) break;
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalido\n";
                }

                if (!confirmacao()) return;

                // Atualiza tabela
                db->getTable("ReagenteLiquido").update()
                    .set("densidade", novaDensidade)
                    .set("volume", novoVolume)
                    .where("id = :id").bind("id", reagente->getId()).execute();

                // Atualiza objeto
                liq->setDensidade(novaDensidade);
                liq->setVolume(novoVolume);
                std::cout << "Atualizado com sucesso\n";
            }
            else // Solido
            {
                // Cast manual ja que validamos o tipo antes
                ReagenteSolido *sol = (ReagenteSolido*) reagente;
                double novaMassa;
                std::string novoEstado;

                while(true) {
                    std::cout << "Nova Massa: "; std::cin >> novaMassa;
                    if(!std::cin.fail() && novaMassa > 0) break;
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalido\n";
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                while(true) {
                    std::cout << "Novo Estado Fisico: "; std::getline(std::cin, novoEstado);
                    if(!novoEstado.empty()) break; std::cout << "Vazio\n";
                }

                if (!confirmacao()) return;

                // Atualiza tabela
                db->getTable("ReagenteSolido").update()
                    .set("massa", novaMassa)
                    .set("estadoFisico", novoEstado)
                    .where("id = :id").bind("id", reagente->getId()).execute();

                // Atualiza objeto
                sol->setMassa(novaMassa);
                sol->setEstadoFisico(novoEstado);
                std::cout << "Atualizado com sucesso\n";
            }
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao atualizar reagente no banco: " << err.what() << std::endl;
    }
}

void Gestor::excluirReagente()
{
    std::cout << "\n Excluir Reagente \n";
    std::cout << "Digite o nome do reagente a excluir: ";
    std::string nomeBusca;
    std::cin.ignore();
    std::getline(std::cin, nomeBusca);

    Reagente *reagente = laboratorio->buscarReagente(nomeBusca);

    if (reagente == nullptr)
    {
        std::cout << "Reagente não encontrado.\n";
        return;
    }

    std::cout << "ATENÇÃO: Você está prestes a excluir: " << reagente->getNome() << "\n";
    if (!confirmacao())
    {
        std::cout << "Operação cancelada.\n";
        return;
    }

    int idParaRemover = reagente->getId();

    try
    {
        // 1-Remove das tabelas filhas
        db->getTable("ReagenteLiquido").remove().where("id = :id").bind("id", idParaRemover).execute();
        db->getTable("ReagenteSolido").remove().where("id = :id").bind("id", idParaRemover).execute();

        // 2-Remove da tabela principal
        db->getTable("Reagente").remove().where("id = :id").bind("id", idParaRemover).execute();

        // 3-Remove da memoria do laboratorio
        laboratorio->removerReagenteDaMemoria(idParaRemover);

        std::cout << "Reagente excluído com sucesso do banco e da memória.\n";
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao excluir do banco de dados: " << err.what() << std::endl;
    }
}

void Gestor::filtrarReagentes()
{
    std::cout << "\nFiltrar Reagentes\n";
    std::cout << "1. Por Categoria (Solido/Liquido)\n";
    std::cout << "2. Por Restricao (Restritos/Livres)\n";
    
    int op;
    
    // Validacao do menu de filtro
    while (true) 
    {
        std::cout << "Opcao: ";
        std::cin >> op;

        if (std::cin.fail()) 
        {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Entrada invalida Digite um numero\n";
        } 
        else 
        {
            break;
        }
    }

    // Pega lista completa
    std::vector<Reagente *> todos = laboratorio->listarReagentes("");
    bool encontrouAlgum = false;

    std::cout << "\n";
    std::cout << std::left << std::setw(25) << "Nome" << std::setw(15) << "Detalhe" << "\n";
    std::cout << "\n";

    if (op == 1)
    {
        std::cout << "Digite 1 para Líquidos ou 2 para Sólidos: ";
        int tipo;
        std::cin >> tipo;

        for (Reagente *r : todos)
        {
            // Verifica se o reagente atual é liquido ou solido
            ReagenteLiquido *liq = dynamic_cast<ReagenteLiquido *>(r);
            ReagenteSolido *sol = dynamic_cast<ReagenteSolido *>(r);

            if (tipo == 1 && liq != nullptr)
            {
                std::cout << std::left << std::setw(25) << r->getNome() << "Líquido\n";
                encontrouAlgum = true;
            }
            else if (tipo == 2 && sol != nullptr)
            {
                std::cout << std::left << std::setw(25) << r->getNome() << "Sólido\n";
                encontrouAlgum = true;
            }
        }
    }
    else if (op == 2)
    {
        // Filtro por Restricao (Restrito vs Livre)
        std::cout << "Digite 1 para ver Reagentes Restritos\n";
        std::cout << "Digite 2 para ver Reagentes Livres\n";
        std::cout << "Opção: ";
        int escolha;
        std::cin >> escolha;

        for (Reagente *r : todos)
        {
            bool ehRestrito = (r->getNivelAcesso() == 1);

            if (escolha == 1)
            {
                // O usuario quer ver restritos.
                if (ehRestrito)
                {
                    std::cout << std::left << std::setw(25) << r->getNome() << "RESTRITO\n";
                    encontrouAlgum = true;
                }
            }
            else if (escolha == 2)
            {
                // O usuario quer ver os livres
                if (!ehRestrito)
                {
                    std::cout << std::left << std::setw(25) << r->getNome() << "Livre\n";
                    encontrouAlgum = true;
                }
            }
        }
    }

    if (!encontrouAlgum)
    {
        std::cout << "Nenhum reagente encontrado com este filtro.\n";
    }
    std::cout << "\n";
}

// gerenciarLaboratorio
void Gestor::gerenciarLaboratorio()
{
    if (this->laboratorio == nullptr)
    {
        std::cout << "Erro: Gestor não está vinculado a nenhum laboratório.\n";
        return;
    }

    int opcao = 0;
    do
    {
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
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;

        // Bloco de protecao contra letras no menu
        if (std::cin.fail())
        {
            std::cin.clear(); // Limpa o estado de erro
            std::cin.ignore(); // Ignora o caractere invalido
            opcao = -1; // Forca cair no default e repetir o loop
            std::cout << "Entrada invalida Digite apenas numeros\n";
        }

        switch (opcao)
        {
        case 1:
            this->cadastrarReagente();
            break;
        case 2:
            this->listarReagentesDoLaboratorio();
            break;
        case 3:
            this->editarReagente();
            break;
        case 4:
            this->excluirReagente();
            break;
        case 5:
            this->filtrarReagentes();
            break;
        case 6:
            this->retirarReagente();
            break;
        case 7:
            this->historicoRetiradas();
            break;
        case 8:
            this->desassociarEstudantes();
            break;
        case 9:
            this->sairLaboratorio();
            break;
        case 0:
            std::cout << "Retornando..\n";
            break;
        default:
            std::cout << "Opção inválida. Tente novamente.\n";
        }
    } while (opcao != 0);
}

// retirarReagente
void Gestor::retirarReagente()
{
    if (this->laboratorio == nullptr)
    {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== RETIRADA DE REAGENTE ===\n";

    // Listar reagentes disponíveis
    std::vector<Reagente *> reagentes = laboratorio->listarReagentes("");
    if (reagentes.empty())
    {
        std::cout << "Nenhum reagente cadastrado no laboratório.\n";
        return;
    }

    std::cout << "Reagentes disponíveis:\n";
    for (size_t i = 0; i < reagentes.size(); i++)
    {
        std::cout << i + 1 << ". " << reagentes[i]->getNome()
                  << " (ID: " << reagentes[i]->getId()
                  << ") - Quantidade: " << reagentes[i]->getQuantidade()
                  << " " << reagentes[i]->getUnidadeMedida()
                  << " - Local: " << reagentes[i]->getLocalArmazenamento() << std::endl;
    }

    int escolha;
    double quantidade;
    // Loop de validacao da escolha do reagente
    while (true)
    {
        std::cout << "\nEscolha o numero do reagente (0 para cancelar): ";
        std::cin >> escolha;

        // Verifica se digitou algo que nao e numero
        if (std::cin.fail())
        {
            std::cin.clear(); // Limpa erro do cin
            std::cin.ignore(); // Limpa buffer
            std::cout << "Entrada invalida Digite apenas numeros\n";
        }
        else
        {
            break; // Sai do loop se for numero valido
        }
    }

    if (escolha == 0)
    {
        std::cout << "Retirada cancelada.\n";
        return;
    }

    if (escolha < 1 || escolha > static_cast<int>(reagentes.size()))
    {
        std::cout << "Escolha inválida.\n";
        return;
    }

    Reagente *reagenteEscolhido = reagentes[escolha - 1];

    // Verificar se há quantidade suficiente
    if (reagenteEscolhido->getQuantidade() <= 0)
    {
        std::cout << "Este reagente está com estoque zerado.\n";
        return;
    }

    // Loop de validacao da quantidade
    while (true)
    {
        std::cout << "Quantidade a retirar (" << reagenteEscolhido->getUnidadeMedida()
                  << ") - Disponivel: " << reagenteEscolhido->getQuantidade() << ": ";
        std::cin >> quantidade;

        // Verifica erro de entrada nao numerica
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Entrada invalida Digite apenas numeros\n";
        }
        // Verifica se quantidade e positiva
        else if (quantidade <= 0)
        {
            std::cout << "Quantidade deve ser maior que zero\n";
        }
        else
        {
            break; // Entrada valida
        }
    }

    if (quantidade > reagenteEscolhido->getQuantidade())
    {
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

    if (!confirmacao())
    {
        std::cout << "Retirada cancelada.\n";
        return;
    }

    try
    {
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

        time_t agora;
        time(&agora);

        struct tm *_tempoInfo = localtime(&agora);

        char buff[20];
        strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", _tempoInfo);

        std::string hora = std::string(buff);

        // Registrar a retirada (se a tabela existir)
        try
        {
            Table retiradaTable = db->getTable("Retirada");
            retiradaTable.insert("reagente_id", "usuario_id", "quantidadeRetirada", "dataHoraRetirada")
                .values(reagenteEscolhido->getId(), this->getId(), quantidade, hora)
                .execute();
        }
        catch (mysqlx::Error &e)
        {
            std::cout << "Erro ao registrar retirada no banco " << e.what() << std::endl;
        }

        std::cout << "Retirada registrada com sucesso!\n";
        std::cout << "Nova quantidade: " << novaQuantidade << " " << reagenteEscolhido->getUnidadeMedida() << std::endl;
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao registrar retirada: " << err.what() << std::endl;
    }
}

void Gestor::sairLaboratorio()
{
    if (this->laboratorio == nullptr)
    {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== SAIR DO LABORATÓRIO ===\n";
    std::cout << "Laboratório atual: " << laboratorio->getNome() << std::endl;
    std::cout << "Departamento: " << laboratorio->getDepartamento() << std::endl;
    std::cout << "Gestor: " << this->getNome() << std::endl;

    if (!confirmacao())
    {
        std::cout << "Operação cancelada.\n";
        return;
    }

    try
    {
        // Atualizar no banco de dados - remover associação
        Table gestorTable = db->getTable("Gestor");
        gestorTable.update()
            .set("laboratorio_id", mysqlx::nullvalue)
            .where("id = :id")
            .bind("id", this->getId())
            .execute();

        this->laboratorio = nullptr;

        std::cout << "Gestor removido do laboratório com sucesso!\n";
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao sair do laboratório: " << err.what() << std::endl;
    }
}

void Gestor::historicoRetiradas()
{
    if (this->laboratorio == nullptr)
    {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== HISTÓRICO DE RETIRADAS ===\n";

    try
    {
        std::vector<int> idsReagentes = laboratorio->getIdsReagentesDoLaboratorio();

        if (idsReagentes.empty())
        {
            std::cout << "Nenhum reagente encontrado neste laboratório.\n";
            return;
        }
        std::string filtro = "";
        for (int i = 0; i < idsReagentes.size(); i++)
        {
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

        for (Row row : res)
        {
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

            if (r1.count() > 0)
            {
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

            if (r2.count() > 0)
            {
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
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao consultar histórico: " << err.what() << std::endl;
    }
}

// desassociarEstudantes
void Gestor::desassociarEstudantes()
{
    if (this->laboratorio == nullptr)
    {
        std::cout << "Este gestor não está associado a nenhum laboratório.\n";
        return;
    }

    std::cout << "\n=== DESASSOCIAR ESTUDANTE ===\n";

    // Buscar estudantes associados a este laboratório
    std::vector<Estudante *> estudantesAssociados;

    try
    {
        Table associadoTable = db->getTable("Associado");
        RowResult result = associadoTable.select("estudante_id")
                               .where("laboratorio_id = :lab_id")
                               .bind("lab_id", laboratorio->getId())
                               .execute();

        for (Row row : result)
        {
            int estudanteId = row[0].get<int>();

            // Buscar estudante
            for (int i = 0; i < estudantes.size(); i++)
            {
                if (estudantes[i]->getId() == estudanteId)
                {
                    estudantesAssociados.push_back(estudantes[i]);
                    break;
                }
            }
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao buscar estudantes associados: " << err.what() << std::endl;
        return;
    }

    if (estudantesAssociados.empty())
    {
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

    for (size_t i = 0; i < estudantesAssociados.size(); i++)
    {
        std::cout << std::left
                  << std::setw(5) << (i + 1)
                  << std::setw(20) << estudantesAssociados[i]->getNome()
                  << std::setw(25) << estudantesAssociados[i]->getEmail()
                  << std::setw(15) << estudantesAssociados[i]->getCurso()
                  << "\n";
    }
    std::cout << std::string(65, '-') << "\n";

    int escolha;
    std::cout << "\nEscolha o número do estudante para desassociar: ";
    std::cin >> escolha;

    if (escolha < 1 || escolha > static_cast<int>(estudantesAssociados.size()))
    {
        std::cout << "Escolha inválida.\n";
        return;
    }

    Estudante *estudanteEscolhido = estudantesAssociados[escolha - 1];

    // Confirmação
    std::cout << "\n--- CONFIRMAÇÃO DE DESASSOCIAÇÃO ---\n";
    std::cout << "Estudante: " << estudanteEscolhido->getNome() << std::endl;
    std::cout << "Email: " << estudanteEscolhido->getEmail() << std::endl;
    std::cout << "Curso: " << estudanteEscolhido->getCurso() << std::endl;
    std::cout << "Laboratório: " << laboratorio->getNome() << std::endl;

    if (!confirmacao())
    {
        std::cout << "Desassociação cancelada.\n";
        return;
    }

    try
    {
        // Remover da tabela Associado
        Table associadoTable = db->getTable("Associado");
        associadoTable.remove()
            .where("estudante_id = :est_id AND laboratorio_id = :lab_id")
            .bind("est_id", estudanteEscolhido->getId())
            .bind("lab_id", laboratorio->getId())
            .execute();

        std::cout << "Estudante desassociado com sucesso!\n";
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao desassociar estudante: " << err.what() << std::endl;
    }
}

void Gestor::carregarAssociacoes(Schema *db)
{
    try
    {
        Table gestorTable = db->getTable("Gestor");

        // Percorre todos os gestores carregados em memória
        for (Gestor *g : gestores)
        {
            // Consulta o laboratorio_id do gestor no banco
            RowResult resultGestor = gestorTable
                                         .select("laboratorio_id")
                                         .where("id = :id")
                                         .bind("id", g->getId())
                                         .execute();

            if (resultGestor.count() == 0)
                continue;

            Row rowGestor = resultGestor.fetchOne();

            // Verifica se o campo laboratorio_id não é nulo
            if (!rowGestor[0].isNull())
            {
                int laboratorioId = rowGestor[0].get<int>();

                // Busca o laboratório correspondente na lista de laboratórios carregados
                for (Laboratorio *lab : Laboratorio::laboratorios)
                {
                    if (lab->getId() == laboratorioId)
                    {
                        g->setLaboratorio(lab);
                        break;
                    }
                }
            }
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao carregar associações gestor-laboratório: " << err.what() << std::endl;
    }

    try
    {
        Table tableAssociado = db->getTable("Associado");
        RowResult result = tableAssociado.select("estudante_id", "laboratorio_id", "papel").execute();

        for (Row row : result)
        {
            int estudanteId = row[0].get<int>();
            int laboratorioId = row[1].get<int>();
            std::string papel = row[2].get<std::string>();

            // Encontra o estudante correspondente
            Estudante *est = nullptr;
            for (int j = 0; j < (int)estudantes.size(); j++)
            {
                if (estudantes[j]->getId() == estudanteId)
                {
                    est = estudantes[j];
                    break;
                }
            }
            if (!est)
            { // Se não encontrou em estudantes, procura em posGraduandos
                for (int j = 0; j < (int)posGraduandos.size(); j++)
                {
                    if (posGraduandos[j]->getId() == estudanteId)
                    {
                        est = posGraduandos[j];
                        break;
                    }
                }
            }
            if (!est)
                continue; // Ignora se não encontrado em nenhuma das listas

            // Encontra o laboratório correspondente
            Laboratorio *lab = nullptr;
            for (int j = 0; j < (int)Laboratorio::laboratorios.size(); j++)
            {
                if (Laboratorio::laboratorios[j]->getId() == laboratorioId)
                {
                    lab = Laboratorio::laboratorios[j];
                    break;
                }
            }
            if (!lab)
                continue; // Ignora se não encontrado

            // A função estaAssociado contém a lógica para corrigir inconsistências
            // entre o banco de dados e a memória. Chamá-la aqui garante que
            // a lista de estudantes do laboratório seja populada corretamente durante o carregamento.
            lab->estaAssociado(est);
            // A função adicionarLaboratorio também tem checagem de duplicatas,
            // então é seguro chamar para garantir que o estudante tenha o laboratório em sua lista.
            est->adicionarLaboratorio(lab, papel);
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro ao carregar associações: " << err.what() << std::endl;
    }
}

void Gestor::limparUsuarios()
{
    for (Usuario *u : usuariosCarregados)
        delete u;
    for (Gestor *g : gestores)
        delete g;
    for (Estudante *e : estudantes)
        delete e;
    for (PosGraduacao *p : posGraduandos)
        delete p;

    usuariosCarregados.clear();
    gestores.clear();
    estudantes.clear();
    posGraduandos.clear();
}

Estudante* Gestor::getEstudanteById(int id)
{
    for (Estudante* e : estudantes)
        if (e->getId() == id)
            return e;
    for (PosGraduacao* p : posGraduandos)
        if (p->getId() == id)
            return p;
    return nullptr;
}
