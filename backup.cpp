// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>  // Para obter informações do arquivo (data)
#include <ctime>       // Para o tipo time_t

// Função auxiliar para obter a data da última modificação de um arquivo
time_t getFileModTime(const std::string& path) {
  struct stat result;
  if (stat(path.c_str(), &result) == 0) {
    return result.st_mtime;
  }
  return 0;  // Retorna 0 se o arquivo não for encontrado
}

/******************************************************************************
 * @brief Função: realizaBackup
 * @details
 * Inicia o processo de backup. Lê os arquivos do "Backup.parm", compara as
 * datas de modificação com os arquivos no destino e age conforme a Tabela de
 * Decisão.
 *
 * @param destino_path O caminho para o diretório de destino (o "pendrive").
 *
 * @return
 * Retorna um código de status da enumeração StatusOperacao.
 * - ERRO_BACKUP_PARM_NAO_EXISTE: Se "Backup.parm" não for encontrado.
 * - ERRO_DESTINO_MAIS_NOVO: Se um arquivo no destino for mais novo que o da
 * origem durante um backup.
 * - OPERACAO_SUCESSO: Se a operação for concluída.
 *
 * @assertiva-entrada
 * - destino_path não deve ser um caminho vazio.
 *
 * @assertiva-saida
 * - Se retornar SUCESSO, os arquivos no destino são tão ou mais novos que os
 * da origem.
 * - Se retornar um erro, o diretório de destino não foi modificado.
 ******************************************************************************/
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
      // A origem é mais nova, então copia.
      std::ifstream src(source_path, std::ios::binary);
      assert(src.is_open());
      std::ofstream dst(dest_path, std::ios::binary);
      assert(dst.is_open());
      dst << src.rdbuf();
      src.close();
      dst.close();
    } else if (data_destino > data_origem) {
      // O destino é mais novo, isso é um erro no backup.
      param_file.close();  // Fecha o arquivo antes de sair.
      return ERRO_DESTINO_MAIS_NOVO;
    }
    // NOTA: Se as datas forem iguais, nada acontece, o que está correto.
  }

  param_file.close();

  return OPERACAO_SUCESSO;
}