#include "DatabaseConnection/databaseConnection.h"
#include "Gestor/gestor.h"
#include "Usuario/usuario.h"
#include "Estudante/estudante.h"
#include "PosGraduacao/posgraduacao.h"
#include "Laboratorio/Laboratorio.h"
#include <iostream>
using namespace mysqlx;

// Função auxiliar para confirmação
bool confirmacaoMenu()
{
    std::cout << "Tem certeza em realizar esta ação? Digite S para Sim e N para Não: ";
    char resposta;
    std::cin >> resposta;
    return (resposta == 'S' || resposta == 's');
}

void acessarMenu(Usuario *usuario) // Função para acessar o menu do usuário
{
    if (!usuario)
    {
        std::cout << "Erro: usuario nulo!\n";
        return;
    }
    usuario->menuPrincipal(); // POLIMORFISMO!!!// Chama o menu principal do usuário logado
}

int main()
{
    Schema *db = nullptr;
    DatabaseConnection conexaoDB;
    Session *session = nullptr;
    try
    {
        // ===================== Configuração do Banco de Dados =====================
        // Estabelece a conexão com o banco de dados
        session = conexaoDB.getSession(); // Obtém a sessão de conexão
        db = conexaoDB.getSchema();       // Obtém o esquema do banco de dados
        // Verifica se a conexão e o esquema foram inicializados corretamente
        if (!session || !db)
        {
            throw std::runtime_error("Falha ao inicializar a conexão com o banco de dados.");
            return 1;
        }

        // Carrega laboratórios e usuários do banco de dados
        Laboratorio::listarLaboratorios(db); // Carrega os laboratorios
        Gestor::carregarUsuarios(db);        // Carrega os dados dos usuarios
        Gestor::carregarAssociacoes(db);     // Carrega as associações entre usuários e laboratórios

        // ===================== Inicializando o Gestor Admin =====================
        std::cout << "Instanciando Gestor Admin..." << std::endl;
        Gestor gestorAdmin("Admin", "admin@example.com", "senha123", 1, db);   // Gestor administrador inicial
        Table usuariosTabela = db->getTable("Usuario");                        // Consulta a tabela de usuários
        RowResult resultado = usuariosTabela                                   // Busca por um usuário com o email do gestorAdmin por consulta (SELECT)
                                  .select("id")                                // Seleciona o campo id
                                  .where("email = :email AND nivelAcesso = 1") // Filtra para consultar por meio do email e nivel de acesso (gestores)
                                  .bind("email", gestorAdmin.getEmail())       // Substitui o parâmetro :email ao email do gestorAdmin
                                  .execute();                                  // Executa a consulta
        if (resultado.count() == 0)
        {                                                                                                // Se nenhum gestor foi encontrado
            std::cout << "Nenhum gestor encontrado no banco. Cadastrando gestor padrão..." << std::endl; // Informa que vai cadastrar o gestor padrão
            try
            {
                // cadastra o gestorAdmin no banco de dados
                Table usuarioTable = db->getTable("Usuario");                             // Insere na tabela Usuario (base)
                Result res = usuarioTable.insert("nome", "email", "senha", "nivelAcesso") // Insere os dados básicos
                                 .values(gestorAdmin.getNome(), gestorAdmin.getEmail(), "senha123", 1)
                                 .execute();                  // Define os valores e executa a inserção
                int usuarioId = res.getAutoIncrementValue();  // Recupera o ID do usuário recém-inserido
                db->getTable("Gestor")                        // Insere na tabela Gestor
                    .insert("id", "cadastrado_por_gestor_id") // Id do gestor e quem cadastrou
                    .values(usuarioId, usuarioId)             // quem cadastrou é ele mesmo
                    .execute();
                std::cout << "Gestor padrão cadastrado com sucesso! Email: " << gestorAdmin.getEmail() << " Senha: senha123\n"; // Mensagem de sucesso
            }
            catch (const Error &err)
            {
                std::cerr << "Erro ao cadastrar gestor: " << err.what() << std::endl;
            }
        }
        else
        {
            Row row = resultado.fetchOne(); // Obtém a primeira linha do resultado
            gestorAdmin.setId(row[0]);      // atualiza o ID do objeto gestorAdmin com o ID do banco de dados
            std::cout << "Gestor já existe no banco. ID: "
                      << gestorAdmin.getId() << std::endl; // Informa que o gestor já existe
            std::cout << "Credenciais do gestor padrão - Email: " << gestorAdmin.getEmail() << " Senha: " << gestorAdmin.getSenha() << std::endl;
        }
    }
    catch (const mysqlx::Error &err)
    {
        std::cerr << "Erro MySQL: " << err.what() << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Erro genérico: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Erro desconhecido ao executar o programa." << std::endl;
    }
    // ===================== Sistema =====================
    // ===================== Login =====================
    std::cout << "======Sejam bem-vindo ao LabUFV!======\n"
              << std::endl;
    std::cout << "Para realizar qualquer atividade, é necessário fazer Login\n"
              << std::endl;
    std::string email, senha;               // Variáveis para armazenar as credenciais de login
    std::unique_ptr<Usuario> usuarioLogado; // Ponteiro inteligente para o usuário logado
    Usuario usuarioTemp("", "", "", 0, db); // Objeto temporário para validação e login

    // Executa até o login seja realizado com sucesso
    while (true)
    {
        try
        {
            std::cout << "E-mail: " << std::endl; // Solicita o email
            std::cin >> email;                    // Pede o e-mail
            Usuario::validarEmail(email);         // Valida o formato do email

            std::cout << "Senha: " << std::endl; // Solicita a senha
            std::cin >> senha;                   // Pede a senha
            Usuario::validarSenha(senha);        // Valida o formato da senha

            if (Usuario::fazerLogin(db, email, senha, &usuarioTemp))
            { // Tenta fazer o login
                // Se o login for bem-sucedido, cria o objeto apropriado com base no nível de acesso

                int id = usuarioTemp.getId();                        // Obtém o ID do usuário logado
                Usuario *encontrado = Gestor::buscarUsuarioById(id); // Busca o usuário pelo ID
                if (!encontrado)
                {
                    std::cerr << "Erro: Usuário não encontrado na lista de usuários carregados.\n";
                    continue; // Volta para o início do loop de login
                }
                int nivelAcessoColetado = encontrado->getNivelAcesso();
                if (nivelAcessoColetado == 1)
                { // Gestor
                    Gestor *g = dynamic_cast<Gestor *>(encontrado); // Tenta converter para Gestor

                    if (!g) // Verifica se a conversão foi bem-sucedida 
                    {
                        std::cerr << "Erro interno: Tipo Gestor esperado.\n";
                        continue;
                    }

                    usuarioLogado = std::make_unique<Gestor>(*g); // Cria uma cópia do Gestor encontrado ao ponteiro inteligente

                    std::cout << "Login bem-sucedido! Bem-vindo, Gestor "
                              << usuarioLogado->getNome() << ".\n\n";
                    break;
                }
                else if (nivelAcessoColetado == 2 || nivelAcessoColetado == 3)
                { // Pos-Graduando ou Aluno de Graduacao
                    // Instancia um objeto Estudante ou PosGraduacao no ponteiro inteligente
                    if (nivelAcessoColetado == 2)
                    {
                        // POS-GRADUAÇÃO
                        PosGraduacao *p = dynamic_cast<PosGraduacao *>(encontrado); // Tenta converter para PosGraduacao 

                        if (!p) // Verifica se a conversão foi bem-sucedida 
                        {
                            std::cerr << "Erro interno: Tipo Pós-Graduação esperado.\n";
                            continue;
                        }

                        usuarioLogado = std::make_unique<PosGraduacao>(*p); // Cria uma cópia do PosGraduacao encontrado ao ponteiro inteligente

                        std::cout << "Login bem-sucedido! Bem-vindo, Pós-Graduando "
                                  << usuarioLogado->getNome() << ".\n\n";
                        break;
                    }
                    else
                    {
                        // ESTUDANTE GRADUAÇÃO
                        Estudante *e = dynamic_cast<Estudante *>(encontrado); // Tenta converter para Estudante

                        if (!e) // Verifica se a conversão foi bem-sucedida
                        {
                            std::cerr << "Erro interno: Tipo Estudante esperado.\n";
                            continue;
                        }

                        usuarioLogado = std::make_unique<Estudante>(*e); // Cria uma cópia do Estudante encontrado ao ponteiro inteligente

                        std::cout << "Login bem-sucedido! Bem-vindo, Estudante "
                                  << usuarioLogado->getNome() << ".\n\n";
                        break;
                    }
                }
                else
                {
                    std::cerr << "Falha no login. Usuário não encontrado. Verifique suas credenciais.\n"
                              << std::endl;
                    usuarioLogado.reset(); // limpa o ponteiro
                    continue;              // volta para tentar novamente
                }
            }
        }
        catch (const std::invalid_argument &ex)
        {
            std::cerr << "Erro de validação: " << ex.what() << "\nTente novamente.\n\n";
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Erro ao ler entrada: " << ex.what() << std::endl;
            continue; // Volta ao início do loop em caso de erro
        }
    }
    if (usuarioLogado)
    {
        acessarMenu(usuarioLogado.get()); // Chama o menu principal do usuário logado -
        // (DETALHE: POLIMORFISMO ACONTECE AQUI, COMO USADO COMO EXEMPLO EM UM DOS EXEMPLOS DA AULA DE PROG2)
    }

    // ===================== Encerramento =====================
    // Limpa todos os laboratórios alocados dinamicamente
    Laboratorio::limparLaboratorios();

    // Limpa todos os gestores alocados dinamicamente
    Gestor::limparUsuarios();

    return 0;
}