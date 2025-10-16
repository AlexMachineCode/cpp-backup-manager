// Copyright 2025 Alex Batista Resende
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "backup.hpp"  // NOLINT

#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>      // Para ler o arquivo
#include <sys/stat.h>
#include <unistd.h>     // Para sleep()
#include <utime.h>

TEST_CASE("Backup falha quando Backup.parm nao existe", "[erros]") {
  remove("Backup.parm");
  REQUIRE(realizaBackup("pendrive") == ERRO_BACKUP_PARM_NAO_EXISTE);
}

TEST_CASE("Backup copia arquivo novo do HD para o Pendrive", "[backup-sucesso]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo1.txt";
  std::ofstream("arquivo1.txt") << "conteudo";
  remove("pendrive/arquivo1.txt");

  realizaBackup("pendrive");

  std::ifstream arquivo_copiado("pendrive/arquivo1.txt");
  REQUIRE(arquivo_copiado.good());

  remove("Backup.parm");
  remove("arquivo1.txt");
  remove("pendrive/arquivo1.txt");
  rmdir("pendrive");
}

TEST_CASE("Backup sobrescreve arquivo antigo no Pendrive", "[backup-atualiza]") {
  // --- PREPARAÇÃO DO CENÁRIO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_modificado.txt";

  // 1. Cria a versão ANTIGA no destino primeiro.
  std::ofstream("pendrive/arquivo_modificado.txt") << "conteudoantigo";

  // 2. ESPERA 1 SEGUNDO para garantir que a próxima data será diferente.
  sleep(1);

  // 3. Cria a versão NOVA na origem.
  std::ofstream("arquivo_modificado.txt") << "conteudonovo";

  // --- AÇÃO ---
  realizaBackup("pendrive");

  // --- VERIFICAÇÃO ---
  std::ifstream arquivo_atualizado("pendrive/arquivo_modificado.txt");
  std::stringstream buffer;
  buffer << arquivo_atualizado.rdbuf();
  std::string conteudo_final = buffer.str();

  // Este REQUIRE agora vai passar!
  REQUIRE(conteudo_final == "conteudonovo");

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_modificado.txt");
  remove("pendrive/arquivo_modificado.txt");
  rmdir("pendrive");
}




TEST_CASE("Backup nao copia arquivos com datas iguais", "[backup-data-igual]") {
  // --- PREPARAÇÃO DO CENÁRIO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_data_igual.txt";

  std::ofstream("arquivo_data_igual.txt") << "conteudonovo";
  // CORREÇÃO AQUI: o conteúdo antigo não tem hífen.
  std::ofstream("pendrive/arquivo_data_igual.txt") << "conteudoantigo";

  // --- AÇÃO ---
  realizaBackup("pendrive");

  // --- VERIFICAÇÃO ---
  std::ifstream arquivo_atualizado("pendrive/arquivo_data_igual.txt");
  std::stringstream buffer;
  buffer << arquivo_atualizado.rdbuf();
  std::string conteudo_final = buffer.str();

  REQUIRE(conteudo_final == "conteudoantigo");

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_data_igual.txt");
  remove("pendrive/arquivo_data_igual.txt");
  rmdir("pendrive");
}

TEST_CASE("Backup gera erro se arquivo de destino for mais novo", "[backup-erro-data]") {
  // --- PREPARAÇÃO DO CENÁRIO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_conflito.txt";

  // 1. Cria a versão ANTIGA na origem primeiro.
  std::ofstream("arquivo_conflito.txt") << "conteudoantigo";

  // 2. ESPERA 1 SEGUNDO para garantir que a próxima data será diferente.
  sleep(1);

  // 3. Cria a versão NOVA no destino.
  std::ofstream("pendrive/arquivo_conflito.txt") << "conteudonovo";

  // --- AÇÃO E VERIFICAÇÃO ---
  // A função deve detectar o conflito e retornar o novo código de erro.
  REQUIRE(realizaBackup("pendrive") == ERRO_DESTINO_MAIS_NOVO);

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_conflito.txt");
  remove("pendrive/arquivo_conflito.txt");
  rmdir("pendrive");
}

TEST_CASE("Restauracao gera erro se arquivo de origem for mais antigo", "[restauracao-erro]") {
  // --- PREPARAÇÃO DO CENÁRIO CORRETO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_rest.txt";

  // 1. Cria a versão ANTIGA na origem (Pendrive) primeiro.
  std::ofstream("pendrive/arquivo_rest.txt") << "conteudoantigo";

  // 2. ESPERA 1 SEGUNDO.
  sleep(1);

  // 3. Cria a versão NOVA no destino (HD).
  std::ofstream("arquivo_rest.txt") << "conteudonovo";

  // --- AÇÃO E VERIFICAÇÃO ---
  // A função deve detectar o conflito e retornar o novo código de erro.
  REQUIRE(realizaRestauracao("pendrive") == ERRO_ORIGEM_MAIS_ANTIGA);

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_rest.txt");
  remove("pendrive/arquivo_rest.txt");
  rmdir("pendrive");
}

TEST_CASE("Restauracao nao copia arquivos com datas iguais", "[restauracao-data-igual]") {
  // --- PREPARAÇÃO DO CENÁRIO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_rest_igual.txt";

  // 1. Cria o arquivo de destino (HD) com um conteúdo.
  std::ofstream("arquivo_rest_igual.txt") << "conteudo-HD";

  // 2. Obtém a data de modificação do arquivo de destino.
  time_t data_destino = getFileModTime("arquivo_rest_igual.txt");

  // 3. Cria o arquivo de origem (Pendrive) com outro conteúdo.
  std::ofstream("pendrive/arquivo_rest_igual.txt") << "conteudo-pendrive";

  // 4. Força o arquivo de origem a ter a mesma data do destino.
  //    (Isso requer a biblioteca <utime.h>)
  struct utimbuf new_times;
  new_times.actime = data_destino;
  new_times.modtime = data_destino;
  utime("pendrive/arquivo_rest_igual.txt", &new_times);

  // --- AÇÃO ---
  realizaRestauracao("pendrive");

  // --- VERIFICAÇÃO ---
  // A função não deve ter copiado, então o conteúdo do destino permanece o mesmo.
  std::ifstream arquivo_final("arquivo_rest_igual.txt");
  std::stringstream buffer;
  buffer << arquivo_final.rdbuf();
  arquivo_final.close();

  REQUIRE(buffer.str() == "conteudo-HD");

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_rest_igual.txt");
  remove("pendrive/arquivo_rest_igual.txt");
  rmdir("pendrive");
}