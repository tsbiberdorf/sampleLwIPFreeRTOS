/**
 * @file Cli.h
 *
 * Public API Called to Interact with the Main Cli command and parse parameter values.
 * @brief Public API for interacting with the CLI
 * @see Cli.c for full implementation
 * @ingroup groupCliCmds
 */

#ifndef UARTTASKS_CLI_CLI_H_
#define UARTTASKS_CLI_CLI_H_

/**
 * @brief set up options for a CLI Command
 * @details this is related to an option of a specific cli command ie "-p"
 */
typedef struct _cliCommandOptions_s
{
	const char *option; /**< text of the option, include '-' */
	const uint8_t optionSize; /**< number of characters in the option text including the '-' character */
	int32_t (*optionCallback)(char *); /**< callback method to call if the option is detected */
}s_cliCommandOptions_t;

/**
 * @brief set up Main pointer for a CLI Command
 */
typedef struct _cliCommands_s
{
	const char *cmdString; /**< text of the command */
	s_cliCommandOptions_t *option; /**< pointer to all of the different options available for the command*/
}s_cliCommands_t;


uint8_t * ParseDecimal(uint8_t *InputStr, uint32_t *PtrDecimal);
uint8_t * ParseHex(uint8_t *InputStr, uint32_t *PtrHex);

void cli(uint8_t InputKey);
void replyOK();
void replyERROR();

#endif /* UARTTASKS_CLI_CLI_H_ */

