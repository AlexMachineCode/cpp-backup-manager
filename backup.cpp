// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>

/******************************************************************************
 * @brief Função: realizaBackup
 * @details
 * Inicia o processo de backup. A lógica é guiada pela Tabela de Decisão.
 * Esta função lê os nomes dos arquivos do "Backup.parm" e os copia para o
 * diretório de destino se eles ainda não existirem lá.
 *
 * @param destino_path O caminho para o diretório de destino (o "pendrive").
 *
 * @return
 * Retorna um código de status da enumeração StatusOperacao.
 * - ERRO_BACKUP_PARM_NAO_EXISTE: Se o arquivo "Backup.parm" não for encontrado.
 * - OPERACAO_SUCESSO: Se a operação for concluída.
 *
 * @assertiva-entrada
 * - destino_path não deve ser um caminho vazio.
 *
 * @assertiva-saida
 * - Se a função retornar SUCESSO, os arquivos listados em "Backup.parm" que
 * não existiam no destino agora existem.
 ******************************************************************************/
int realizaBackup(const std::string& destino_path) {
  // Assertiva de Entrada: Garante que o caminho de destino não é vazio.
  assert(!destino_path.empty());

  std::ifstream param_file("Backup.parm");

  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  std::string nome_arquivo_a_copiar;
  while (param_file >> nome_arquivo_a_copiar) {
    std::string source_path = nome_arquivo_a_copiar;
    std::string dest_path = destino_path + "/" + nome_arquivo_a_copiar;

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