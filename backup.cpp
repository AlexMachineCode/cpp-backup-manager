// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <ctime>

int getFileModTime(const std::string& path) {
  struct stat result;
  if (stat(path.c_str(), &result) == 0) {
    return result.st_mtime;
  }
  return 0;
}

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

    // LÓGICA SIMPLES (FIX): Apenas sobrescreve
    std::ifstream src(source_path, std::ios::binary);
    assert(src.is_open());
    std::ofstream dst(dest_path, std::ios::binary);
    assert(dst.is_open());
    dst << src.rdbuf();
    src.close();
    dst.close();
  }

  param_file.close();
  return OPERACAO_SUCESSO;
}