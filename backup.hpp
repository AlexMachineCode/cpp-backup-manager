// Copyright 2025 Alex Batista Resende
#ifndef BACKUP_HPP_
#define BACKUP_HPP_

#include <string>

// Enum para os códigos de status da operação
enum StatusOperacao {
  OPERACAO_SUCESSO,
  ERRO_BACKUP_PARM_NAO_EXISTE
  ERRO_DESTINO_MAIS_NOVO
};

// Apenas a DECLARAÇÃO da função. O corpo dela vai no .cpp
int realizaBackup(const std::string& destino_path);

#endif  // BACKUP_HPP_