/*
  Modes:
    1. Generate mode:
      Given a directory structure (_input_file) where each directory
      represents a project which is either a library or an executable,
      and a working directory (_working_dir) this program generates a
      root makefile and a makefile for each project.

    2. Distribute mode:
      Given a directory structure (_input_file) and location of this
      source file (_own_source) and the root project (_root_project)
      and the output directory (_output_dir), and the working directory
      (_working_dir), this program copies only
      the necessary files to the output directory as well as a modified
      _input_file with the reduced directory structure.

      In simple terms, the distribute mode is used to isolate only the
      relevant files for a project.

    3. Clean mode:
      Given a directory structure (_input_file) and the working directory
      (_working_dir), this program removes any generated makefiles.

      makefiles that are %custom are not removed.

    4. Help mode:
      Display the help message.
*/

#include "pmt/base/args.h"

#include <stdio.h>

int
main(int argc, char const* const* argv) {
#if 0
 args_parser* parser = args_parser_create();

 /* void args */
 args_parser_add_arg(parser, arg_info_create("gen-mode", 'g', ARGS_TYPE_VOID, "Generate mode"));
 args_parser_add_arg(parser, arg_info_create("dist-mode", 'd', ARGS_TYPE_VOID, "Distribute mode"));
 args_parser_add_arg(parser, arg_info_create("clean-mode", 'c', ARGS_TYPE_VOID, "Clean mode"));
 args_parser_add_arg(parser, arg_info_create("help", 'h', ARGS_TYPE_VOID, "Display this help message"));

 /* string args */
 args_parser_add_arg(parser, arg_info_create("out-dir", 'o', ARGS_TYPE_STRING, "Output directory"));
 args_parser_add_arg(parser, arg_info_create("src-dir", 's', ARGS_TYPE_STRING, "Source directory of this program"));
 args_parser_add_arg(parser, arg_info_create("in-file", 'i', ARGS_TYPE_STRING, "Input file"));
 args_parser_add_arg(parser, arg_info_create("root-project", 'r', ARGS_TYPE_STRING, "Root project name"));

 args_parser_parse(parser, argc, argv);

 arg_value help_arg = args_parser_get_short(parser, 'h');

 if (help_arg._was_present) {
  args_parser_print_args(parser);
 } else if (args_parser_get_short(parser, 'g')._was_present) {
  printf("Generate mode\n");
 } else if (args_parser_get_short(parser, 'd')._was_present) {
  printf("Distribute mode\n");
 } else if (args_parser_get_short(parser, 'c')._was_present) {
  printf("Clean mode\n");
 }

 args_parser_destroy(parser);
#endif
 return 0;
}
