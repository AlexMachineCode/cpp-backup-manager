// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <ctime>
#include <cerrno>
#include <cstring>
#include <iostream>

/***************************************************************************
 * Função: getFileModTime
 ***************************************************************************/
time_t getFileModTime(const std::string& path) {
  assert(!path.empty());
  struct stat result;
  if (stat(path.c_str(), &result) == 0) {
    return result.st_mtime;
  }
  return 0;
}

/***************************************************************************
 * Função: copiarArquivo
 ***************************************************************************/
void copiarArquivo(const std::string& origem, const std::string& destino) {
  assert(!origem.empty());
  assert(!destino.empty());

  // Garante que o diretório de destino existe
  size_t pos = destino.find_last_of('/');
  if (pos != std::string::npos) {
    std::string dir = destino.substr(0, pos);
    mkdir(dir.c_str(), 0777);
  }

  std::ifstream src(origem, std::ios::binary);
  assert(src.is_open());

  std::ofstream dst(destino, std::ios::binary);
  if (!dst.is_open()) {
    std::cerr << "[ERRO] Não foi possível criar destino: "
              << destino << " (errno=" << errno << ")\n";
    return;
  }

  dst << src.rdbuf();
}

/***************************************************************************
 * Função: realizaBackup
 ***************************************************************************/
int realizaBackup(const std::string& destino_path) {
  assert(!destino_path.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) return ERRO_BACKUP_PARM_NAO_EXISTE;

  std::ofstream log("Backup.log", std::ios::app);
  int copiados = 0, ignorados = 0, erros = 0;
  int codigo_final = OPERACAO_SUCESSO;

  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    const std::string source_path = nome_arquivo;
    const std::string dest_path = destino_path + "/" + nome_arquivo;

    const time_t data_origem = getFileModTime(source_path);
    if (data_origem == 0) {
      erros++;
      log << "[ERRO] Arquivo nao encontrado: " << source_path << std::endl;
      codigo_final = ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;
      continue; // <--- IMPORTANTE: não sai da função
    }

    struct stat st;
    if (stat(destino_path.c_str(), &st) != 0 || !(st.st_mode & S_IWUSR)) {
      erros++;
      log << "[ERRO] Sem permissao de escrita em: " << destino_path << std::endl;
      codigo_final = ERRO_SEM_PERMISSAO;
      continue;
    }

    const time_t data_destino = getFileModTime(dest_path);

    if (data_destino > data_origem) {
      erros++;
      log << "[ERRO] Destino mais novo: " << dest_path << std::endl;
      codigo_final = ERRO_DESTINO_MAIS_NOVO;
      continue;
    } else if (data_origem > data_destino) {
      copiarArquivo(source_path, dest_path);
      log << "[OK] COPIADO: " << nome_arquivo << std::endl;
      copiados++;
    } else {
      log << "[IGNORADO] " << nome_arquivo << std::endl;
      ignorados++;
    }
  }

  // --- RESUMO FINAL ---
  if (log.is_open()) {
    log << "[RESUMO] "
        << "Copiados: " << copiados
        << " | Ignorados: " << ignorados
        << " | Erros: " << erros
        << std::endl;
    log.flush();
    log.close();
  }

  param_file.close();
  return codigo_final;
}
/***************************************************************************
 * Função: realizaRestauracao
 ***************************************************************************/
int realizaRestauracao(const std::string& origem_path) {
  assert(!origem_path.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) return ERRO_BACKUP_PARM_NAO_EXISTE;

  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    const std::string source_path = origem_path + "/" + nome_arquivo;
    const std::string dest_path = nome_arquivo;

    const time_t data_origem = getFileModTime(source_path);
    if (data_origem == 0) return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;

    const time_t data_destino = getFileModTime(dest_path);

    if (data_origem < data_destino) return ERRO_ORIGEM_MAIS_ANTIGA;
    else if (data_origem > data_destino) copiarArquivo(source_path, dest_path);
  }

  return OPERACAO_SUCESSO;
}
