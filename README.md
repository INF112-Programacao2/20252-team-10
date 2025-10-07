# 🧪 Sistema de Gestão de Estoque de Reagentes de Laboratório

## 📘 Sobre o Projeto
O *Sistema de Gestão de Estoque de Reagentes de Laboratório* tem como objetivo gerenciar o controle de reagentes químicos, permitindo o registro detalhado de entradas, saídas, prazos de validade e locais de armazenamento.

O sistema emite *alertas automáticos* para reagentes próximos do vencimento ou com estoque baixo, facilitando a reposição e garantindo a *segurança das atividades laboratoriais*.

Além disso, o sistema possui diferentes *perfis de usuário* — Gestores Master, Gestores e Membros — cada um com níveis de permissão específicos, assegurando *controle de acesso* e confiabilidade nas informações.

---

## 👥 Integrantes do Projeto
- Gustavo Santos Pinto  
- Marcus Vinicius de Souza Santos Prado  
- Nathalia de Assis Almeida  
- Rafael Rangel Fietto  
- Tais dos Santos Ferreira  

---

## 🧭 Funcionalidades Principais
- Cadastro e controle de reagentes (nome, validade, quantidade, unidade, local de armazenamento).  
- Registro de retiradas e atualização automática do estoque.  
- Alertas automáticos para reagentes vencidos ou com estoque crítico.  
- Gestão de usuários: cadastro e remoção de membros e gestores.  
- Controle de acesso hierárquico (Gestores Master, Gestores, Membros).  
- Busca e filtros por nome, categoria ou local de armazenamento.  
- Histórico de movimentações e rastreabilidade completa das ações.  

---

## 👤 Perfis de Usuário

### 🧪 Membro do Laboratório
- Registrar retirada de reagentes.  
- Consultar quantidade e validade dos reagentes.  
- Visualizar local de armazenamento (geladeira, freezer, armário etc.).  
- Receber aviso ao tentar retirar reagente vencido.  

### ⚙️ Gestor do Laboratório
- Cadastrar e remover membros.  
- Adicionar, repor e excluir reagentes.  
- Receber alertas automáticos de reagentes vencidos ou com estoque crítico.  
- Gerenciar o estoque e supervisionar retiradas.  
- Consultar relatórios e histórico de movimentações.  

### 🧭 Gestor Master
- Criar e remover gestores (polimorfismo).  
- Alocar gestores em laboratórios.  
- Gerenciar múltiplos laboratórios.  
- Supervisionar todas as operações do sistema.  
- Possui todas as permissões de Gestor e controle administrativo completo.  

---

## 📜 User Stories (Histórias de Usuário)

| ID | História do Usuário | Critérios de Aceitação |
|----|-------------------|----------------------|
| 1 | Como membro, quero registrar a retirada de um reagente para controlar o estoque. | Atualiza estoque, confirma registro, impede retirada além do disponível e alerta gestor se atingir nível crítico. |
| 2 | Como membro, quero ver a quantidade atual de um reagente. | Exibe quantidade e unidade, indica nível crítico, permite busca e atualização em tempo real. |
| 3 | Como gestor, quero ser avisado sobre reagentes com estoque crítico. | Exibe notificações automáticas ao login, mostra lista com quantidades e acesso à reposição. |
| 4 | Como gestor, quero ser alertado sobre reagentes vencidos. | Verifica validade, impede retirada vencida e permite remoção pelo gestor. |
| 5 | Como gestor, quero cadastrar e remover membros. | Permite cadastro e exclusão com confirmação e bloqueio se houver pendências. |
| 6 | Como gestor, quero adicionar novos reagentes. | Cadastra com nome, quantidade, validade e local; atualiza estoque e registra quem cadastrou. |
| 7 | Como membro, quero visualizar onde o reagente está armazenado. | Mostra e permite atualização da localização e busca por local. |
| 8 | Como gestor master, quero criar ou remover gestores. | Permite criar e deletar contas de gestores, alocar laboratórios e supervisionar atividades. |

---

## 🧩 Estrutura de Classes (Cartões CRC)

| Classe | Responsabilidades | Colaborações |
|--------|-----------------|--------------|
| Usuário | Fazer login, editar perfil, retirar e devolver reagentes, consultar e filtrar reagentes. | Retirada, Reagente, Laboratório |
| Gestor Master | Criar/deletar gestores (polimorfismo), alocar gestores a laboratórios, supervisionar gestão geral. | Gestor, Laboratório |
| Gestor | Criar/deletar usuários, adicionar/excluir/repor reagentes, receber alertas, gerenciar laboratório. | Reagente, Laboratório, Alerta, Usuário |
| Reagente | Armazenar dados, calcular validade e nível crítico, atualizar quantidade, exibir condições de armazenamento (polimorfismo). | Laboratório, Alerta, Usuário, Retirada |
| Laboratório | Listar reagentes, controlar entradas e saídas, registrar histórico de retiradas. | Reagente, Retirada, Gestor, Usuário |
| Retirada | Registrar data, hora, usuário e quantidade retirada. | Usuário, Reagente, Laboratório, Gestor |
| Alerta | Monitorar validade e quantidade crítica, enviar notificações, armazenar histórico. | Reagente, Gestor |

---

## ⚙️ Tecnologias
- *Backend:* C++  
- *Frontend:* React  
- *Banco de Dados:* Em estudo (compatível com C++)  
- *Controle de Versão:* Git e GitHub  

---

## 🎯 Objetivo Geral
Desenvolver um sistema intuitivo e eficiente para gerenciamento de reagentes químicos em laboratórios, promovendo *organização, rastreabilidade e segurança* no uso e armazenamento.

---

## 🌍 Contribuição para os ODS (Objetivos de Desenvolvimento Sustentável)
- *ODS 3 – Saúde e Bem-Estar:* Previne o uso de reagentes vencidos, aumentando a segurança laboratorial.  
- *ODS 4 – Educação de Qualidade:* Apoia instituições de ensino no aprendizado sobre boas práticas laboratoriais.  
- *ODS 9 – Indústria, Inovação e Infraestrutura:* Soluções tecnológicas para otimização de processos.  
- *ODS 12 – Consumo e Produção Responsáveis:* Uso racional de reagentes e descarte adequado.  

---

## 🧱 Status do Projeto
📍 *Em desenvolvimento* — Etapa de modelagem e definição de requisitos.
