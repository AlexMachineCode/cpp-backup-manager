// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <fstream>  // Necessário para manipulação de arquivos
#include <cassert>  // Necessário para usar assert()

/******************************************************************************
 * @brief Função: realizaBackup
 *
 * @details
 * Inicia o processo de backup. A lógica é guiada pela Tabela de Decisão
 * dos slides da aula. Esta implementação inicial cobre a primeira coluna da
 * tabela: o que acontece se "Backup.parm" não existir.
 *
 * @return
 * Retorna um código de status da enumeração StatusOperacao.
 * - ERRO_BACKUP_PARM_NAO_EXISTE: Se o arquivo "Backup.parm" não for encontrado.
 * - OPERACAO_SUCESSO: Nos outros casos (lógica futura).
 *
 * @assertiva-saida
 * - Se a função retornar ERRO_BACKUP_PARM_NAO_EXISTE, o sistema de arquivos
 * não foi modificado pela operação.
 ******************************************************************************/
int realizaBackup() {
  std::ifstream param_file("Backup.parm");

  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  // Assertiva de saída implícita: se chegamos aqui, o arquivo deve estar aberto.
  assert(param_file.is_open());

  param_file.close();  // Boa prática: fechar o arquivo quando terminar de usá-lo.

  return OPERACAO_SUCESSO;
}