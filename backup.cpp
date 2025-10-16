// Copyright 2025 Alex Batista Resende
#include "backup.hpp"  // NOLINT

#include <string>
#include <fstream>
#include <cassert>

/******************************************************************************
 * @brief Função: realizaBackup
 * @details Inicia o processo de backup, copiando arquivos listados em
 * "Backup.parm" para o diretório de destino.
 * @param destino_path O caminho para o diretório de destino (o "pendrive").
 * @return Um código de status da enumeração StatusOperacao.
 ******************************************************************************/
int realizaBackup(const std::string& destino_path) {
  std::ifstream param_file("Backup.parm");

  if (!param_file.is_open()) {
    return ERRO_BACKUP_PARM_NAO_EXISTE;
  }

  // --- LÓGICA NOVA PARA FAZER O TESTE PASSAR ---
  std::string nome_arquivo_a_copiar;
  // Lê a primeira linha do Backup.parm
  if (param_file >> nome_arquivo_a_copiar) {
    // Monta os caminhos de origem e destino
    std::string source_path = nome_arquivo_a_copiar;
    std::string dest_path = destino_path + "/" + nome_arquivo_a_copiar;

    // Abre os arquivos para leitura (origem) e escrita (destino)
    std::ifstream src(source_path, std::ios::binary);
    std::ofstream dst(dest_path, std::ios::binary);

    // Copia o conteúdo do arquivo de origem para o de destino
    dst << src.rdbuf();

    src.close();
    dst.close();
  }

  param_file.close();

  return OPERACAO_SUCESSO;
}