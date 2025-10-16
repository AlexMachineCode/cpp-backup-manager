// Copyright 2025 Alex Batista Resende
#ifndef BACKUP_HPP_
#define BACKUP_HPP_

// Enum para os códigos de status da operação
enum StatusOperacao {
  OPERACAO_SUCESSO,
  ERRO_BACKUP_PARM_NAO_EXISTE
};

// Apenas a DECLARAÇÃO da função. O corpo dela vai no .cpp
int realizaBackup();

#endif  // BACKUP_HPP_