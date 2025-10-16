// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <unistd.h>  // Para função access()
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
 * Função: possuiPermissaoEscrita
 * -------------------------------------------------------------------------
 * Verifica se o processo atual tem permissão de escrita em um diretório.
 * Parâmetros:
 *   diretorio - Caminho do diretório a ser verificado.
 * Retorno:
 *   true  - se o processo tiver permissão de escrita.
 *   false - caso contrário.
 ***************************************************************************/
bool possuiPermissaoEscrita(const std::string& diretorio) {
  assert(!diretorio.empty());
  return (access(diretorio.c_str(), W_OK) == 0);
}

/***************************************************************************
 * Função: validarDestino
 * -------------------------------------------------------------------------
 * Realiza todas as verificações de pré-condição sobre o destino:
 *  - Existência de permissão de escrita.
 *  - Existência do arquivo de parâmetros.
 * Caso alguma falhe, retorna o código de erro apropriado.
 ***************************************************************************/
int validarDestino(const std::string& destino) {
  if (!possuiPermissaoEscrita(destino)) {
    return ERRO_SEM_PERMISSAO;
  }
  std::ifstream parm("Backup.parm");
  if (!parm.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }
  return OPERACAO_SUCESSO;
}

/***************************************************************************
 * Função auxiliar: processarTransferencia
 * -------------------------------------------------------------------------
 * Generaliza a lógica comum de backup/restauração,
 * incluindo validações de permissão, existência e data.
 ***************************************************************************/
int processarTransferencia(const std::string& origem, const std::string& destino,
                           int erroMaisNovo, int erroMaisAntigo) {
  assert(!origem.empty());
  assert(!destino.empty());

  int validacao = validarDestino(destino);
  if (validacao != OPERACAO_SUCESSO) return validacao;

  std::ifstream param_file("Backup.parm");
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
