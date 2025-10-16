// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <ctime>

/**
 * Retorna o tempo de modificação de um arquivo no sistema.
 * @param path Caminho do arquivo a ser consultado.
 * @return Tempo de modificação (time_t) ou 0 se o arquivo não existir.
 */
time_t getFileModTime(const std::string& path) {
  struct stat result;
  if (stat(path.c_str(), &result) == 0) {
    return result.st_mtime;
  }
  return 0;
}

/**
 * Copia o conteúdo de um arquivo de origem para um destino.
 * @param origem Caminho do arquivo de origem.
 * @param destino Caminho do arquivo de destino.
 */
void copiarArquivo(const std::string& origem, const std::string& destino) {
  std::ifstream src(origem, std::ios::binary);
  assert(src.is_open());
  std::ofstream dst(destino, std::ios::binary);
  assert(dst.is_open());
  dst << src.rdbuf();
  src.close();
  dst.close();
}

/**
 * Realiza o backup dos arquivos listados em "Backup.parm"
 * para o diretório de destino informado.
 *
 * @param destino_path Caminho do diretório de destino.
 * @return Código de status da operação.
 */
int realizaBackup(const std::string& destino_path) {
  assert(!destino_path.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    const std::string source_path = nome_arquivo;
    const std::string dest_path = destino_path + "/" + nome_arquivo;

    const time_t data_origem = getFileModTime(source_path);

    // 🔥 Correção: arquivo de origem inexistente
    if (data_origem == 0) {
      param_file.close();
      return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;
    }

    const time_t data_destino = getFileModTime(dest_path);

    if (data_origem > data_destino) {
      copiarArquivo(source_path, dest_path);
    } else if (data_destino > data_origem) {
      param_file.close();
      return ERRO_DESTINO_MAIS_NOVO;
    }
  }

  param_file.close();
  return OPERACAO_SUCESSO;
}

/**
 * Realiza a restauração dos arquivos a partir do diretório de origem (pendrive)
 * para o diretório atual, respeitando as datas de modificação.
 *
 * @param origem_path Caminho do diretório de origem.
 * @return Código de status da operação.
 */
int realizaRestauracao(const std::string& origem_path) {
  assert(!origem_path.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    const std::string source_path = origem_path + "/" + nome_arquivo;
    const std::string dest_path = nome_arquivo;

    const time_t data_origem = getFileModTime(source_path);

    // 🔥 Correção: arquivo de origem inexistente
    if (data_origem == 0) {
      param_file.close();
      return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;
    }

    const time_t data_destino = getFileModTime(dest_path);

    if (data_origem < data_destino) {
      param_file.close();
      return ERRO_ORIGEM_MAIS_ANTIGA;
    } else if (data_origem > data_destino) {
      copiarArquivo(source_path, dest_path);
    }
  }

  param_file.close();
  return OPERACAO_SUCESSO;
}
