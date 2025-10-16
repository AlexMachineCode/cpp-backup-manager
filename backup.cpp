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
 * Descrição:
 *   Retorna o tempo de modificação de um arquivo no sistema.
 * Parâmetros:
 *   path - Caminho do arquivo a ser consultado.
 * Valor retornado:
 *   Tempo de modificação (time_t) ou 0 se o arquivo não existir.
 * Assertiva de entrada:
 *   path não deve ser string vazia.
 * Assertiva de saída:
 *   Retorno >= 0.
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
 * Descrição:
 *   Copia o conteúdo de um arquivo de origem para um destino.
 * Parâmetros:
 *   origem  - Caminho do arquivo de origem.
 *   destino - Caminho do arquivo de destino.
 * Assertiva de entrada:
 *   origem e destino devem ser strings não vazias e acessíveis.
 * Assertiva de saída:
 *   O arquivo destino deve existir após a cópia.
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
 * Função: realizaBackup
 * -------------------------------------------------------------------------
 * Descrição:
 *   Lê o arquivo "Backup.parm" e executa o backup dos arquivos listados,
 *   copiando-os para o diretório de destino informado.
 * Parâmetros:
 *   destino_path - Caminho do diretório onde os arquivos serão copiados.
 * Valor retornado:
 *   OPERACAO_SUCESSO                - se o backup foi realizado com êxito.
 *   ERRO_BACKUP_PARM_NAO_EXISTE     - se o arquivo "Backup.parm" não foi encontrado.
 *   ERRO_ARQUIVO_ORIGEM_NAO_EXISTE  - se algum arquivo de origem não existir.
 *   ERRO_DESTINO_MAIS_NOVO          - se o arquivo destino for mais novo que o de origem.
 * Assertiva de entrada:
 *   destino_path não é string vazia.
 * Assertiva de saída:
 *   Retorno corresponde a um valor definido no enum StatusOperacao.
 ***************************************************************************/
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

/***************************************************************************
 * Função: realizaRestauracao
 * -------------------------------------------------------------------------
 * Descrição:
 *   Lê o arquivo "Backup.parm" e restaura os arquivos do diretório de origem
 *   (pendrive) para o diretório atual, respeitando as datas de modificação.
 * Parâmetros:
 *   origem_path - Caminho do diretório de origem (pendrive).
 * Valor retornado:
 *   OPERACAO_SUCESSO            - se a restauração for bem-sucedida.
 *   ERRO_BACKUP_PARM_NAO_EXISTE - se o arquivo "Backup.parm" não foi encontrado.
 *   ERRO_ORIGEM_MAIS_ANTIGA     - se a origem for mais antiga que o destino.
 * Assertiva de entrada:
 *   origem_path não é string vazia.
 * Assertiva de saída:
 *   Retorno corresponde a um valor definido no enum StatusOperacao.
 ***************************************************************************/
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
