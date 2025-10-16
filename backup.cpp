// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <unistd.h>  // para acesso a permissões
#include <ctime>

/***************************************************************************
 * Função: getFileModTime
 * -------------------------------------------------------------------------
 * Retorna o tempo de modificação de um arquivo no sistema.
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
 * -------------------------------------------------------------------------
 * Copia o conteúdo de um arquivo de origem para um destino.
 ***************************************************************************/
void copiarArquivo(const std::string& origem, const std::string& destino) {
  assert(!origem.empty());
  assert(!destino.empty());
  std::ifstream src(origem, std::ios::binary);
  assert(src.is_open());
  std::ofstream dst(destino, std::ios::binary);
  assert(dst.is_open());
  dst << src.rdbuf();
}

/***************************************************************************
 * Função auxiliar: possuiPermissaoEscrita
 * -------------------------------------------------------------------------
 * Verifica se o processo atual tem permissão de escrita no diretório dado.
 ***************************************************************************/
bool possuiPermissaoEscrita(const std::string& diretorio) {
  return (access(diretorio.c_str(), W_OK) == 0);
}

/***************************************************************************
 * Função auxiliar: processarTransferencia
 * -------------------------------------------------------------------------
 * Generaliza a lógica de backup/restauração, incluindo validações de erro.
 ***************************************************************************/
int processarTransferencia(const std::string& origem, const std::string& destino,
                           int erroMaisNovo, int erroMaisAntigo) {
  assert(!origem.empty());
  assert(!destino.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) return ERRO_BACKUP_PARM_NAO_EXISTE;

  // 🔒 Verificação de permissão de escrita
  if (!possuiPermissaoEscrita(destino)) {
    param_file.close();
    return ERRO_SEM_PERMISSAO;
  }

  std::string nome_arquivo;
  while (param_file >> nome_arquivo) {
    const std::string path_origem = origem + "/" + nome_arquivo;
    const std::string path_destino = destino + "/" + nome_arquivo;

    const time_t data_origem = getFileModTime(path_origem);
    const time_t data_destino = getFileModTime(path_destino);

    if (data_origem == 0) {
      param_file.close();
      return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;
    }

    if (data_origem > data_destino) {
      copiarArquivo(path_origem, path_destino);
    } else if (data_destino > data_origem) {
      param_file.close();
      return (erroMaisNovo != 0) ? erroMaisNovo : erroMaisAntigo;
    }
  }

  param_file.close();
  return OPERACAO_SUCESSO;
}

/***************************************************************************
 * Função: realizaBackup
 ***************************************************************************/
int realizaBackup(const std::string& destino_path) {
  return processarTransferencia(".", destino_path,
                                ERRO_DESTINO_MAIS_NOVO, 0);
}

/***************************************************************************
 * Função: realizaRestauracao
 ***************************************************************************/
int realizaRestauracao(const std::string& origem_path) {
  return processarTransferencia(origem_path, ".",
                                0, ERRO_ORIGEM_MAIS_ANTIGA);
}
