// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <ctime>

time_t getFileModTime(const std::string& path) {
  struct stat result;
  if (stat(path.c_str(), &result) == 0) {
    return result.st_mtime;
  }
  return 0;
}

// --- NOVA FUNÇÃO AUXILIAR DE REFATORAÇÃO ---
void copiarArquivo(const std::string& origem, const std::string& destino) {
  std::ifstream src(origem, std::ios::binary);
  assert(src.is_open());
  std::ofstream dst(destino, std::ios::binary);
  assert(dst.is_open());
  dst << src.rdbuf();
  src.close();
  dst.close();
}

// -- Função de Backup (agora mais limpa) --
int realizaBackup(const std::string& destino_path) {
  assert(!destino_path.empty());
  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }
  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    std::string source_path = nome_arquivo;
    std::string dest_path = destino_path + "/" + nome_arquivo;
    time_t data_origem = getFileModTime(source_path);
    time_t data_destino = getFileModTime(dest_path);
    if (data_origem > data_destino) {
      copiarArquivo(source_path, dest_path); // Usa a função auxiliar
    } else if (data_destino > data_origem) {
      param_file.close();
      return ERRO_DESTINO_MAIS_NOVO;
    }
  }
  param_file.close();
  return OPERACAO_SUCESSO;
}

// -- Função de Restauração (agora mais limpa) --
int realizaRestauracao(const std::string& origem_path) {
  assert(!origem_path.empty());
  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }
  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    std::string source_path = origem_path + "/" + nome_arquivo;
    std::string dest_path = nome_arquivo;
    time_t data_origem = getFileModTime(source_path);
    time_t data_destino = getFileModTime(dest_path);
    if (data_origem < data_destino) {
      param_file.close();
      return ERRO_ORIGEM_MAIS_ANTIGA;
    } else if (data_origem > data_destino) {
      copiarArquivo(source_path, dest_path); // Usa a função auxiliar
    }
  }
  param_file.close();
  return OPERACAO_SUCESSO;
}