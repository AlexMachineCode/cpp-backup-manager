// Copyright 2025 Alex Batista Resende
#ifndef BACKUP_HPP_
#define BACKUP_HPP_

#include <string>
#include <ctime>  // Adicionado para o tipo time_t

// Enum para os códigos de status da operação
enum StatusOperacao {
  OPERACAO_SUCESSO,
  ERRO_BACKUP_PARM_NAO_EXISTE,
  ERRO_DESTINO_MAIS_NOVO,
  ERRO_ORIGEM_MAIS_ANTIGA,
  ERRO_ARQUIVO_ORIGEM_NAO_EXISTE,
  ERRO_SEM_PERMISSAO
};

// Declaração das funções
int realizaBackup(const std::string& destino_path);
int realizaRestauracao(const std::string& origem_path);
void registrarLog(const std::string& arquivo, const std::string& mensagem);

// Declaração da função auxiliar para que outros arquivos a conheçam
time_t getFileModTime(const std::string& path);

#endif  // BACKUP_HPP_