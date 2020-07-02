/**
 * @file cliCmdsModbus.h
 * @brief Public implementation of API for interacting with modbus via cli
 * @see cliCmdsModbus.c for full implemenation
 * @ingroup groupCliCmds
 */

#ifndef UARTTASKS_CLI_CLICMDSMODBUS_H_
#define UARTTASKS_CLI_CLICMDSMODBUS_H_

//@todo figure out why I needed to include cli into this header
#include "Cli.h"
const char* MbCliInit(s_cliCommands_t *CmdOpt);

#endif /* UARTTASKS_CLI_CLICMDSMODBUS_H_ */
