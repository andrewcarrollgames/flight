// Copyright (c) 2025 Andrew Carroll Games, LLC
// Macro generator - parses extension API headers and generates plugin_macros_generated.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #define PATH_SEP '\\'
#else
    #include <dirent.h>
    #include <unistd.h>
    #define PATH_SEP '/'
#endif

#define MAX_LINE 1024
#define MAX_FUNCTIONS 256
#define MAX_NAME 128

typedef struct {
    char name[MAX_NAME];           // Function name: "LogHello"
    char return_type[MAX_NAME];    // Return type: "void", "bool", etc.
    char params[MAX_LINE];         // Full parameter list
} FunctionInfo;

typedef struct {
    char api_name[MAX_NAME];       // "TestAPI"
    char ext_name[MAX_NAME];       // "Test"
    FunctionInfo functions[MAX_FUNCTIONS];
    int function_count;
} ExtensionInfo;

// Convert CamelCase to SNAKE_CASE
void camel_to_snake_upper(const char* input, char* output) {
    int j = 0;
    for (int i = 0; input[i]; i++) {
        if (isupper(input[i]) && i > 0 && islower(input[i-1])) {
            output[j++] = '_';
        }
        output[j++] = toupper(input[i]);
    }
    output[j] = '\0';
}

// Trim whitespace from string
void trim(char* str) {
    char* end;
    while(isspace(*str)) str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    *(end+1) = 0;
}

// Parse a function pointer line: "void (*DoSomething)(int x, float y);"
int parse_function_pointer(const char* line, FunctionInfo* func) {
    // Look for pattern: return_type (*FuncName)(params);
    const char* star = strstr(line, "(*");
    if (!star) return 0;

    // Extract return type (everything before "(*")
    int ret_len = star - line;
    strncpy(func->return_type, line, ret_len);
    func->return_type[ret_len] = '\0';
    trim(func->return_type);

    // Extract function name (between "(* and )")
    star += 2; // Skip "(*"
    const char* end_paren = strchr(star, ')');
    if (!end_paren) return 0;

    int name_len = end_paren - star;
    strncpy(func->name, star, name_len);
    func->name[name_len] = '\0';
    trim(func->name);

    // Extract parameters (between next "(" and ")")
    const char* param_start = strchr(end_paren, '(');
    if (!param_start) return 0;
    param_start++;

    const char* param_end = strrchr(param_start, ')');
    if (!param_end) return 0;

    int param_len = param_end - param_start;
    strncpy(func->params, param_start, param_len);
    func->params[param_len] = '\0';
    trim(func->params);

    return 1;
}

// Extract parameter names from parameter list for macro calls
// "int x, float y" -> "x, y"
void extract_param_names(const char* params, char* output) {
    if (strlen(params) == 0 || strcmp(params, "void") == 0) {
        output[0] = '\0';
        return;
    }

    char buffer[MAX_LINE];
    strcpy(buffer, params);

    char* result = malloc(MAX_LINE);
    result[0] = '\0';

    char* token = strtok(buffer, ",");
    int first = 1;

    while (token) {
        // Get last word (the parameter name)
        char* last_word = NULL;
        char* word = strtok(token, " \t*");
        while (word) {
            last_word = word;
            word = strtok(NULL, " \t*");
        }

        if (last_word) {
            if (!first) strcat(result, ", ");
            strcat(result, last_word);
            first = 0;
        }

        token = strtok(NULL, ",");
    }

    strcpy(output, result);
    free(result);
}

// Parse an API header file
int parse_api_header(const char* filepath, ExtensionInfo* ext) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open: %s\n", filepath);
        return 0;
    }

    char line[MAX_LINE];
    int in_struct = 0;
    ext->function_count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Look for: typedef struct XxxAPI {
        if (strstr(line, "typedef struct") && strstr(line, "API")) {
            char* api_start = strstr(line, "struct") + 6;
            while (isspace(*api_start)) api_start++;

            char* api_end = api_start;
            while (*api_end && !isspace(*api_end) && *api_end != '{') api_end++;

            int len = api_end - api_start;
            strncpy(ext->api_name, api_start, len);
            ext->api_name[len] = '\0';

            // Extract extension name (TestAPI -> Test)
            strcpy(ext->ext_name, ext->api_name);
            char* api_suffix = strstr(ext->ext_name, "API");
            if (api_suffix) *api_suffix = '\0';

            in_struct = 1;
            continue;
        }

        // Exit struct when we see closing brace
        if (in_struct && strchr(line, '}')) {
            break;
        }

        // Parse function pointers inside struct
        if (in_struct && strstr(line, "(*")) {
            FunctionInfo func;
            if (parse_function_pointer(line, &func)) {
                ext->functions[ext->function_count++] = func;
            }
        }
    }

    fclose(file);
    return ext->function_count > 0;
}

// Generate macros for one extension
void generate_extension_macros(FILE* out_hot, FILE* out_static, ExtensionInfo* ext) {
    fprintf(out_hot, "// %s Extension Macros\n", ext->ext_name);
    fprintf(out_static, "// %s Extension Macros\n", ext->ext_name);

    for (int i = 0; i < ext->function_count; i++) {
        FunctionInfo* func = &ext->functions[i];

        // Generate macro name: TEST_LOG_HELLO
        char func_upper[MAX_NAME];
        camel_to_snake_upper(func->name, func_upper);

        char ext_upper[MAX_NAME];
        camel_to_snake_upper(ext->ext_name, ext_upper);

        char macro_name[MAX_NAME];
        snprintf(macro_name, sizeof(macro_name), "%s_%s", ext_upper, func_upper);

        // Extract parameter names
        char param_names[MAX_LINE];
        extract_param_names(func->params, param_names);

        // Hot reload macro
        fprintf(out_hot, "#define %s(...) \\\n", macro_name);
        fprintf(out_hot, "    ((%s*)__engine_api()->GetExtensionAPI(\"%s\"))->%s(__VA_ARGS__)\n",
                ext->api_name, ext->ext_name, func->name);

        // Static macro
        fprintf(out_static, "#define %s %s_%s\n",
                macro_name, ext->ext_name, func->name);
    }

    fprintf(out_hot, "\n");
    fprintf(out_static, "\n");
}

// Platform-agnostic directory iteration
#ifdef _WIN32
int iterate_directory(const char* shared_dir, FILE* temp_hot, FILE* temp_static,
                      int* extension_count, int* total_functions) {
    WIN32_FIND_DATAA find_data;
    char search_path[1024];
    snprintf(search_path, sizeof(search_path), "%s\\*_api.h", shared_dir);

    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open directory: %s\n", shared_dir);
        return 0;
    }

    do {
        const char* name = find_data.cFileName;
        size_t len = strlen(name);

        // Skip non-API headers
        if (len < 6 || strcmp(name + len - 6, "_api.h") != 0) continue;

        // Skip platform/engine/plugin APIs
        if (strcmp(name, "platform_api.h") == 0 ||
            strcmp(name, "engine_api.h") == 0 ||
            strcmp(name, "plugin_api.h") == 0) {
            continue;
        }

        // Build full path
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s\\%s", shared_dir, name);

        // Parse the header
        ExtensionInfo ext;
        if (parse_api_header(filepath, &ext)) {
            printf("  Parsed %s: %d functions\n", name, ext.function_count);
            generate_extension_macros(temp_hot, temp_static, &ext);
            (*extension_count)++;
            (*total_functions) += ext.function_count;
        }
    } while (FindNextFileA(hFind, &find_data));

    FindClose(hFind);
    return 1;
}
#else
int iterate_directory(const char* shared_dir, FILE* temp_hot, FILE* temp_static,
                      int* extension_count, int* total_functions) {
    DIR* dir = opendir(shared_dir);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", shared_dir);
        return 0;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        const char* name = entry->d_name;
        size_t len = strlen(name);

        // Skip non-API headers
        if (len < 6 || strcmp(name + len - 6, "_api.h") != 0) continue;

        // Skip platform/engine/plugin APIs
        if (strcmp(name, "platform_api.h") == 0 ||
            strcmp(name, "engine_api.h") == 0 ||
            strcmp(name, "plugin_api.h") == 0) {
            continue;
        }

        // Build full path
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", shared_dir, name);

        // Parse the header
        ExtensionInfo ext;
        if (parse_api_header(filepath, &ext)) {
            printf("  Parsed %s: %d functions\n", name, ext.function_count);
            generate_extension_macros(temp_hot, temp_static, &ext);
            (*extension_count)++;
            (*total_functions) += ext.function_count;
        }
    }

    closedir(dir);
    return 1;
}
#endif

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: macro_gen <shared_include_dir> <output_file>\n");
        return 1;
    }

    const char* shared_dir = argv[1];
    const char* output_file = argv[2];

    // Create temporary file names instead of using tmpfile()
    char temp_hot_name[256];
    char temp_static_name[256];

    snprintf(temp_hot_name, sizeof(temp_hot_name), "%s_hot_temp.txt", output_file);
    snprintf(temp_static_name, sizeof(temp_static_name), "%s_static_temp.txt", output_file);

    FILE* temp_hot = fopen(temp_hot_name, "w+");
    FILE* temp_static = fopen(temp_static_name, "w+");

    if (!temp_hot || !temp_static) {
        fprintf(stderr, "Failed to create temp files\n");
        return 1;
    }

    int extension_count = 0;
    int total_functions = 0;

    // Platform-specific directory iteration
    if (!iterate_directory(shared_dir, temp_hot, temp_static,
                          &extension_count, &total_functions)) {
        fclose(temp_hot);
        fclose(temp_static);
        remove(temp_hot_name);
        remove(temp_static_name);
        return 1;
    }

    // Write final output file
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Failed to open output file: %s\n", output_file);
        fclose(temp_hot);
        fclose(temp_static);
        remove(temp_hot_name);
        remove(temp_static_name);
        return 1;
    }

    fprintf(out, "// Auto-generated - do not edit\n");
    fprintf(out, "// Generated from extension API headers\n");
    fprintf(out, "// Extensions: %d, Functions: %d\n\n", extension_count, total_functions);
    fprintf(out, "#ifndef PLUGIN_MACROS_GENERATED_H\n");
    fprintf(out, "#define PLUGIN_MACROS_GENERATED_H\n\n");
    fprintf(out, "#ifdef ENABLE_GAME_AS_PLUGIN\n\n");

    // Copy hot-reload macros
    rewind(temp_hot);
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), temp_hot)) {
        fputs(buffer, out);
    }

    fprintf(out, "#else // Static build\n\n");

    // Copy static macros
    rewind(temp_static);
    while (fgets(buffer, sizeof(buffer), temp_static)) {
        fputs(buffer, out);
    }

    fprintf(out, "#endif // ENABLE_GAME_AS_PLUGIN\n\n");
    fprintf(out, "#endif // PLUGIN_MACROS_GENERATED_H\n");

    fclose(out);
    fclose(temp_hot);
    fclose(temp_static);

    // Clean up temp files
    remove(temp_hot_name);
    remove(temp_static_name);

    printf("Generated %s\n", output_file);
    printf("  %d extensions, %d total functions\n", extension_count, total_functions);

    return 0;
}