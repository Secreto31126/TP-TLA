#ifndef VALIDADTOR_HEADER
#define VALIDADTOR_HEADER

#include <stdbool.h>

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"

bool validateStructures(Structure *structure);
void initializeValidatorModule();
void shutdownValidatorModule();
/**
 * @brief Retrieves a style variable by its reference in the hash table
 *
 * @param name The name of the variable
 * @return StyleVariable* The variable if defined, NULL otherwise
 */
const StyleVariable *getStyleVariableByReference(const char *name);

#endif
