#include "../Retirada/Retirada.h"
#include <ctime>
#include <sstream>
#include <iomanip>

// Construtor
Retirada::Retirada(int id, Usuario *usuario, Reagente *reagente, float quantidade)
    : id(id), usuario(usuario), reagente(reagente), quantidade(quantidade), confirmada(false)
{

    // Gera data/hora atual automaticamente
    std::time_t now = std::time(nullptr);      // Pega tempo atual em segundos
    std::tm *localTime = std::localtime(&now); // Converte para struct
    std::stringstream ss;
    ss << std::put_time(localTime, "%d/%m/%Y %H:%M"); // Formata como "25/12/2024 14:30"
    dataHora = ss.str();                              // Salva como string
}

Retirada::Retirada(int id, Usuario *usuario, Reagente *reagente, float quantidade, std::string dataHora)
    : id(id), usuario(usuario), reagente(reagente), quantidade(quantidade), dataHora(dataHora), confirmada(false)
{

    // Gera data/hora atual automaticamente
    std::time_t now = std::time(nullptr);      // Pega tempo atual em segundos
    std::tm *localTime = std::localtime(&now); // Converte para struct
    std::stringstream ss;
    ss << std::put_time(localTime, "%d/%m/%Y %H:%M"); // Formata como "25/12/2024 14:30"
    dataHora = ss.str();                              // Salva como string
}

// Tenta confirmar a retirada
std::string Retirada::confirmarRetirada()
{
    // Validações básicas
    if (quantidade <= 0)
    {
        return "Quantidade tem ser maior que zero";
    }

    // Verifica se tem estoque suficiente
    if (quantidade > reagente->getQuantidade())
    {
        return "Quantidade indisponivel. Disponivel: " + std::to_string(reagente->getQuantidade());
    }

    // Atualiza estoque
    reagente->setQuantidade(reagente->getQuantidade() - quantidade);
    confirmada = true; // Marca como confirmada

    // Verifica se ficou com estoque crítico após a retirada
    if (reagente->getQuantidade() <= reagente->getQuantidadeCritica())
    {
        return "Retirada realizada! ALERTA: quantidade critica em estoque (" +
               std::to_string(reagente->getQuantidade()) + ")";
    }

    return "Retirada realizada"; // Tudo certo >-<
}

// Cancela retirada
std::string Retirada::cancelarRetirada()
{
    if (confirmada)
    {
        // Devolve a quantidade retirada ao estoque
        reagente->setQuantidade(reagente->getQuantidade() + quantidade);
        confirmada = false; // Marca como não confirmada
        return "Retirada cancelada, estoque restaurado";
    }
    return "Retirada cancelada"; // Já estava cancelada
}

// Gera string com informações da retirada
std::string Retirada::getInfo()
{
    std::stringstream info;
    info << "ID: " << id << "\n"
         << "Usuario: " << usuario->getNome() << "\n"
         << "Reagente: " << reagente->getNome() << "\n"
         << "Quantidade: " << quantidade << " " << reagente->getUnidadeMedida() << "\n"
         << "Data/Hora: " << dataHora << "\n"
         << "Status: " << (confirmada ? "Confirmada" : "Pendente");
    return info.str(); // Converte stringstream para string
}
