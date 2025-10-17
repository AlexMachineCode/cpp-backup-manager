# 🧰 C++ Backup Manager

Um sistema de **backup e restauração de arquivos** desenvolvido em **C++**, com foco em **segurança, robustez e testes automatizados**.  
Este projeto foi desenvolvido como parte de um trabalho acadêmico, mas estruturado seguindo boas práticas de engenharia de software.

---

## 📋 Descrição do Projeto

O sistema realiza **cópias de segurança (backup)** e **restauração** de arquivos entre o HD e um diretório de destino (simulado como `"pendrive"`).  
Ele foi projetado com **testes automatizados** utilizando o framework **Catch2**, garantindo que cada cenário de uso seja validado corretamente.

### Funcionalidades principais:
- 📦 Cópia de novos arquivos  
- 🔁 Sobrescrita de arquivos antigos  
- ⏸️ Ignora arquivos com datas iguais  
- ⚠️ Tratamento de erros (arquivo inexistente, permissão negada, etc.)  
- 🧾 Registro detalhado de operações no arquivo `Backup.log`  
- 📊 Resumo final com contagem de **Copiados**, **Ignorados** e **Erros**

---

## 🧠 Conceitos e Técnicas Utilizadas

- Manipulação de arquivos (`<fstream>`, `<sys/stat.h>`, `<utime.h>`)
- Controle de datas e permissões com `stat` e `utime`
- Testes unitários com **Catch2**
- Uso de **assertivas** e **tratamento de erros**
- Modularização e **refatoração incremental**
- Controle de versão com **Git** e commits semânticos (`feat`, `fix`, `refactor`)
- Automação de build com **Makefile**

---

## ⚙️ Como Compilar e Executar

Certifique-se de ter o **g++** e o **Makefile** configurados no ambiente Linux (funciona perfeitamente no WSL também).

```bash
# Compila e executa todos os testes
make
bash
Copiar código
# Remove binários e arquivos temporários
make clean
Os testes são executados automaticamente via o arquivo testa_backup.cpp usando o Catch2.

🧪 Testes Automatizados
O projeto conta com 13 casos de teste e 19 assertivas implementadas com o framework Catch2, cobrindo os seguintes cenários:

Falta do arquivo Backup.parm

Cópia de arquivo novo

Sobrescrita de arquivo antigo

Arquivos com mesma data

Destino mais novo que a origem

Restauração com origem antiga

Erro de permissão no diretório de destino

Arquivos inexistentes na origem

Geração e validação do Backup.log com resumo final

Exemplo de execução bem-sucedida:

markdown
Copiar código
===============================================================================
All tests passed (19 assertions in 13 test cases)
===============================================================================
📁 Estrutura do Projeto
php
Copiar código
trabalho2-backup/
├── backup.cpp           # Implementação principal do sistema
├── backup.hpp           # Cabeçalho com definições e constantes
├── testa_backup.cpp     # Testes automatizados com Catch2
├── catch.hpp            # Framework de testes embutido
├── Makefile             # Script de build
├── relatorio.txt        # Relatório final do projeto
└── README.md            # Este arquivo
🧾 Relatório do Projeto
Durante o desenvolvimento, foi aplicada a metodologia TDD (Test-Driven Development), seguindo os ciclos:

Red – Criação de testes falhando intencionalmente.

Green – Implementação mínima para passar nos testes.

Refactor – Limpeza e melhoria do código sem alterar o comportamento.

Cada ciclo foi versionado com commits semânticos e mensagens descritivas no Git.
Ao final, o sistema passou por refatorações para eliminar duplicações e garantir clareza na lógica de backup e restauração.
