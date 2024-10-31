#include "Validator.h"

#define TOTAL_STRUCTURES 8

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

typedef bool (*structureValidators)(const Structure *);
static structureValidators validators[TOTAL_STRUCTURES];

void initializeValidatorModule()
{
    _logger = createLogger("Calculator");

    validators[STRUCTURE_TREE] = _validateTree;
}

void shutdownValidatorModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }
}

/* PRIVATE FUNCTIONS */

static bool _validateStyleVariable(const char);

static bool _validateStyles(const Styles *styles)
{
    if (styles == NULL)
    {
        return true;
    }

    Styles *current = styles;
    while (current)
    {
        if (styles->property == '$' && !_validateStyleVariable(styles->rule))
        {
            logError(_logger, "Invalid style variable");
            return false;
        }

        current = current->next;
    }

    return _validateStyles(styles->next);
}

static bool _validateTreeCell(const Cells *cell)
{
    if (!cell)
    {
        return true;
    }

    if (!cell->value->type != CELL_FINAL)
    {
        logError(_logger, "First cell value is not final");
        return false;
    }

    bool valid = true;
    Cells *brother = cell->next;
    while (brother && valid)
    {
        if (brother->value->type != CELL_FINAL)
        {
            valid = _validateTreeCell(brother->value->cells);
        }

        brother = brother->next;
    }

    return valid;
}

static bool _validateTree(const Structure *tree)
{
    if (tree == NULL)
    {
        return true;
    }

    if (tree->cells == NULL)
    {
        logError(_logger, "Tree has no cells");
        return false;
    }

    return _validateTreeCell(tree->cells);
}

/* PUBLIC FUNCTIONS */

bool validateStructures(const Structure *structure)
{
    if (structure == NULL)
    {
        return true;
    }

    bool result = validators[structure->type](structure);

    if (!result)
    {
        logError(_logger, "Validation failed");
        return false;
    }

    return validateStructures(structure->next);
}
