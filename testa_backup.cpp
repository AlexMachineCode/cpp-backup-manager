// Copyright 2025 Alex Batista Resende
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "backup.hpp"  // NOLINT

#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
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
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_modificado.txt";
  std::ofstream("pendrive/arquivo_modificado.txt") << "conteudoantigo";
  sleep(1);
  std::ofstream("arquivo_modificado.txt") << "conteudonovo";

  realizaBackup("pendrive");

  std::ifstream arquivo_atualizado("pendrive/arquivo_modificado.txt");
  std::stringstream buffer;
  buffer << arquivo_atualizado.rdbuf();
  std::string conteudo_final = buffer.str();

  REQUIRE(conteudo_final == "conteudonovo");

  remove("Backup.parm");
  remove("arquivo_modificado.txt");
  remove("pendrive/arquivo_modificado.txt");
  rmdir("pendrive");
}

TEST_CASE("Backup nao copia arquivos com datas iguais", "[backup-data-igual]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_data_igual.txt";
  std::ofstream("arquivo_data_igual.txt") << "conteudonovo";
  std::ofstream("pendrive/arquivo_data_igual.txt") << "conteudoantigo";

  realizaBackup("pendrive");

  std::ifstream arquivo_atualizado("pendrive/arquivo_data_igual.txt");
  std::stringstream buffer;
  buffer << arquivo_atualizado.rdbuf();
  std::string conteudo_final = buffer.str();

  REQUIRE(conteudo_final == "conteudoantigo");

  remove("Backup.parm");
  remove("arquivo_data_igual.txt");
  remove("pendrive/arquivo_data_igual.txt");
  rmdir("pendrive");
}

TEST_CASE("Backup gera erro se arquivo de destino for mais novo", "[backup-erro-data]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_conflito.txt";
  std::ofstream("arquivo_conflito.txt") << "conteudoantigo";
  sleep(1);
  std::ofstream("pendrive/arquivo_conflito.txt") << "conteudonovo";

  REQUIRE(realizaBackup("pendrive") == ERRO_DESTINO_MAIS_NOVO);

  remove("Backup.parm");
  remove("arquivo_conflito.txt");
  remove("pendrive/arquivo_conflito.txt");
  rmdir("pendrive");
}

TEST_CASE("Restauracao gera erro se arquivo de origem for mais antigo", "[restauracao-erro]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_rest.txt";
  std::ofstream("pendrive/arquivo_rest.txt") << "conteudoantigo";
  sleep(1);
  std::ofstream("arquivo_rest.txt") << "conteudonovo";

  REQUIRE(realizaRestauracao("pendrive") == ERRO_ORIGEM_MAIS_ANTIGA);

  remove("Backup.parm");
  remove("arquivo_rest.txt");
  remove("pendrive/arquivo_rest.txt");
  rmdir("pendrive");
}

TEST_CASE("Restauracao nao copia arquivos com datas iguais", "[restauracao-data-igual]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_rest_igual.txt";
  std::ofstream("arquivo_rest_igual.txt") << "conteudo-HD";
  time_t data_destino = getFileModTime("arquivo_rest_igual.txt");
  std::ofstream("pendrive/arquivo_rest_igual.txt") << "conteudo-pendrive";

  struct utimbuf new_times;
  new_times.actime = data_destino;
  new_times.modtime = data_destino;
  utime("pendrive/arquivo_rest_igual.txt", &new_times);

  realizaRestauracao("pendrive");

  std::ifstream arquivo_final("arquivo_rest_igual.txt");
  std::stringstream buffer;
  buffer << arquivo_final.rdbuf();
  arquivo_final.close();

  REQUIRE(buffer.str() == "conteudo-HD");

  remove("Backup.parm");
  remove("arquivo_rest_igual.txt");
  remove("pendrive/arquivo_rest_igual.txt");
  rmdir("pendrive");
}

TEST_CASE("Restauracao atualiza arquivo no HD se o pendrive for mais novo", "[restauracao-sucesso]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_rest_atualiza.txt";
  std::ofstream("arquivo_rest_atualiza.txt") << "conteudo-antigo";
  sleep(1);
  std::ofstream("pendrive/arquivo_rest_atualiza.txt") << "conteudo-novo";

  realizaRestauracao("pendrive");

  std::ifstream arquivo_final("arquivo_rest_atualiza.txt");
  std::stringstream buffer;
  buffer << arquivo_final.rdbuf();

  REQUIRE(buffer.str() == "conteudo-novo");

  remove("Backup.parm");
  remove("arquivo_rest_atualiza.txt");
  remove("pendrive/arquivo_rest_atualiza.txt");
  rmdir("pendrive");
}

TEST_CASE("Backup gera erro se arquivo de origem nao existe", "[backup-erro-origem]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_inexistente.txt";

  // 💡 Garante que o arquivo realmente não existe antes do teste
  remove("arquivo_inexistente.txt");

  REQUIRE(realizaBackup("pendrive") == ERRO_ARQUIVO_ORIGEM_NAO_EXISTE);

  remove("Backup.parm");
  rmdir("pendrive");
}

TEST_CASE("Restauracao gera erro se arquivo de origem nao existe", "[restauracao-erro-origem]") {
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_inexistente_rest.txt";

  // 💡 Garante que o arquivo realmente não existe no pendrive
  remove("pendrive/arquivo_inexistente_rest.txt");

  REQUIRE(realizaRestauracao("pendrive") == ERRO_ARQUIVO_ORIGEM_NAO_EXISTE);

  remove("Backup.parm");
  rmdir("pendrive");
}

TEST_CASE("Backup gera erro se destino nao tiver permissao de escrita", "[backup-erro-permissao]") {
  // --- PREPARAÇÃO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_permissao.txt";
  std::ofstream("arquivo_permissao.txt") << "dados";

  // Remove permissão de escrita do diretório de destino
  chmod("pendrive", 0555);

  // --- AÇÃO ---
  int resultado = realizaBackup("pendrive");

  // --- VERIFICAÇÃO ---
  REQUIRE(resultado == ERRO_SEM_PERMISSAO);

  // --- LIMPEZA ---
  chmod("pendrive", 0777);  // restaura permissão pra poder apagar
  remove("Backup.parm");
  remove("arquivo_permissao.txt");
  rmdir("pendrive");
}

TEST_CASE("Backup registra operacoes no arquivo de log", "[backup-log]") {
  // --- PREPARAÇÃO ---
  mkdir("pendrive", 0777);
  std::ofstream("Backup.parm") << "arquivo_log.txt";
  std::ofstream("arquivo_log.txt") << "conteudo";

  // Garante que o log não existe antes
  remove("Backup.log");

  // --- AÇÃO ---
  realizaBackup("pendrive");

  // --- VERIFICAÇÃO ---
  std::ifstream log("Backup.log");
  REQUIRE(log.good());  // O log deve existir

  std::stringstream buffer;
  buffer << log.rdbuf();
  std::string conteudo_log = buffer.str();

  REQUIRE(conteudo_log.find("arquivo_log.txt") != std::string::npos);
  REQUIRE(conteudo_log.find("COPIADO") != std::string::npos);

  // --- LIMPEZA ---
  remove("Backup.parm");
  remove("arquivo_log.txt");
  remove("pendrive/arquivo_log.txt");
  remove("Backup.log");
  rmdir("pendrive");
}