#ifndef RETIRADA_H
#define RETIRADA_H

#include <string>
#include "../Usuario/usuario.h"
#include "../Reagente/reagente.h"

class Usuario;

// CLASSE RETIRADA
// Representa uma retirada de reagente
// Contém informações do usuário, reagente, quantidade e confirmação

class Retirada
{
private:
    int id;               // Identificador único da retirada
    Usuario *usuario;     // Usuário que realizou a retirada
    Reagente *reagente;   // Reagente retirado
    float quantidade;     // Quantidade retirada
    std::string dataHora; // Data e hora da retirada (gerada automaticamente)
    bool confirmada;      // Status de confirmação da retirada

public:
    // Construtor
    Retirada(int id, Usuario *usuario, Reagente *reagente, float quantidade);
    Retirada(int id, Usuario *usuario, Reagente *reagente, float quantidade, std::string dataHora);

    // Métodos principais
    std::string confirmarRetirada(); // Valida e executa a retirada, atualizando estoque
    std::string cancelarRetirada();  // Cancela retirada e restaura estoque
    std::string getInfo();           // Retorna string formatada com informações da retirada

    // Getters
    bool estaConfirmada() const { return confirmada; }
    int getId() const { return id; }
    Usuario *getUsuario() const { return usuario; }
    Reagente *getReagente() const { return reagente; }
    float getQuantidade() const { return quantidade; }
    std::string getDataHora() const { return dataHora; }
};

#endif
