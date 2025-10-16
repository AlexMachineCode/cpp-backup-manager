#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "backup.hpp" 

#include <cstdio>

TEST_CASE("Backup falha quando Backup.parm nao existe", "[erros]") {
  
  remove("Backup.parm");

  // AÇÃO E VERIFICAÇÃO: Chama a função e verifica se o retorno é o erro esperado.
  REQUIRE(realizaBackup() == ERRO_BACKUP_PARM_NAO_EXISTE);
}