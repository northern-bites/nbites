#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <vector>

void populate_options(
   boost::program_options::options_description &config_file_options);

/**
 * populates the variable map with the options from argv and the config files.
 *
 * first invocation should be from main, and subsequent invocations will recall
 * argv used in first invocation and overwrite with any new argv.
 *
 * unfortunately, once a variable is set in a variable map, there is no way to
 * change it, so we have to create a whole new map and reparse everything.
 *
 * @param argv    a vector of command-line options
 * @param vm      the variable map to fill
 * @param generic any additional options (like help/version) to be understood
 * @returns a complete options description of all supported options
 */
boost::program_options::options_description
store_and_notify(std::vector<std::string> argv,
                 boost::program_options::variables_map &vm,
                 boost::program_options::options_description* generic =
                    NULL);

/**
 * populates the variable map with the options from argv and the config files.
 *
 * first invocation should be from main, and subsequent invocations will recall
 * argv used in first invocation and overwrite with any new argv.
 *
 * unfortunately, once a variable is set in a variable map, there is no way to
 * change it, so we have to create a whole new map and reparse everything.
 *
 * @param argc    the length of argv
 * @param argv    an array of command-line options
 * @param vm      the variable map to fill
 * @param generic any additional options (like help/version) to be understood
 * @returns a complete options description of all supported options
 */
boost::program_options::options_description
store_and_notify(int argc, char **argv,
                 boost::program_options::variables_map &vm,
                 boost::program_options::options_description* generic =
                    NULL);

void options_print(boost::program_options::variables_map &vm);
