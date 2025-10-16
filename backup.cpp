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
 * Função auxiliar: Retorna o tempo de modificação de um arquivo
 ***************************************************************************/
time_t getFileModTime(const std::string& path) {
  assert(!path.empty());
  struct stat result;
  if (stat(path.c_str(), &result) == 0) return result.st_mtime;
  return 0;
}

/***************************************************************************
 * Função auxiliar: Verifica se há permissão de escrita em um diretório
 ***************************************************************************/
bool temPermissaoEscrita(const std::string& dir_path) {
  struct stat st;
  return (stat(dir_path.c_str(), &st) == 0) && (st.st_mode & S_IWUSR);
}

/***************************************************************************
 * Função auxiliar: Copia o conteúdo de um arquivo de origem para destino
 ***************************************************************************/
void copiarArquivo(const std::string& origem, const std::string& destino) {
  assert(!origem.empty());
  assert(!destino.empty());

  size_t pos = destino.find_last_of('/');
  if (pos != std::string::npos) {
    std::string dir = destino.substr(0, pos);
    mkdir(dir.c_str(), 0777);
  }

  std::ifstream src(origem, std::ios::binary);
  if (!src.is_open()) {
    std::cerr << "[ERRO] Não foi possível abrir origem: "
              << origem << " (errno=" << errno << ")\n";
    return;
  }

  std::ofstream dst(destino, std::ios::binary);
  if (!dst.is_open()) {
    std::cerr << "[ERRO] Não foi possível criar destino: "
              << destino << " (errno=" << errno << ")\n";
    return;
  }

  dst << src.rdbuf();
}

/***************************************************************************
 * Funções auxiliares para log
 ***************************************************************************/
void registrarLog(const std::string& mensagem) {
  std::ofstream log("Backup.log", std::ios::app);
  if (log.is_open()) log << mensagem << std::endl;
}

void registrarResumo(int copiados, int ignorados, int erros) {
  std::ofstream log("Backup.log", std::ios::app);
  if (log.is_open()) {
    log << "[RESUMO] Copiados: " << copiados
        << " | Ignorados: " << ignorados
        << " | Erros: " << erros << std::endl;
  }
}

/***************************************************************************
 * Função: realizaBackup
 ***************************************************************************/
int realizaBackup(const std::string& destino_path) {
  assert(!destino_path.empty());

  std::ifstream param("Backup.parm");
  if (!param.is_open()) return ERRO_BACKUP_PARM_NAO_EXISTE;

  int copiados = 0, ignorados = 0, erros = 0;
  std::string nome_arquivo;

  while (param >> nome_arquivo) {
    const std::string origem = nome_arquivo;
    const std::string destino = destino_path + "/" + nome_arquivo;

    time_t t_origem = getFileModTime(origem);
    if (t_origem == 0) {
      registrarLog("[ERRO] Arquivo inexistente: " + origem);
      erros++;
      continue;
    }

    if (!temPermissaoEscrita(destino_path)) {
      registrarLog("[ERRO] Sem permissão para escrever em: " + destino_path);
      return ERRO_SEM_PERMISSAO;
    }

    time_t t_dest = getFileModTime(destino);
    if (t_dest > t_origem) {
      registrarLog("[ERRO] Destino mais novo: " + destino);
      return ERRO_DESTINO_MAIS_NOVO;
    } else if (t_origem > t_dest) {
      copiarArquivo(origem, destino);
      registrarLog("[OK] COPIADO: " + nome_arquivo);
      copiados++;
    } else {
      registrarLog("[IGNORADO] " + nome_arquivo);
      ignorados++;
    }
  }

  registrarResumo(copiados, ignorados, erros);
  if (erros > 0) return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;
  return OPERACAO_SUCESSO;
}

/***************************************************************************
 * Função: realizaRestauracao
 ***************************************************************************/
int realizaRestauracao(const std::string& origem_path) {
  assert(!origem_path.empty());

  std::ifstream param("Backup.parm");
  if (!param.is_open()) return ERRO_BACKUP_PARM_NAO_EXISTE;

  std::string nome_arquivo;
  while (param >> nome_arquivo) {
    const std::string origem = origem_path + "/" + nome_arquivo;
    const std::string destino = nome_arquivo;

    time_t t_origem = getFileModTime(origem);
    if (t_origem == 0) return ERRO_ARQUIVO_ORIGEM_NAO_EXISTE;

    time_t t_dest = getFileModTime(destino);
    if (t_origem < t_dest) return ERRO_ORIGEM_MAIS_ANTIGA;
    else if (t_origem > t_dest) copiarArquivo(origem, destino);
  }

  return OPERACAO_SUCESSO;
}
