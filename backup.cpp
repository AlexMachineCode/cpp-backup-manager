// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <fstream>
#include <cassert>

/******************************************************************************
 * @brief Função: realizaBackup
 * @details Inicia o processo de backup.
 * @param destino_path O caminho para o diretório de destino (o "pendrive").
 * @return Um código de status da enumeração StatusOperacao.
 ******************************************************************************/
int realizaBackup(const std::string& destino_path) {
  std::ifstream param_file("Backup.parm");

  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  param_file.close();

  // Por enquanto, a função não faz nada com os arquivos.
  // Isso fará o novo teste falhar, como esperado.
  return OPERACAO_SUCESSO;
}