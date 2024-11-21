#include "Validator.h"

#define TOTAL_STRUCTURES 8
// A variable name can only have lower or upper case letters, numbers and -
// 26 + 26 + 10 + 1 = 62
#define HASH_BASE 62
// The number of the name chars to be hashed
#define HASH_LENGTH 3
// HASH_BASE (63) ^ HASH_LENGTH (3)
#define HASH_SIZE 250047

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

static bool _validateTree(const Structure *tree);
static int _getVariableHash(const char *name);
static const StyleVariable *_getStyleVariableByReference(const char *name);
static bool _addStyleVariableToHash(const StyleVariable *variable);
static bool _validateStyleVariableReference(const char *reference);
static bool _validateStyles(const Styles *styles);
static bool _validateStyleVariables(const StyleVariable *variables);
static bool _validateTreeCell(const Cells *cell);
static bool _validateTree(const Structure *tree);



typedef bool (*structureValidators)(const Structure *);
static structureValidators validators[TOTAL_STRUCTURES];

struct VariableHashEntry
{
    const StyleVariable *value;
    struct VariableHashEntry *next;
};
static struct VariableHashEntry _variables[HASH_SIZE];

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

    for (int i = 0; i < HASH_SIZE; i++)
    {
        struct VariableHashEntry *entry = &_variables[i];
        while (entry)
        {
            struct VariableHashEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
}

/* PRIVATE FUNCTIONS */

/**
 * @brief Calculates the hash of a variable name
 * @note If the name is shorter than HASH_LENGTH, the algorithm will still work
 *
 * @param name
 * @return int
 */
static int _getVariableHash(const char *name)
{
    int hash = 0;
    for (int i = 0; i < HASH_LENGTH && name[i] != '\0'; i++)
    {
        int value = 0;
        if (islower(name[i]))
        {
            value = name[i] - 'a';
        }
        else if (isupper(name[i]))
        {
            value = name[i] - 'A' + 26;
        }
        else if (isdigit(name[i]))
        {
            value = name[i] - '0' + 52;
        }
        else if (name[i] == '-')
        {
            value = 62;
        }

        hash = hash * HASH_BASE + value;
    }

    return hash;
}

/**
 * @brief Retrieves a style variable by its reference in the hash table
 *
 * @param name The name of the variable
 * @return StyleVariable* The variable if defined, NULL otherwise
 */
static const StyleVariable *_getStyleVariableByReference(const char *name)
{
    int hash = _getVariableHash(name);
    struct VariableHashEntry *entry = &_variables[hash];

    while (entry)
    {
        int diff = strcmp(entry->value->name, name);

        if (!diff)
        {
            return entry->value;
        }

        if (diff > 0)
        {
            return NULL;
        }

        entry = entry->next;
    }

    return NULL;
}

static bool _addStyleVariableToHash(const StyleVariable *variable)
{
    int hash = _getVariableHash(variable->name);
    struct VariableHashEntry *entry = &_variables[hash];

    int diff;
    while ((diff = strcmp(variable->name, entry->value->name)) < 0)
    {
        entry = entry->next;
    }

    if (diff == 0)
    {
        logError(_logger, "Style variable already defined");
        return false;
    }

    struct VariableHashEntry *newEntry = malloc(sizeof(struct VariableHashEntry));
    newEntry->value = variable;
    newEntry->next = NULL;
    entry->next = newEntry;

    return true;
}

static bool _validateVariables(const StyleVariable *variables)
{
    if (variables == NULL)
    {
        return true;
    }

    if (!_validateStyleVariableReference(variables->name))
    {
        logError(_logger, "Invalid style variable reference");
        return false;
    }

    return _validateVariables(variables->next);
}

static bool _validateStyleVariableReference(const char *reference)
{
    const StyleVariable *variable = _getStyleVariableByReference(reference);
    if (variable == NULL)
    {
        logError(_logger, "Style variable not defined");
    }

    return variable != NULL;
}

static bool _validateStyles(const Styles *styles)
{
    if (styles == NULL)
    {
        return true;
    }

    const Styles *current = styles;
    while (current)
    {
        if (*styles->property == '$' && !_validateStyleVariableReference(styles->rule))
        {
            logError(_logger, "Invalid style variable");
            return false;
        }

        current = current->next;
    }

    return _validateStyles(styles->next);
}

static bool _validateStyleVariables(const StyleVariable *variables)
{
    if (variables == NULL)
    {
        return true;
    }

    if (!_validateStyles(variables->styles))
    {
        logError(_logger, "Invalid styles");
        return false;
    }

    if (!_addStyleVariableToHash(variables))
    {
        logError(_logger, "Duplicated style variable");
        return false;
    }

    return _validateStyleVariables(variables->next);
}

static bool _validateTreeCell(const Cells *cell)
{
    if (!cell)
    {
        return true;
    }

    if (cell->value->type != CELL_FINAL)
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
