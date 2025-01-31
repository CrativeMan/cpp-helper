#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void toUpperString(char *string) {
  char *s = string;
  while (*s) {
    *s = toupper((unsigned char)*s);
    s++;
  }
}

int generateCppFile(int argc, char **argv) {
  char filename[128];
  char *name = argv[2];
  char *extension = ".cpp";

  // create uppercase name
  char *upperName = malloc(strlen(name) + 1);
  strcpy(upperName, name);
  toUpperString(upperName);

  // create filename plus extension
  if (sizeof(filename) < strlen(name) + 1) {
    fprintf(stderr, "Name '%s' is to long\n", name);
    return 1;
  }
  strncpy(filename, name, sizeof(filename));

  if (sizeof(filename) < (strlen(filename) + strlen(extension) + 1)) {
    fprintf(stderr, "Final size of filename to long\n");
    return 1;
  }
  strncat(filename, extension, (sizeof(filename) - strlen(filename) - 1));
  printf("Filename for main cpp file is %s\n", filename);

  // generate file
  FILE *file;
  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Failed to create main cpp file for '%s'\n", filename);
    free(upperName);
    return 1;
  }

  // write header odr violation guards
  fprintf(file, "#include \"%s.hpp\"\n", name);
  fprintf(file, "\n%s::%s(", name, name);
  int i;
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      fprintf(file, "%s", argv[i]);
    } else {
      fprintf(file, i == argc - 1 ? " %s" : " %s, ", argv[i]);
    }
  }
  fprintf(file, "){\n");
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      continue;
    } else {
      fprintf(file, "\tthis.%s = %s;\n", argv[i], argv[i]);
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
  if (sizeof(filename) < strlen(name) + 1) {
    fprintf(stderr, "Name '%s' is to long\n", name);
    return 1;
  }
  strncpy(filename, name, sizeof(filename));

  if (sizeof(filename) < (strlen(filename) + strlen(extension) + 1)) {
    fprintf(stderr, "Final size of filename to long\n");
    return 1;
  }
  strncat(filename, extension, (sizeof(filename) - strlen(filename) - 1));
  printf("Filename for header is %s\n", filename);

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
  fprintf(file, "\n\nclass %s {\npublic:\n", name);
  fprintf(file, "\t%s();\n", name);
  fprintf(file, "private:\n");

  int i;
  for (i = 3; i < argc; i++) {
    if (i % 2) {
      fprintf(file, "\t%s", argv[i]);
    } else {
      fprintf(file, " %s;\n", argv[i]);
    }
  }

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
    }
  }
}

int main(int argc, char **argv) {
  parseCommandLineArgs(argc, argv);
  return 0;
}
