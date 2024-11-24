#ifndef VALIDADTOR_HEADER
#define VALIDADTOR_HEADER

#include <stdbool.h>

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"

bool validateStructures(Structure *structure);
void initializeValidatorModule();
void shutdownValidatorModule();

#endif
