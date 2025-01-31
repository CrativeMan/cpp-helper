#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printHelp() {
  printf("How to use:\n");
  printf("Generate class and header in this directory:\n");
  printf("\tHow to run:\n");
  printf(
      "\033[0;33m\t\tcmk class <name_of_class> <param_1_type> <param_1_name> "
      "<param_2_type> <param_2_name> <...>\n\033[0m");
  printf("\tEdge cases:\n");
  printf("\033[0;33m\t\tIf you want to pass in a special parameter like a "
         "pointer or a const variable\n");
  printf("\t\tput the param in \"\" like \"<param_*x_name>\" or \"const "
         "<param_x_name>\".\n\033[0m");
}

void toUpperString(char *string) {
  char *s = string;
  while (*s) {
    *s = toupper((unsigned char)*s);
    s++;
  }
}

int createFilename(char *dest, size_t destSize, const char *baseName,
                   const char *extension) {
  if (!dest || !baseName || !extension)
    return 1;

  size_t nameLen = strlen(baseName);
  size_t extLen = strlen(extension);

  if (destSize < (nameLen + extLen + 1))
    return 1;

  strncpy(dest, baseName, destSize);
  dest[destSize - 1] = '\0';
  strncat(dest, extension, destSize - strlen(dest) - 1);

  return 0;
}

int generateCppFile(int argc, char **argv) {
  char filename[128];
  char *name = argv[2];
  char *extension = ".cpp";

  // create uppercase name
  char *upperName = malloc(strlen(name) + 1);
  strcpy(upperName, name);
  toUpperString(upperName);

  if (createFilename(filename, sizeof(filename), name, extension) != 0) {
    fprintf(stderr, "Failed to generate cpp file name\n");
    return 1;
  }

  // generate file
  FILE *file;
  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Failed to create main cpp file for '%s'\n", filename);
    free(upperName);
    return 1;
  }

  // include
  fprintf(file, "#include \"%s.hpp\"\n", name);
  // constructor
  fprintf(file, "\n%s::%s(", name, name);
  // constructor variables
  int i;
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      fprintf(file, "%s", argv[i]);
    } else {
      fprintf(file, i == argc - 1 ? " %s" : " %s, ", argv[i]);
    }
  }
  // constructor asign private variables the parameters of the constructor
  fprintf(file, "){\n");
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      continue;
    } else {
      char *tmp = argv[i];
      if (tmp[0] == '*') { // check if param name starts with * for pointers and
                           // if so remove it
        memmove(tmp, tmp + 1, strlen(tmp));
      }
      fprintf(file, "\tthis->%s = %s;\n", argv[i], argv[i]);
    }
  }
  fprintf(file, "}");

  fclose(file);
  free(upperName);

  return 0;
}

int generateHeaderFile(int argc, char **argv) {
  char filename[128];
  char *name = argv[2];
  char *extension = ".hpp";

  // create uppercase name
  char *upperName = malloc(strlen(name) + 1);
  strcpy(upperName, name);
  toUpperString(upperName);

  // create filename plus extension
  if (createFilename(filename, sizeof(filename), name, extension) != 0) {
    fprintf(stderr, "Failed to generate hpp file name\n");
    return 1;
  }

  // generate file
  FILE *file;
  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Failed to create header file for '%s'\n", filename);
    free(upperName);
    return 1;
  }

  // write header odr violation guards
  fprintf(file, "#ifndef %s_HPP\n#define %s_HPP", upperName, upperName);
  // write class definition
  fprintf(file, "\n\nclass %s {\npublic:\n", name);
  fprintf(file, "\t%s(", name);
  int i;
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      fprintf(file, "%s", argv[i]);
    } else {
      fprintf(file, i == argc - 1 ? " %s" : " %s, ", argv[i]);
    }
  }
  fprintf(file, ");\nprivate:\n");

  // write private args
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      fprintf(file, "\t%s", argv[i]);
    } else {
      fprintf(file, " %s;\n", argv[i]);
    }
  }

  // end if of odr violation guard
  fprintf(file, "};\n\n#endif //%s_HPP", upperName);

  fclose(file);
  free(upperName);

  return 0;
}

void parseCommandLineArgs(int argc, char **argv) {
  if (argc > 1) {
    // format of command line argument is as followed:
    // mkc class <name_of_class> <para_1_type> <para_1_name> <para_2_type>
    // <para_2_name> <...>
    // ----------------------
    // check if input is valid for class generation
    // if argv1 is class the argc has to be odd so for every parameter there is
    // a type and a name for it
    if (!strcmp(argv[1], "class") && argc % 2) {
      generateHeaderFile(argc, argv);
      generateCppFile(argc, argv);
    } else {
      printHelp();
    }
  } else {
    printHelp();
  }
}

int main(int argc, char **argv) {
  parseCommandLineArgs(argc, argv);
  return 0;
}
