// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
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
 * Função auxiliar: processarTransferencia
 * -------------------------------------------------------------------------
 * Generaliza a lógica de transferência (backup/restauração), evitando
 * duplicação entre realizaBackup e realizaRestauracao.
 *
 * Parâmetros:
 *   origem        - Caminho base dos arquivos de origem.
 *   destino       - Caminho base dos arquivos de destino.
 *   erroMaisNovo  - Código de erro se o destino for mais novo.
 *   erroMaisAntigo- Código de erro se a origem for mais antiga.
 *
 * Retorna:
 *   OPERACAO_SUCESSO                - se tudo ocorreu bem.
 *   ERRO_BACKUP_PARM_NAO_EXISTE     - se o Backup.parm não foi encontrado.
 *   ERRO_ARQUIVO_ORIGEM_NAO_EXISTE  - se algum arquivo de origem não existe.
 ***************************************************************************/
int processarTransferencia(const std::string& origem, const std::string& destino,
                           int erroMaisNovo, int erroMaisAntigo) {
  assert(!origem.empty());
  assert(!destino.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) return ERRO_BACKUP_PARM_NAO_EXISTE;

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
 * -------------------------------------------------------------------------
 * Copia arquivos do diretório atual para o destino informado.
 ***************************************************************************/
int realizaBackup(const std::string& destino_path) {
  return processarTransferencia(".", destino_path,
                                ERRO_DESTINO_MAIS_NOVO, 0);
}

/***************************************************************************
 * Função: realizaRestauracao
 * -------------------------------------------------------------------------
 * Copia arquivos do diretório de origem (pendrive) para o diretório atual.
 ***************************************************************************/
int realizaRestauracao(const std::string& origem_path) {
  return processarTransferencia(origem_path, ".",
                                0, ERRO_ORIGEM_MAIS_ANTIGA);
}
