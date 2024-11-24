#include "Validator.h"

#define TOTAL_STRUCTURES 8
// A variable name can only have lower or upper case letters, numbers and -
#define HASH_BASE 62
// The number of the name chars to be hashed
#define HASH_LENGTH 3
// HASH_BASE (63) ^ HASH_LENGTH (3)
#define HASH_SIZE 250047

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

/**
 * @brief Calculates the hash of a variable name
 * @note If the name is shorter than HASH_LENGTH, the algorithm will still work
 *
 * @param name The name of the variable
 * @return int The hash of the variable
 */
static int _getVariableHash(const char *name);
/**
 * @brief Inserts a style variable in the hash table
 *
 * @param variable The variable to be inserted
 * @return true The variable was inserted successfully
 * @return false The variable is already defined
 */
static bool _addStyleVariableToHash(const StyleVariable *variable);
/**
 * @brief Checks if a style variable reference exists in the hash table
 *
 * @param reference The reference to be checked
 * @return true The reference exists
 * @return false The reference does not exist
 */
static bool _validateStyleVariableReference(const char *reference);
/**
 * @brief Validates the content of a style variable or annotation
 *
 * @param styles The styles to be validated
 * @return true If the styles are valid
 * @return false If the styles are invalid
 */
static bool _validateStyles(const Styles *styles);
/**
 * @brief Validates the variables of a structure
 *
 * @param variables The variables to be validated
 * @return true If the variables are valid
 * @return false If the variables are invalid
 */
static bool _validateStyleVariables(const StyleVariable *variables);
/**
 * @brief Validates the content of an array structure
 *
 * @param array The structure to be validated
 * @return true The structure is valid
 * @return false The structure is invalid
 */
static bool _validateArray(const Structure *array);
/**
 * @brief Validates the cell of an array
 *
 * @param cell The cell to be validated
 * @return true The cell is valid
 * @return false The cell is invalid
 */
static bool _validateArrayCells(const Cells *cell);
/**
 * @brief Validates the cell of a tree
 *
 * @param cell The cell to be validated
 * @return true The cell is valid
 * @return false The cell is invalid
 */
static bool _validateTreeCell(const Cells *cell);
/**
 * @brief Validates the content of a tree structure
 *
 * @param tree The structure to be validated
 * @return true The structure is valid
 * @return false The structure is invalid
 */
static bool _validateTree(const Structure *tree);
/**
 * @brief Validates the cell of a list
 *
 * @param cell The cell to be validated
 * @return true The cell is valid
 * @return false The cell is invalid
 */
static bool _validateListCell(const Cells *cell);
/**
 * @brief Validates the content of a list structure
 *
 * @param list The structure to be validated
 * @return true The structure is valid
 * @return false The structure is invalid
 */
static bool _validateList(const Structure *list);
/**
 * @brief Validates the content of a graph structure
 *
 * @param graph The structure to be validated
 * @return true The structure is valid
 * @return false The structure is invalid
 */
static bool _validateGraph(const Structure *graph);
/**
 * @brief Validates the content of a table structure
 *
 * @param graph The structure to be validated
 * @return true The structure is valid
 * @return false The structure is invalid
 */
static bool _validateTable(const Structure *table);

typedef bool (*structureValidators)(const Structure *);
static structureValidators validators[TOTAL_STRUCTURES];

struct VariableHashEntry
{
    const StyleVariable *value;
    struct VariableHashEntry *next;
};
static struct VariableHashEntry *_variables[HASH_SIZE] = {NULL};

void initializeValidatorModule()
{
    _logger = createLogger("Validator");

    validators[STRUCTURE_TREE] = _validateTree;
    validators[STRUCTURE_ARRAY] = _validateArray;
    validators[STRUCTURE_LIST] = _validateList;
    validators[STRUCTURE_LINKED_LIST] = _validateList;
    validators[STRUCTURE_DOUBLE_LINKED_LIST] = _validateList;
    validators[STRUCTURE_GRAPH] = _validateGraph;
    validators[STRUCTURE_DIRECTED_GRAPH] = _validateGraph;
    validators[STRUCTURE_TABLE] = _validateTable;
}

void shutdownValidatorModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }

    for (int i = 0; i < HASH_SIZE; i++)
    {
        struct VariableHashEntry *entry = _variables[i];
        while (entry)
        {
            struct VariableHashEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
}

/* PRIVATE FUNCTIONS */

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
const StyleVariable *getStyleVariableByReference(const char *name)
{
    int hash = _getVariableHash(name);
    struct VariableHashEntry *entry = _variables[hash];

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
    struct VariableHashEntry *entry = _variables[hash];

    int diff;
    while (entry && (diff = strcmp(variable->name, entry->value->name)) < 0)
    {
        entry = entry->next;
    }

    if (entry && !diff)
    {
        logError(_logger, "Style variable already defined");
        return false;
    }

    struct VariableHashEntry *newEntry = malloc(sizeof(struct VariableHashEntry));
    newEntry->value = variable;
    newEntry->next = NULL;

    if (!entry)
    {
        _variables[hash] = newEntry;
    }
    else
    {
        entry->next = newEntry;
    }

    return true;
}

static bool _validateStyleVariableReference(const char *reference)
{
    const StyleVariable *variable = getStyleVariableByReference(reference);
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
        if (styles->property == PROPERTY_VARIABLE && !_validateStyleVariableReference(styles->rule))
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

static bool _validateAnnotations(const AnnotationList *annotations)
{
    if (annotations == NULL)
    {
        return true;
    }

    if (!_validateStyles(annotations->value->style))
    {
        logError(_logger, "Invalid annotation style");
        return false;
    }

    return _validateAnnotations(annotations->next);
}

static StructureLabels *_appendToLabels(StructureLabels *labels, const char *name)
{
    if (labels == NULL)
    {
        StructureLabels *newLabel = malloc(sizeof(StructureLabels));
        newLabel->name = name;
        newLabel->next = NULL;
        return newLabel;
    }

    int diff = strcmp(labels->name, name);
    if (diff == 0)
    {
        return labels;
    }

    if (diff > 0)
    {
        StructureLabels *newLabel = malloc(sizeof(StructureLabels));
        newLabel->name = name;
        newLabel->next = labels;
        return newLabel;
    }

    labels->next = _appendToLabels(labels->next, name);
    return labels;
}

static void _retrieveCellLabels(const Cells *cell, StructureLabels **left, StructureLabels **right)
{
    if (cell == NULL)
    {
        return;
    }

    if (!cell->value)
    {
        *right = _appendToLabels(*right, cell->label);
    }
    else
    {
        if (cell->label)
        {
            *left = _appendToLabels(*left, cell->label);
        }

        if (cell->value->type != CELL_FINAL)
        {
            _retrieveCellLabels(cell->value->cells, left, right);
        }
    }

    _retrieveCellLabels(cell->next, left, right);
}

static bool _freeStructureLabels(StructureLabels *labels)
{
    if (labels == NULL)
    {
        return true;
    }

    StructureLabels *next = labels->next;
    free(labels);
    return _freeStructureLabels(next);
}

static bool _validateLabels(Structure *structure)
{
    StructureLabels *left = NULL;
    StructureLabels *right = NULL;

    _retrieveCellLabels(structure->cells, &left, &right);

    // Validate all right labels are in the left
    StructureLabels *current_right = right;
    StructureLabels *current_left = left;

    while (current_right)
    {
        bool found = false;
        while (current_left && !found)
        {
            int diff = strcmp(current_right->name, current_left->name);

            if (!diff)
            {
                found = true;
            }

            if (diff < 0)
            {
                break;
            }

            current_left = current_left->next;
        }

        if (!found)
        {
            logError(_logger, "Label not found in the left set");
            return false;
        }

        current_right = current_right->next;
    }

    structure->labels = left;
    return true;
}

static bool _validateArrayCells(const Cells *cell)
{
    const Cells *current = cell;
    while (current)
    {
        if (!current->value || current->value->type != CELL_FINAL)
        {
            logError(_logger, "Array cell is not final");
            return false;
        }

        current = current->next;
    }

    return true;
}

static bool _validateArray(const Structure *array)
{
    if (array == NULL)
    {
        return true;
    }

    if (array->cells == NULL)
    {
        logError(_logger, "Array has no cells");
        return false;
    }

    return _validateArrayCells(array->cells);
}

static bool _validateTreeCell(const Cells *cell)
{
    if (!cell)
    {
        return true;
    }

    if (!cell->value || cell->value->type != CELL_FINAL)
    {
        logError(_logger, "First cell value is not final");
        return false;
    }

    bool valid = true;
    Cells *brother = cell->next;
    while (brother && valid)
    {
        if (!brother->value)
        {
            logError(_logger, "Brother cell has no value");
            return false;
        }

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

static bool _validateListCell(const Cells *cell)
{
    const Cells *current = cell;
    while (current)
    {
        if (!current->value || current->value->type != CELL_FINAL)
        {
            logError(_logger, "List cell is not final");
            return false;
        }

        current = current->next;
    }

    return true;
}

static bool _validateList(const Structure *list)
{
    if (list == NULL)
    {
        return true;
    }

    if (list->cells == NULL)
    {
        logError(_logger, "List has no cells");
        return false;
    }

    return _validateListCell(list->cells);
}

static bool _validateGraphCells(const Cells *cell, bool root)
{
    if (cell == NULL)
    {
        return true;
    }

    if (!root && (!cell->value || cell->value->type != CELL_FINAL))
    {
        logError(_logger, "Graph cell is not final");
        return false;
    }

    bool valid = true;
    const Cells *curr = cell;
    while (curr && valid)
    {
        if (curr->value && curr->value->type != CELL_FINAL)
        {
            valid = _validateGraphCells(curr->value->cells, false);
        }

        curr = curr->next;
    }

    return valid;
}

static bool _validateGraph(const Structure *graph)
{
    if (graph == NULL)
    {
        return true;
    }

    if (graph->cells == NULL)
    {
        logError(_logger, "Graph has no cells");
        return false;
    }

    return _validateGraphCells(graph->cells, true);
}

static bool _validateTableCells(const Cells *cell)
{
    if (cell == NULL)
    {
        return true;
    }

    const Cells *outer = cell;
    while (outer)
    {
        if (!outer->value || outer->value->type == CELL_FINAL)
        {
            logError(_logger, "Table outer cell is final");
            return false;
        }

        const Cells *inner = outer->value->cells;
        while (inner)
        {
            if (!inner->value || inner->value->type != CELL_FINAL)
            {
                logError(_logger, "Table inner cell is not final");
                return false;
            }

            inner = inner->next;
        }

        outer = outer->next;
    }

    return true;
}

static bool _validateTable(const Structure *table)
{
    if (table == NULL)
    {
        return true;
    }

    if (table->cells == NULL)
    {
        logError(_logger, "Table has no cells");
        return false;
    }

    return _validateTableCells(table->cells);
}

/* PUBLIC FUNCTIONS */

bool validateStructures(Structure *structure)
{
    if (structure == NULL)
    {
        return true;
    }

    bool result;
    result = _validateStyleVariables(structure->variables);
    result = result && _validateAnnotations(structure->annotations);
    result = result && _validateLabels(structure);
    result = result && validators[structure->type](structure);

    if (!result)
    {
        logError(_logger, "Validation failed");
        return false;
    }

    return validateStructures(structure->next);
}
