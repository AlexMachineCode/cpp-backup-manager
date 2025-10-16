// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>

/******************************************************************************
 * @brief Função: realizaBackup
 * @details Inicia o processo de backup.
 * @param destino_path O caminho para o diretório de destino (o "pendrive").
 * @return Um código de status da enumeração StatusOperacao.
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

    // --- LÓGICA TEMPORÁRIA PARA FORÇAR A FALHA ---
    // Verifica se o arquivo de destino já existe para não sobrescrevê-lo.
    std::ifstream arquivo_existente(dest_path);
    if (arquivo_existente.is_open()) {
      arquivo_existente.close();
      continue;  // Pula para o próximo arquivo se o destino já existe.
    }
    // --- FIM DA LÓGICA TEMPORÁRIA ---

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