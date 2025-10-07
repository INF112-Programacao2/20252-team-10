# 🧪 Sistema de Gestão de Estoque de Reagentes de Laboratório

## 📘 Sobre o Projeto
O **Sistema de Gestão de Estoque de Reagentes de Laboratório** tem como objetivo **facilitar o controle e a organização dos reagentes químicos utilizados em laboratórios**, garantindo **segurança, rastreabilidade e eficiência** nas atividades realizadas.

A aplicação permite o **registro detalhado de entradas e saídas**, o **monitoramento de prazos de validade** e a **localização dos reagentes**. Além disso, o sistema **emite alertas automáticos** quando um reagente está próximo do vencimento ou com o estoque em nível crítico, auxiliando na reposição e evitando o uso de produtos vencidos.

O sistema possui **diferentes perfis de acesso**, como **gestores e membros**, cada um com permissões específicas para assegurar o controle e a integridade dos dados.

---

## 👥 Integrantes do Projeto
- Gustavo Santos Pinto  
- Marcus Vinicius de Souza Santos Prado  
- Nathalia de Assis Almeida  
- Rafael Rangel Fietto  
- Tais dos Santos Ferreira  

---

## 🧭 Funcionalidades Principais
- **Cadastro e controle de reagentes** com informações detalhadas (nome, validade, quantidade, unidade de medida e local de armazenamento).  
- **Registro de retiradas** e **atualização automática do estoque**.  
- **Alertas automáticos** para reagentes vencidos ou em quantidade crítica.  
- **Gestão de usuários**, com opções para cadastrar e remover membros.  
- **Controle de acesso** com permissões distintas para gestores e membros.  
- **Histórico de movimentações** e registro de quem realizou cada ação.  
- **Busca e filtros** por nome, categoria ou local de armazenamento.  

---

## 👤 Perfis de Usuário

### 🧪 Membro do Laboratório
- Pode consultar reagentes disponíveis.  
- Registrar retiradas.  
- Verificar quantidades e validade dos reagentes.  
- Visualizar o local de armazenamento (geladeira, freezer, armário, etc.).  

### ⚙️ Gestor do Laboratório
- Cadastra e remove membros.  
- Adiciona, repõe e exclui reagentes.  
- Recebe alertas automáticos de validade vencida e quantidade crítica.  
- Gerencia o estoque e as informações do laboratório.  
- Acompanha o histórico de retiradas e movimentações.  

---

## 🧩 Estrutura de Classes (Resumo CRC)

| Classe | Principais Responsabilidades | Colaborações |
|--------|------------------------------|---------------|
| **Usuário** | Fazer login, editar perfil, retirar e devolver reagentes, consultar e filtrar reagentes disponíveis. | Retirada, Reagente, Laboratório |
| **Gestor** | Criar e deletar usuários, adicionar e excluir reagentes, repor estoque, gerenciar laboratório, receber alertas automáticos. | Reagente, Laboratório, Alerta, Usuário |
| **Reagente** | Controlar validade e quantidade, calcular níveis críticos, exibir condições e local de armazenamento. | Laboratório, Alerta, Usuário, Retirada |
| **Laboratório** | Listar reagentes, controlar entradas e saídas, registrar histórico de retiradas e reagentes alocados. | Reagente, Retirada, Gestor, Usuário |
| **Retirada** | Registrar data, hora, usuário e quantidade retirada. | Usuário, Reagente, Laboratório, Gestor |
| **Alerta** | Monitorar validade e quantidade crítica, enviar notificações e armazenar histórico. | Reagente, Gestor |

---

## ⚙️ Tecnologias (Em análise de possibilidade)
- **Backend:** C++  
- **Banco de Dados:** (Avaliando a melhor alternativa, considerando a linguagem) 
- **Frontend:** React   
- **Controle de Versão:** Git e GitHub  

---

## 🎯 Objetivo Geral
Desenvolver um sistema eficiente e intuitivo para o **gerenciamento de reagentes químicos em laboratórios**, promovendo **organização, segurança e rastreabilidade** em todas as etapas de uso e armazenamento.

---

---

## 🌍 Contribuição para os ODS (Objetivos de Desenvolvimento Sustentável)

O projeto está alinhado aos **Objetivos de Desenvolvimento Sustentável (ODS)** da **Agenda 2030 da ONU**, especialmente aos seguintes:

- **ODS 3 – Saúde e Bem-Estar:**  
  Promove práticas seguras no uso de reagentes químicos, evitando contaminações, acidentes laboratoriais e o uso de substâncias vencidas, contribuindo para um ambiente mais saudável e seguro.

- **ODS 4 – Educação de Qualidade:**  
  Auxilia instituições de ensino e pesquisa ao oferecer uma ferramenta que facilita o aprendizado sobre gestão de insumos laboratoriais, incentivando boas práticas científicas e a responsabilidade no uso de materiais.

- **ODS 9 – Indústria, Inovação e Infraestrutura:**  
  Envolve o desenvolvimento de uma solução tecnológica inovadora voltada à gestão de recursos laboratoriais, otimizando processos e incentivando a automação e a eficiência nas atividades científicas.

- **ODS 12 – Consumo e Produção Responsáveis:**  
  Contribui para o uso racional de reagentes e o controle de desperdício, garantindo o descarte adequado de produtos vencidos e reduzindo impactos ambientais.

---

## 🧱 Status do Projeto
📍 *Em desenvolvimento* — fase de modelagem e definição de requisitos.
