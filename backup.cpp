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

// -- Função de Backup (COMPLETA E CORRIGIDA) --
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
      std::ifstream src(source_path, std::ios::binary);
      assert(src.is_open());
      std::ofstream dst(dest_path, std::ios::binary);
      assert(dst.is_open());
      dst << src.rdbuf();
      src.close();
      dst.close();
    } else if (data_destino > data_origem) {
      param_file.close();
      return ERRO_DESTINO_MAIS_NOVO;
    }
  }
  param_file.close();
  return OPERACAO_SUCESSO;  // <-- O RETURN QUE FALTAVA!
}

// -- Função de Restauração (COMPLETA E DOCUMENTADA) --
/******************************************************************************
 * @brief Função: realizaRestauracao
 * @details
 * Inicia o processo de restauração de arquivos de uma origem (pendrive) para
 * o diretório local (HD). A lógica é guiada pela Tabela de Decisão.
 *
 * @param origem_path O caminho para o diretório de origem (o "pendrive").
 *
 * @return
 * Retorna um código de status da enumeração StatusOperacao.
 * - ERRO_ORIGEM_MAIS_ANTIGA: Se um arquivo na origem for mais antigo que o
 * do destino durante uma restauração.
 *
 * @assertiva-entrada
 * - origem_path não deve ser um caminho vazio.
 *
 * @assertiva-saida
 * - Se retornar um erro, o diretório de destino (HD) não foi modificado.
 ******************************************************************************/
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
    }
  }
  param_file.close();
  return OPERACAO_SUCESSO;
}