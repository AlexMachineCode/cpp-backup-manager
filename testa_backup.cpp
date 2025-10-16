#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "backup.hpp" 

#include <cstdio>

#include <fstream>      // Para criar e verificar arquivos
#include <sys/stat.h>   // Para usar mkdir()
#include <unistd.h>     // Para usar rmdir()

TEST_CASE("Backup falha quando Backup.parm nao existe", "[erros]") {
  
  remove("Backup.parm");

  // AÇÃO E VERIFICAÇÃO: Chama a função e verifica se o retorno é o erro esperado.
  REQUIRE(realizaBackup("pendrive") == ERRO_BACKUP_PARM_NAO_EXISTE);
}


TEST_CASE("Backup copia arquivo novo do HD para o Pendrive", "[backup-sucesso]") {
  // --- PREPARAÇÃO DO CENÁRIO ---
  // 1. Cria os diretórios e arquivos necessários
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo1.txt";
  std::ofstream("arquivo1.txt") << "conteudo do arquivo";

  // 2. Garante que o arquivo de destino não existe
  remove("pendrive/arquivo1.txt");

  // --- AÇÃO ---
  realizaBackup("pendrive");

  // --- VERIFICAÇÃO ---
  // 3. Verifica se o arquivo agora existe no destino
  std::ifstream arquivo_copiado("pendrive/arquivo1.txt");
  REQUIRE(arquivo_copiado.good());

  // --- LIMPEZA ---
  // 4. Apaga tudo que foi criado para não afetar outros testes
  remove("Backup.parm");
  remove("arquivo1.txt");
  remove("pendrive/arquivo1.txt");
  rmdir("pendrive");
}



TEST_CASE("Backup sobrescreve arquivo antigo no Pendrive", "[backup-atualiza]") {
  // --- PREPARAÇÃO DO CENÁRIO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_modificado.txt";
  std::ofstream("arquivo_modificado.txt") << "conteudonovo";  // Sem espaço
  std::ofstream("pendrive/arquivo_modificado.txt") << "conteudoantigo"; // Sem espaço

  // --- AÇÃO ---
  realizaBackup("pendrive");

  // --- VERIFICAÇÃO ---
  // Forma correta de ler o conteúdo inteiro de um arquivo pequeno
  std::ifstream arquivo_atualizado("pendrive/arquivo_modificado.txt");
  std::stringstream buffer;
  buffer << arquivo_atualizado.rdbuf();
  std::string conteudo_final = buffer.str();

  // Este REQUIRE agora vai falhar, pois o conteúdo lido será "conteudoantigo"
  REQUIRE(conteudo_final == "conteudonovo");

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_modificado.txt");
  remove("pendrive/arquivo_modificado.txt");
  rmdir("pendrive");
}