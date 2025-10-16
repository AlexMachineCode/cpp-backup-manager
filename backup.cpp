// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <fstream>  // Necessário para manipulação de arquivos (ifstream)
#include <cassert>  // Necessário para usar assert()

/**
 * @brief Executa a rotina de backup.
 * @details Verifica a existência do arquivo Backup.parm e, se existir,
 * prossegue com a lógica de backup.
 * @return OPERACAO_SUCESSO se o backup for concluído (ainda não implementado).
 * @return ERRO_BACKUP_PARM_NAO_EXISTE se o arquivo de parâmetros não for encontrado.
 */

int realizaBackup() {
  // Tenta abrir o arquivo de parâmetros para leitura.
  std::ifstream param_file("Backup.parm");

  // Se o arquivo não pôde ser aberto, ele não existe.
  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  // Se o arquivo existe, por enquanto retornamos sucesso.
  return OPERACAO_SUCESSO;
}