// Copyright (c) 2025 Andrew Carroll Games, LLC
// API header generator - scans extension source files and generates API headers

// At the top, add platform-specific includes
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 2048
#define MAX_FUNCTIONS 512
#define MAX_NAME 256

    typedef struct {
  char return_type[MAX_NAME];
  char function_name[MAX_NAME];       // Full: "Test_LogHello"
  char prefix[MAX_NAME];              // Extracted: "Test"
  char name_without_prefix[MAX_NAME]; // Stripped: "LogHello"
  char full_signature[MAX_LINE];
  char params[MAX_LINE];
  int line_number;
} FunctionDecl;

typedef struct {
  char extension_name[MAX_NAME]; // "Test", "SDL", etc.
  FunctionDecl functions[MAX_FUNCTIONS];
  int function_count;
} ExtensionAPI;

// Trim whitespace
void trim(char *str) {
  char *end;
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  *(end + 1) = 0;
}

// Parse a function declaration
// Input: "EXTENSION_API void Test_LogHello(void) {"
// Extract: return_type="void", name="Test_LogHello", prefix="Test", name_without_prefix="LogHello"
int parse_function_decl(const char *line, FunctionDecl *func) {
  char buffer[MAX_LINE];
  strncpy(buffer, line, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  // Remove EXTENSION_API marker
  char *start = strstr(buffer, "EXTENSION_API");
  if (!start)
    return 0;
  start += 13; // Skip "EXTENSION_API"

  while (isspace(*start))
    start++;

  // Find opening parenthesis
  char *paren = strchr(start, '(');
  if (!paren)
    return 0;

  // Extract everything before '(' as "return_type function_name"
  *paren = '\0';
  char *name_start = start;

  // Find the function name (last word before '(')
  // Walk backwards to find last space or asterisk
  char *last_space = strrchr(name_start, ' ');
  char *last_star = strrchr(name_start, '*');

  // Use whichever comes last
  char *split = last_space;
  if (last_star && (!split || last_star > split)) {
    split = last_star;
  }

  if (!split) {
    // No space/star found, entire thing is function name
    strcpy(func->function_name, name_start);
    strcpy(func->return_type, "void");
  } else {
    // Split into return type and function name
    *split = '\0';
    strcpy(func->return_type, name_start);
    strcpy(func->function_name, split + 1);
  }

  trim(func->return_type);
  trim(func->function_name);

  // Extract prefix from function name (everything before first underscore)
  char *underscore = strchr(func->function_name, '_');
  if (underscore) {
    size_t prefix_len = underscore - func->function_name;
    strncpy(func->prefix, func->function_name, prefix_len);
    func->prefix[prefix_len] = '\0';

    // Store name without prefix for API struct
    strcpy(func->name_without_prefix, underscore + 1);
  } else {
    // No underscore - error, but we'll catch it later
    strcpy(func->prefix, func->function_name);
    strcpy(func->name_without_prefix, func->function_name);
  }

  // Extract parameters (between '(' and ')' or '{')
  paren++; // Skip '('
  char *end_paren = strchr(paren, ')');
  if (!end_paren)
    return 0;

  *end_paren = '\0';
  strcpy(func->params, paren);
  trim(func->params);

  // If params is empty, use "void"
  if (func->params[0] == '\0') {
    strcpy(func->params, "void");
  }

  // Build full signature for comments
  snprintf(func->full_signature, sizeof(func->full_signature),
           "%s %s(%s)",
           func->return_type, func->function_name, func->params);

  return 1;
}

// Scan a source file for EXTENSION_API functions
int scan_source_file(const char *filepath, ExtensionAPI *api, const char *filename) {
  FILE *file = fopen(filepath, "r");
  if (!file) {
    fprintf(stderr, "Warning: Could not open %s\n", filepath);
    return 0;
  }

  char line[MAX_LINE];
  int line_num = 0;
  int found = 0;

  while (fgets(line, sizeof(line), file)) {
    line_num++;

    // Only look for EXTENSION_API marker
    if (!strstr(line, "EXTENSION_API"))
      continue;

    FunctionDecl func;
    func.line_number = line_num;

    if (parse_function_decl(line, &func)) {
      // First function sets the extension name
      if (api->extension_name[0] == '\0') {
        strcpy(api->extension_name, func.prefix);
        printf("  Detected extension name: '%s' from %s\n",
               api->extension_name, func.function_name);
      }

      // Validate prefix matches extension name
      if (strcmp(func.prefix, api->extension_name) != 0) {
        fprintf(stderr, "\n");
        fprintf(stderr, "========================================\n");
        fprintf(stderr, "ERROR: Function prefix mismatch\n");
        fprintf(stderr, "========================================\n");
        fprintf(stderr, "File: %s\n", filepath);
        fprintf(stderr, "Line: %d\n", line_num);
        fprintf(stderr, "Function: %s\n", func.function_name);
        fprintf(stderr, "\n");
        fprintf(stderr, "Expected prefix: '%s_'\n", api->extension_name);
        fprintf(stderr, "Found prefix:    '%s_'\n", func.prefix);
        fprintf(stderr, "\n");
        fprintf(stderr, "All EXTENSION_API functions in an extension must\n");
        fprintf(stderr, "use the same prefix (the extension name).\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "First function was: %s\n", api->functions[0].function_name);
        fprintf(stderr, "Which established prefix: '%s_'\n", api->extension_name);
        fprintf(stderr, "========================================\n");
        fclose(file);
        exit(1);
      }

      // Check for underscore in function name
      if (!strchr(func.function_name, '_')) {
        fprintf(stderr, "\n");
        fprintf(stderr, "========================================\n");
        fprintf(stderr, "ERROR: Invalid function name format\n");
        fprintf(stderr, "========================================\n");
        fprintf(stderr, "File: %s\n", filepath);
        fprintf(stderr, "Line: %d\n", line_num);
        fprintf(stderr, "Function: %s\n", func.function_name);
        fprintf(stderr, "\n");
        fprintf(stderr, "EXTENSION_API functions must follow the pattern:\n");
        fprintf(stderr, "  ExtensionName_FunctionName\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "Example: Test_LogHello, SDL_CreateWindow\n");
        fprintf(stderr, "========================================\n");
        fclose(file);
        exit(1);
      }

      if (api->function_count < MAX_FUNCTIONS) {
        api->functions[api->function_count++] = func;
        found++;
      } else {
        fprintf(stderr, "Warning: Maximum functions (%d) reached in %s\n",
                MAX_FUNCTIONS, filename);
      }
    }
  }

  fclose(file);
  return found;
}

// Generate the API header file
void generate_api_header(ExtensionAPI *api, const char *output_path) {
  FILE *out = fopen(output_path, "w");
  if (!out) {
    fprintf(stderr, "Error: Could not create %s\n", output_path);
    return;
  }

  char api_name[MAX_NAME];
  snprintf(api_name, sizeof(api_name), "%sAPI", api->extension_name);

  char guard[MAX_NAME];
  snprintf(guard, sizeof(guard), "%s_EXTENSION_API_H", api->extension_name);
  // Convert to uppercase
  for (int i = 0; guard[i]; i++) {
    guard[i] = toupper((unsigned char)guard[i]);
  }

  // Write header
  fprintf(out, "// Auto-generated API header for %s extension\n", api->extension_name);
  fprintf(out, "// Do not edit manually - regenerated from source files\n");
  fprintf(out, "//\n");
  fprintf(out, "// Generated from %d functions:\n", api->function_count);
  for (int i = 0; i < api->function_count && i < 5; i++) {
    fprintf(out, "//   - %s\n", api->functions[i].function_name);
  }
  if (api->function_count > 5) {
    fprintf(out, "//   ... and %d more\n", api->function_count - 5);
  }
  fprintf(out, "\n");

  fprintf(out, "#ifndef %s\n", guard);
  fprintf(out, "#define %s\n\n", guard);

  fprintf(out, "#include <stdbool.h>\n");
  fprintf(out, "#include <stdint.h>\n\n");

  fprintf(out, "#ifdef __cplusplus\n");
  fprintf(out, "extern \"C\" {\n");
  fprintf(out, "#endif\n\n");

  // Forward declarations for types used by this extension
  fprintf(out, "// Forward declarations\n");
  fprintf(out, "typedef struct EngineAPI EngineAPI;\n");
  fprintf(out, "typedef struct PlatformAPI PlatformAPI;\n\n");

  // Export macro
  fprintf(out, "// Export macro for extension functions\n");
  fprintf(out, "#ifdef _WIN32\n");
  fprintf(out, "  #define EXTENSION_API __declspec(dllexport)\n");
  fprintf(out, "#else\n");
  fprintf(out, "  #define EXTENSION_API __attribute__((visibility(\"default\")))\n");
  fprintf(out, "#endif\n\n");

  // API struct typedef
  fprintf(out, "// %s extension API\n", api->extension_name);
  fprintf(out, "typedef struct %s {\n", api_name);

  for (int i = 0; i < api->function_count; i++) {
    FunctionDecl *func = &(api->functions[i]);
    fprintf(out, "    %s (*%s)(%s);\n",
            func->return_type,
            func->name_without_prefix,
            func->params);
  }

  fprintf(out, "} %s;\n\n", api_name);

  // Forward declarations for static builds
  fprintf(out, "// Forward declarations for static builds\n");
  for (int i = 0; i < api->function_count; i++) {
    FunctionDecl *func = &(api->functions[i]);
    fprintf(out, "EXTENSION_API %s %s(%s);\n",
            func->return_type,
            func->function_name,
            func->params);
  }

  fprintf(out, "\n#ifdef __cplusplus\n");
  fprintf(out, "}\n");
  fprintf(out, "#endif\n\n");
  fprintf(out, "#endif // %s\n", guard);

  fclose(out);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: api_gen <source_dir> <output_file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Scans <source_dir> for .c files containing EXTENSION_API functions\n");
    fprintf(stderr, "and generates an API header at <output_file>.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "  api_gen extensions/test shared/include/test_extension_api.h\n");
    fprintf(stderr, "\n");
    return 1;
  }

  const char *source_dir = argv[1];
  const char *output_file = argv[2];

  //ExtensionAPI api = {0};
  ExtensionAPI *api = calloc(1, sizeof(ExtensionAPI));
  if (!api) {
    fprintf(stderr, "Error: Could not allocate memory for API structure\n");
    return 1;
  }

  printf("Scanning %s for EXTENSION_API functions...\n", source_dir);

  int total_files = 0;

#ifdef _WIN32
  // Windows implementation using FindFirstFile/FindNextFile
  WIN32_FIND_DATAA find_data;
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s\\*.c", source_dir);

  HANDLE hFind = FindFirstFileA(search_path, &find_data);
  if (hFind == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Error: Could not open directory: %s\n", source_dir);
    return 1;
  }

  do {
    // Skip directories
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      continue;
    }

    const char *name = find_data.cFileName;
    size_t len = strlen(name);

    // Only process .c files
    if (len < 3 || strcmp(name + len - 2, ".c") != 0)
      continue;

    // Build full path
    char filepath[MAX_PATH];
    snprintf(filepath, sizeof(filepath), "%s\\%s", source_dir, name);

    int found = scan_source_file(filepath, api, name);
    if (found > 0) {
      printf("  %s: %d functions\n", name, found);
      total_files++;
    }
  } while (FindNextFileA(hFind, &find_data) != 0);

  FindClose(hFind);

#else
  // Unix/Linux/macOS implementation using dirent.h
  DIR *dir = opendir(source_dir);
  if (!dir) {
    fprintf(stderr, "Error: Could not open directory: %s\n", source_dir);
    return 1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    const char *name = entry->d_name;
    size_t len = strlen(name);

    // Only process .c files
    if (len < 3 || strcmp(name + len - 2, ".c") != 0)
      continue;

    // Build full path
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", source_dir, name);

    int found = scan_source_file(filepath, api, name);
    if (found > 0) {
      printf("  %s: %d functions\n", name, found);
      total_files++;
    }
  }

  closedir(dir);
#endif

  if (api->function_count == 0) {
    fprintf(stderr, "\n");
    fprintf(stderr, "Warning: No EXTENSION_API functions found in %s\n", source_dir);
    fprintf(stderr, "\n");
    fprintf(stderr, "Make sure your functions are marked with EXTENSION_API:\n");
    fprintf(stderr, "  EXTENSION_API void MyExt_DoSomething(int param) { ... }\n");
    fprintf(stderr, "\n");
    return 1;
  }

  printf("\n");
  printf("Generating API header...\n");
  printf("  Extension: %s\n", api->extension_name);
  printf("  Functions: %d\n", api->function_count);
  printf("  Output:    %s\n", output_file);

  generate_api_header(api, output_file);
  free(api);
  printf("\nSuccess!\n");
  return 0;
}