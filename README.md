# 🧪 Sistema de Gestão de Estoque de Reagentes de Laboratório

## 📘 Sobre o Projeto
O **Sistema de Gestão de Estoque de Reagentes de Laboratório** tem como objetivo facilitar o controle de reagentes químicos em ambientes laboratoriais, permitindo registrar entradas, saídas, prazos de validade e locais de armazenamento.

O sistema também fornece **alertas automáticos** sobre:
- Reagentes próximos do vencimento;
- Estoque crítico ou insuficiente;
- Tentativas de retirada de reagentes vencidos.

Além disso, possui diferentes perfis de usuário — **Gestores e Membros** — cada um com permissões específicas, garantindo segurança e confiabilidade dos dados.

---

## 👥 Integrantes do Projeto
- Gustavo Santos Pinto  
- Marcus Vinicius de Souza Santos Prado  
- Nathalia de Assis Almeida  
- Rafael Rangel Fietto  
- Tais dos Santos Ferreira

---

## 🧭 Funcionalidades Principais (Escopo Completo)
As funcionalidades abaixo representam o escopo total previsto. Atualmente, o projeto já possui autenticação e estrutura inicial do banco:

- Cadastro e controle de reagentes (nome, validade, quantidade, unidade, local).
- Registro de retiradas com atualização automática do estoque.
- Alertas de validade e estoque crítico.
- Gestão de usuários (cadastro e remoção de membros e gestores).
- Controle hierárquico de permissões.
- Busca e filtros por nome, categoria e local de armazenamento.
- Histórico completo de movimentações.

---

## 👤 Perfis de Usuário
### 🧪 Membro do Laboratório
- Registrar retirada de reagentes.
- Consultar quantidade e validade.
- Ver localização de armazenamento.
- Receber alertas ao tentar retirar reagente vencido.

### ⚙️ Gestor do Laboratório
- Cadastrar e remover membros.
- Adicionar, repor e excluir reagentes.
- Receber alertas de estoque e validade.
- Consultar relatórios e histórico.

---

## 📜 User Stories (Histórias de Usuário)
| ID | História | Critérios de Aceitação |
|----|----------|-------------------------|
| 1 | Como membro, quero registrar retirada | Atualiza estoque, valida quantidade, alerta gestor ao atingir nível crítico. |
| 2 | Como membro, quero consultar quantidade | Exibe quantidade e unidade, alerta para nível crítico. |
| 3 | Como gestor, quero ser avisado de estoque crítico | Exibe notificações automáticas. |
| 4 | Como gestor, quero ser alertado de vencimento | Verifica validade e impede retirada. |
| 5 | Como gestor, quero cadastrar/remover membros | Cadastro com validação e confirmação. |
| 6 | Como gestor, quero adicionar reagentes | Registra nome, validade, quantidade e local. |
| 7 | Como membro, quero ver localização | Exibe e permite buscar por local. |

---

## 🧩 Estrutura de Classes (Modelo CRC)
| Classe | Responsabilidades | Colaborações |
|--------|------------------|--------------|
| **Usuário** | Login, edição de perfil, retiradas, consultas | Retirada, Reagente, Laboratório |
| **Gestor** | Gerenciar usuários e estoque, administrar alertas | Reagente, Laboratório, Usuário |
| **Reagente** | Armazenar dados, validade, quantidade | Laboratório, Usuário, Alerta |
| **Laboratório** | Gerenciar reagentes e histórico de retiradas | Reagente, Gestor, Usuário |
| **Retirada** | Registrar data, hora, usuário e quantidade | Reagente, Usuário, Laboratório |
| **Alerta** | Monitorar condições e gerar notificações | Reagente, Gestor |

---

## ⚙️ Tecnologias e Configuração
### Tecnologias Utilizadas
- **C++** (Aplicação de Console)
- **MySQL** (X DevAPI – MySQL Connector/C++)
- **Git / GitHub**

### Pré-requisitos
- g++
- MySQL Connector/C++

## Script de setup
- Dentro da pasta resources, execute o script shell setup.sh
- Quando a janela da instalação mysql abrir, mantenha o padrão e clique ok
- Inicie o servidor mysql (normalmente sudo service mysql start)
- Inicie o banco com o comando:
```
sudo mysql -u root -p < resources/script.txt
```
- 

### 🔧 Compilação e Execução
Para compilar:
```bash
make all
```

Para executar:
```bash
./a.out
```![diagrama](https://github.com/user-attachments/assets/ffbfc8c9-2f32-488e-afee-f8ff6e3af01f)
![diagrama](https://github.com/user-attachments/assets/4fd0d338-993e-4829-8cc7-370974ea232e)


Para limpar arquivos temporários:
```bash
make clean
```

---

## 🎯 Objetivo Geral
Criar um sistema eficiente, seguro e intuitivo para gerenciar reagentes laboratoriais, assegurando organização, rastreabilidade e segurança.

---

## 🌍 Contribuição para os ODS
- **ODS 3 – Saúde e Bem-Estar**
- **ODS 4 – Educação de Qualidade**
- **ODS 9 – Indústria, Inovação e Infraestrutura**
- **ODS 12 – Consumo e Produção Responsáveis**

---

## 📄 Licença
Projeto desenvolvido para fins acadêmicos.
