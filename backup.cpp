// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <unistd.h>  // Para função access()
#include <ctime>
#include <iomanip>   // Para std::put_time
#include <sstream>

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
 ***************************************************************************/
bool possuiPermissaoEscrita(const std::string& diretorio) {
  assert(!diretorio.empty());
  return (access(diretorio.c_str(), W_OK) == 0);
}

/***************************************************************************
 * Função: registrarLog
 * -------------------------------------------------------------------------
 * Registra uma linha no arquivo Backup.log, contendo:
 *   [timestamp] nome_do_arquivo - mensagem
 ***************************************************************************/
void registrarLog(const std::string& arquivo, const std::string& mensagem) {
  std::ofstream log("Backup.log", std::ios::app);
  if (!log.is_open()) return;

  // Obtém data/hora atual formatada
  std::time_t agora = std::time(nullptr);
  std::tm* tempo = std::localtime(&agora);
  log << "[" << std::put_time(tempo, "%d/%m/%Y %H:%M:%S") << "] "
      << arquivo << " - " << mensagem << std::endl;
}

/***************************************************************************
 * Função auxiliar: processarTransferencia
 * -------------------------------------------------------------------------
 * Generaliza a lógica comum de backup/restauração,
 * incluindo validações de permissão, existência, data e logs.
 ***************************************************************************/
int processarTransferencia(const std::string& origem, const std::string& destino,
                           int erroMaisNovo, int erroMaisAntigo) {
  assert(!origem.empty());
  assert(!destino.empty());

  std::ifstream param_file("Backup.parm");
  if (!param_file.is_open()) {
    registrarLog("Backup.parm", "ERRO: arquivo de parametros nao encontrado");
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  if (!possuiPermissaoEscrita(destino)) {
    registrarLog(destino, "ERRO: sem permissao de escrita");
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
      registrarLog(nome_arquivo, "ERRO: arquivo de origem inexistente");
      param_file.close();
      return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;
    }

    if (data_origem > data_destino) {
      copiarArquivo(path_origem, path_destino);
      registrarLog(nome_arquivo, "COPIADO");
    } else if (data_destino > data_origem) {
      registrarLog(nome_arquivo, "IGNORADO (destino mais novo)");
      param_file.close();
      return (erroMaisNovo != 0) ? erroMaisNovo : erroMaisAntigo;
    } else {
      registrarLog(nome_arquivo, "IGNORADO (datas iguais)");
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
