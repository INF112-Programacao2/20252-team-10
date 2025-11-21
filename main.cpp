#include "DatabaseConnection/databaseConnection.h"
#include "Gestor/gestor.h"
#include "Usuario/usuario.h"
#include "Estudante/estudante.h"
#include "PosGraduacao/posgraduacao.h"
#include "Laboratorio/Laboratorio.h"
#include <iostream>
using namespace mysqlx;

// Função auxiliar para confirmação
bool confirmacao1() {
    std::cout << "Tem certeza em realizar esta ação? Digite S para Sim e N para Não: ";
    char resposta;
    std::cin >> resposta;
    return (resposta == 'S' || resposta == 's');
}

void menuGestor(Gestor* gestor) {
    int opcao = 0;
    do {
            std::cout << "\n===== Menu Gestor =====\n";
            // Imprime os dados dos gestor em uso
            std::cout << "Seja bem-vindo " << gestor->getNome() << " novamente!\n";
            std::cout << "Dados do usuário: ("
                        // Imprime o ID do usuario
                        << gestor->getId()  << ") "
                        // Imprime o nome do usuario
                        << gestor->getNome() << " - "
                        // Imprime o email do usuario
                        << gestor->getEmail()  << " - ";
                        // Imprime o nome do laboratorio
                        if(gestor->getLaboratorio()){
                        std::cout << gestor->getLaboratorio()->getNome() << " - \n";
                        }

                        // Imprime o nivel de acesso do usuario, em forma de texto, que representa o que é no sistema
                        std::cout << ((gestor->getNivelAcesso() == 1) ? "Gestor\n"
                            : (gestor->getNivelAcesso() == 2) ? "Pós-graduação\n"
                            : (gestor->getNivelAcesso() == 3) ? "Graduação\n"
                            : "Desconhecido\n");
            // Atributo que recebe true se estiver associado a um laboratorio e false, caso contrário
            bool associado = gestor->estaAssociado();

            // Se for associado, imprime um menu especifico para este acso
            if(associado){
                std::cout << "===== Menu Gestor =====\n";
                std::cout << "1. Gerenciar laboratório\n";                     // Acessa o submenu de gestão do laboratório: dados, estatísticas, reagentes (cadastrar, editar, excluir)
                std::cout << "2. Listar estudantes do laboratório\n";           // Mostra todos os estudantes atualmente associados ao laboratório
                std::cout << "3. Acessar reagentes em alerta\n";  // Lista reagentes em alerta

                std::cout << "4. Acessar reagentes restritos\n";               // Acessa reagentes restritos que exigem autorização especial
                // std::cout << "5. Retirar reagente\n";                           // Retirar reagentes do laboratório (pede confirmação e registra retirada)
                // std::cout << "8. Deletar estudante\n";                          // Remove estudante do sistema completamente (pede confirmação)
                // std::cout << "9. Histórico de retiradas (últimos 7 dias)\n";    // Mostra o histórico de retiradas recentes do laboratório
                // std::cout << "10. Cadastrar estudante\n";                        // Adiciona novo estudante e associa automaticamente ao laboratório
                // std::cout << "11. Cadastrar gestor\n";                           // Adiciona novo gestor ao sistema (sem laboratório associado)
                // std::cout << "12. Listar usuários do sistema\n";                 // Lista todos os usuários cadastrados (gestores e estudantes)
                // std::cout << "13. Sair do laboratório\n";                        // Sai da gestão do laboratório (não desassocia automaticamente)
            }else{ // Se nao estiver associado, imprime um menu para quem não é associaod a nada
                // Menu quando o gestor não está associado a nenhum laboratório
                std::cout << "1. Associar laboratório\n";   // Associa o gestor a um laboratório disponível (pede confirmação)
                std::cout << "2. Cadastrar usuario\n";      // Cria um novo usuario no sistema (sem associação inicial)
                std::cout << "3. Listar usuários do sistema\n"; // Mostra todos os usuários cadastrados
                std::cout << "4. Deletar gestor\n";        // Exclui um gestor do sistema (pede confirmação)
                std::cout << "5. Deletar estudante\n";     // Exclui um estudante do sistema (pede confirmação)


            }
            std::cout << "0. Sair do sistema\n";  // Encerra o menu principal
            std::cout << "Escolha uma opção: ";
            std::cin >> opcao;
            switch(opcao) {
            case 1:
                if(associado) gestor->gerenciarLaboratorio();
                else gestor->associarLaboratorio();
                break;
            case 2:
                if(associado) gestor->getLaboratorio()->listarEstudantes();
                else gestor->cadastrarUsuario();
                break;
            case 3:
                if(associado) gestor->acessarReagentesAlerta();
                 else
                gestor->listarUsuarios();
                break;
            case 4:
                if(associado) gestor->menuReagentesRestritos();
                break;
            // case 5:
            //     if(associado) gestor->menuReagentesRestritos();
            //     else gestor->deletarGestor();
            //     break;
            // case 6: if(associado) gestor->menuReagentesRestritos(); break;
            // case 7: if(associado) gestor->retirarReagente(); break;
            // case 8: if(associado) gestor->deletarEstudante(); break;
            // case 9: if(associado) gestor->historicoRetiradas(); break;
            // case 10: if(associado) gestor->cadastrarEstudante(); break;
            // case 11: if(associado) gestor->cadastrarGestor(); break;
            // case 12: if(associado) gestor->listarUsuarios(); break;
            // case 13: if(associado) gestor->sairLaboratorio(); break;
            case 0:
                std::cout << "Saindo...\n";
                break;
            default:
                std::cout << "Opção inválida! Tente novamente.\n";
        }

    } while(opcao != 0);
};
void menuEstudante(Estudante* estudante) {
    int opcao = 0;

    do {
        std::cout << "\n===== Menu Estudante =====\n";
        std::cout << "1. Meus laboratórios\n";            // Lista laboratórios aos quais o estudante está associado
        std::cout << "2. Retirar reagente\n";            // Retirar reagentes (pede confirmação e registra retirada)
        std::cout << "0. Sair\n";                        // Sai do menu

        std::cout << "Escolha uma opção: ";
        std::cin >> opcao;

        switch (opcao) {
            case 1:
                //estudante->acessarLaboratorios();    // Dentro dessa função, o usuário pode acessar laboratório, listar usuários, reagentes, filtrar, etc.
                std::cout << "Lab acessado" << std::endl;
                break;
            case 2:
                if (confirmacao1()) {
                    std::cout << "Reagente retirado" << std::endl;
                    //estudante->retirarReagente();   // Função interna cuida da retirada e registro
                }
                break;
            case 0:
                std::cout << "Saindo do menu do estudante...\n";
                break;
            default:
                std::cout << "Opção inválida! Tente novamente.\n";
        }

    } while (opcao != 0);
}
int main() {
    Schema* db = nullptr;
    DatabaseConnection conexaoDB;
    Session* session = nullptr;
    try{
        // ===================== Configuração do Banco de Dados =====================
        //Estabelece a conexão com o banco de dados
        session = conexaoDB.getSession(); // Obtém a sessão de conexão
        db = conexaoDB.getSchema(); // Obtém o esquema do banco de dados
        // Verifica se a conexão e o esquema foram inicializados corretamente
        if (!session || !db) {
                throw std::runtime_error("Falha ao inicializar a conexão com o banco de dados.");
                return 1;
        }

        // ===================== Inicializando o Gestor Admin =====================
        std::cout << "Instanciando Gestor Admin..." << std::endl;
        Gestor gestorAdmin("Admin", "admin@example.com", "senha123", 1, db); // Gestor administrador inicial
        Table usuariosTabela = db->getTable("Usuario"); // Consulta a tabela de usuários
        RowResult resultado = usuariosTabela // Busca por um usuário com o email do gestorAdmin por consulta (SELECT)
                .select("id") // Seleciona o campo id
                .where("email = :email AND nivelAcesso = 1") // Filtra para consultar por meio do email e nivel de acesso (gestores)
                .bind("email", gestorAdmin.getEmail())// Substitui o parâmetro :email ao email do gestorAdmin
                .execute(); // Executa a consulta
        if (resultado.count() == 0) { // Se nenhum gestor foi encontrado
            std::cout << "Nenhum gestor encontrado no banco. Cadastrando gestor padrão..." << std::endl; // Informa que vai cadastrar o gestor padrão
            try{
                //cadastra o gestorAdmin no banco de dados
                Table usuarioTable = db->getTable("Usuario"); // Insere na tabela Usuario (base)
                Result res = usuarioTable.insert("nome", "email", "senha", "nivelAcesso") //Insere os dados básicos
                    .values(gestorAdmin.getNome(), gestorAdmin.getEmail(), "senha123", 1).execute(); // Define os valores e executa a inserção
                int usuarioId = res.getAutoIncrementValue(); // Recupera o ID do usuário recém-inserido
                db->getTable("Gestor") // Insere na tabela Gestor
                    .insert("id", "cadastrado_por_gestor_id") // Id do gestor e quem cadastrou
                    .values(usuarioId, usuarioId) // quem cadastrou é ele mesmo
                    .execute();
                std::cout << "Gestor padrão cadastrado com sucesso! Email: " << gestorAdmin.getEmail() << " Senha: senha123\n"; // Mensagem de sucesso
            } catch (const Error &err) {
                std::cerr << "Erro ao cadastrar gestor: " << err.what() << std::endl;
            }
        } else {
            Row row = resultado.fetchOne(); // Obtém a primeira linha do resultado
            gestorAdmin.setId(row[0]);  // atualiza o ID do objeto gestorAdmin com o ID do banco de dados
            std::cout << "Gestor já existe no banco. ID: "
                        << gestorAdmin.getId() << std::endl; // Informa que o gestor já existe
        }
    } catch (const mysqlx::Error &err) {
        std::cerr << "Erro MySQL: " << err.what() << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "Erro genérico: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Erro desconhecido ao executar o programa." << std::endl;
    }
    // ===================== Sistema =====================
    // ===================== Login =====================
    Laboratorio::listarLaboratorios(db); // Carrega os laboratorios
    std::cout << "======Sejam bem-vindo ao LabUFV!======\n" << std::endl;
    std::cout << "Para realizar qualquer atividade, é necessário fazer Login\n" << std::endl;
    std::string email, senha; // Variáveis para armazenar as credenciais de login
    std::unique_ptr<Usuario> usuarioLogado; // Ponteiro inteligente para o usuário logado
    Usuario usuarioTemp; // Objeto temporário para validação e login

    // Executa até o login seja realizado com sucesso
    while (true) {
        try {
            std::cout << "E-mail: " << std::endl; // Solicita o email
            std::cin >> email; // Pede o e-mail
            Usuario::validarEmail(email); // Valida o formato do email

            std::cout << "Senha: " << std::endl;  // Solicita a senha
            std::cin >> senha; // Pede a senha
            Usuario::validarSenha(senha); // Valida o formato da senha

        if(Usuario::fazerLogin(db, email, senha, &usuarioTemp)){ // Tenta fazer o login
            // Se o login for bem-sucedido, cria o objeto apropriado com base no nível de acesso
            int nivelAcessoColetado = usuarioTemp.getNivelAcesso();
            if(nivelAcessoColetado == 1){ // Gestor
                //Instancia um objeto Gestor no ponteiro inteligente
                usuarioLogado = std::make_unique<Gestor>(usuarioTemp.getNome(), usuarioTemp.getEmail(),
                                                        usuarioTemp.getSenha(), usuarioTemp.getNivelAcesso(), db);
                usuarioLogado->setId(usuarioTemp.getId());
                std::cout << "Login bem-sucedido! Bem-vindo, Gestor " << usuarioLogado->getNome() << ".\n" << std::endl;
                break;
            } else if(nivelAcessoColetado == 2 || nivelAcessoColetado == 3){ // Pos-Graduando ou Aluno de Graduacao
                // Carrega dados específicos de estudante do banco antes de instanciar a classe derivada
                std::string matricula, curso, nivel;
                try {
                    // Acessa a tabela de Estudante
                    Table estudanteTable = db->getTable("Estudante");
                    // Faz uma consulta com os dados que necessitam
                    RowResult resultadoEstudante = estudanteTable
                        .select("matricula", "curso", "nivel")
                        .where("id = :id") // Com id do usuário que fez login
                        .bind("id", usuarioTemp.getId()) // Seta o valor de id
                        .execute(); // Executa a consulta

                    // Se o numero de linhas for maior que 0, armazena as informações de acordo com os tipos do C++
                    if(resultadoEstudante.count() > 0){
                        Row estudanteRow = resultadoEstudante.fetchOne(); // Pega o resultado da primeira linha
                        matricula = estudanteRow[0].get<std::string>(); // Copia a matricula e coloca na variavel
                        curso = estudanteRow[1].get<std::string>(); // Copia a curso e coloca na variavel
                        nivel = estudanteRow[2].get<std::string>(); // Copia a nivel e coloca na variavel
                    }
                } catch (const mysqlx::Error &err) {
                    std::cerr << "Erro ao carregar dados do estudante: " << err.what() << std::endl;
                }

                //Instancia um objeto Estudante ou PosGraduacao no ponteiro inteligente
                if(nivelAcessoColetado == 2){
                    usuarioLogado = std::make_unique<PosGraduacao>(usuarioTemp.getNome(), usuarioTemp.getEmail(),
                                                        usuarioTemp.getSenha(), usuarioTemp.getNivelAcesso(), db,
                                                        matricula, curso, nivel);
                    usuarioLogado->setId(usuarioTemp.getId());
                    std::cout << "Login bem-sucedido! Bem-vindo, Pós-Graduando " << usuarioLogado->getNome() << ".\n" << std::endl;
                    break;
                } else {
                    // Intancia estudante (graduação)
                    usuarioLogado = std::make_unique<Estudante>(usuarioTemp.getNome(), usuarioTemp.getEmail(),
                                                                usuarioTemp.getSenha(), usuarioTemp.getNivelAcesso(), db,
                                                                matricula, curso, nivel);
                    usuarioLogado->setId(usuarioTemp.getId());
                    std::cout << "Login bem-sucedido! Bem-vindo, Aluno de Graduação " << usuarioLogado->getNome() << ".\n" << std::endl;
                    break;
                }
            } else {
                std::cerr << "Falha no login. Usuário não encontrado. Verifique suas credenciais.\n" << std::endl;
                usuarioLogado.reset(); // limpa o ponteiro
                continue; // volta para tentar novamente
            }
        }
    }catch (const std::invalid_argument &ex) {
            std::cerr << "Erro de validação: " << ex.what() << "\nTente novamente.\n\n";
            } catch (const std::exception &ex) {
            std::cerr << "Erro ao ler entrada: " << ex.what() << std::endl;
                continue; // Volta ao início do loop em caso de erro
            }
    }
        if(usuarioLogado) {
            if(usuarioLogado->getNivelAcesso() == 1){
            //Converte o ponteiro inteligente do tipo Usuario para do tipo Gestor
            // Isso é para acessar os metodos da classe Gestor
            Gestor *gestor = dynamic_cast<Gestor *>(usuarioLogado.get()); //Retorna o ponteiro do tipo Uusario e converte como gestor
            menuGestor(gestor); // Acessa o menu de gestor
            }
            else { // Graduação ou Pós-graduação
                Estudante* estudante = dynamic_cast<Estudante*>(usuarioLogado.get());
                if (!estudante) {
                    // Se for pós-graduação, converte para PosGraduacao
                    PosGraduacao* pos = dynamic_cast<PosGraduacao*>(usuarioLogado.get());
                    if (pos) {
                        menuEstudante(pos);
                    }
                } else {
                    menuEstudante(estudante);
                }
            }
        }
    Laboratorio l(1, "LabUFV", "DBB", db);
    l.listarReagentes();

    // Limpa todos os laboratórios alocados dinamicamente
    Laboratorio::limparLaboratorios();

    return 0;
}
