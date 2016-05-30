//
//  utilities.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/17/16.
//

#include <stdio.h>
#include "utilities.hpp"
#include "utilities-test.hpp"

namespace nbl {
    namespace utilities {

        NBL_MAKE_STRINGS(LevelStrings, NBL_LEVEL_SET)

        bool mayUseColor() {
            const char * term = getenv("TERM");

            if (!term) {
                return false;
            } else {
                std::string str(term);

                return  str.find("xterm") != std::string::npos ||
                        str.find("color") != std::string::npos;
            }
        }

        bool useColor = mayUseColor();

        color_string color_red = "\x1b[31m", color_green = "\x1b[32m", color_yellow = "\x1b[33m", color_blue = "\x1b[34m",
            color_magenta = "\x1b[35m", color_cyan = "\x1b[36m", color_bold_yellow = "\x1b[33;1m",
            color_bold_red = "\x1b[31;1m", color_reset = "\x1b[0m";

        color_string LevelColors[] = {
            color_cyan, color_bold_yellow, color_bold_red, color_reset
        };

        const char * get_path_suffixes(const char * filePath, int elements) {
            ssize_t pos = strlen(filePath) - 1;
            int sepsFound = 0;

            for (;pos >= 0 && sepsFound < elements; --pos) {
                if (filePath[pos] == '/') ++sepsFound;
            }

            if (pos != -1) return filePath + pos + 2;   //found all elements
            else return filePath + pos + 1;             //searched to start of string.
        }

        NBL_MAKE_TEST(PathSuffixTest) {
            const char * path = "/a/b/c";
            NBL_ASSERT(strcmp(get_path_suffixes(path, 1), "c") == 0);
            NBL_ASSERT(strcmp(get_path_suffixes(path, 2), "b/c") == 0);
            NBL_ASSERT(strcmp(get_path_suffixes(path, 3), "/a/b/c") == 0);
            NBL_ASSERT(strcmp(get_path_suffixes(path, 4), "/a/b/c") == 0);
            return true;
        }

        void color_printf(color_string color, const char * format, ...) {
            va_list vaargs;
            va_start(vaargs, format);
            printf("%s", color && useColor ? color : "");
            vprintf(format, vaargs);
            printf("%s", color && useColor ? color_reset : "");
            va_end(vaargs);
        }

        void nbl_print( LogLevel curLev, LogLevel msgLevel, const char * file, int line, const char * format,
                       ...) {
            va_list vaargs;
            va_start(vaargs, format);

            if (msgLevel >= curLev) {
                color_string prefixColor = (useColor) ?
                LevelColors[msgLevel] : "";
                color_string postfixColor = (useColor) ?
                    color_reset : "";

                //just get first char of LevelString
                printf("%s[%c][%s:%d]: ",
                       prefixColor, *(LevelStrings[msgLevel]),  get_path_suffixes(file, 1), line);
                vprintf(format, vaargs);
                //use std::cout for std::endl flushing
                std::cout << postfixColor << std::endl;
            }
            
            va_end(vaargs);
        }

        const int break_char_size = 50;
        void print_divider( int pre, int post, color_string color ) {
            std::string breakStr(break_char_size, '-');
            std::string preStr(pre, '\n');
            std::string postStr(post + 1, '\n');

            std::cout << (color && useColor ? color : "") << preStr << breakStr
                << postStr << (color && useColor ? color_reset : "");
        }

        const char * space_string = nullptr;
        const char * indent(int spaces) {
            NBL_ASSERT(spaces < 255 && spaces > 0);

            if (space_string == nullptr) {
                char * interim = (char *) malloc(256);
                memset( (void *) interim, ' ', 256);
                interim[255] = '\0';
                space_string = interim;
            }

            return space_string + (255 - spaces);
        }
        
        std::string format(const char * format, ...) {
            va_list args;
            va_start(args, format);

            //+1 for \0
            ssize_t len = (ssize_t) vsnprintf(NULL, 0, format, args) + 1;
            if (len < 0) throw std::runtime_error("could not format string!");

            va_end(args);
            va_start(args, format);

            char buffer[ len ];
            vsnprintf(buffer, len, format, args);
            va_end(args);

            return std::string(buffer);
        }

        std::string get_error(const int errsaved) {
            static const size_t max_msg_size = 256;

#ifdef __APPLE__
            char buf[max_msg_size];
            buf[0] = '\0';
            strerror_r(errsaved, buf, max_msg_size);
#else
            char optbuf[max_msg_size];
            optbuf[0] = '\0';
            const char * buf = strerror_r(errsaved, optbuf, max_msg_size);
#endif

            NBL_CHECK(buf);
            NBL_CHECK( strlen(buf) );

            return std::string(buf);
        }

        void safe_perror(const int err) {
            printf("ERROR: %s\n", get_error(err).c_str());
        }

        bool safe_perror_describe(const int err, const char * file, int line, const char * description) {
            std::string errorstr = get_error(err);
            nbl_print(NBL_INFO_LEVEL, NBL_ERROR_LEVEL, file, line, "Error for '%s' was '%s'",
                      description, errorstr.c_str());

            return true;
        }
    }
}

#include <map>
#include <vector>
#include <memory>

#define NBL_LOGGING_LEVEL NBL_INFO_LEVEL

using namespace nbl::utilities;

namespace nbl {
    namespace test {
        const char default_section_s[] = NBL_STRINGIFY(DEFAULT_SECTION) ;
        std::map<std::string, std::vector<std::shared_ptr<TestBase>>> * all_tests = nullptr;
        size_t test_count = 0;

        char test_no_color[] = "";
        color_string test_section_color = test_no_color;
        color_string test_start_color = test_no_color;
        color_string test_passed_color = test_no_color;
        color_string test_failed_color = test_no_color;
        color_string test_post_color = test_no_color;

        bool TestBase::runTest() {

            printf("\n%s%s[%s]\n%sfrom [%s:%d]\n%s",
                   test_start_color, indent(4), name, indent(8),
                   get_path_suffixes(file, 3),
                   line,
                   test_post_color);

            bool ret = testBody();

            printf("%s%s[%s]%s\n", ret ? test_passed_color : test_failed_color, indent(4),
                   ret ? "passed" : "failed", test_post_color);
            
            return ret;
        }

        void init_all_tests() {
            if (all_tests == nullptr) {
                NBL_WARN("initializing test environment...")
                all_tests = new std::map<std::string, std::vector<std::shared_ptr<TestBase>>>;

                if (mayUseColor()) {
                    test_section_color = color_yellow;
                    test_start_color = color_cyan;
                    test_passed_color = color_green;
                    test_failed_color = color_red;
                    test_post_color = color_reset;
                }

                //ensure default section exists
                (*all_tests)[default_section_s];
            }
        }

        void add_test_to(const char * section, TestBase * test) {
            NBL_ASSERT(test);
            NBL_ASSERT(test->file);
            NBL_ASSERT(test->name);
            init_all_tests();

            NBL_INFO("registering test %s", test->name);
            ++test_count;
            (*all_tests)[section].push_back(std::shared_ptr<TestBase>(test));
        }

        bool run_section(std::string section) {
            printf("%s%srunning section <%s>%s\n",
                   test_section_color, indent(2),
                   section.c_str(), test_post_color);
            print_divider(0, 0, test_section_color);

            if ( all_tests->find(section) == all_tests->end() ) {
                NBL_ERROR("section %s did not designate any tests!", section.c_str());
                return false;
            }

            if (all_tests->at(section).empty()) {
                printf("%s\t... no tests found ...\n%s", test_section_color, test_post_color);
            } else {
                for (std::shared_ptr<TestBase>& tp : all_tests->at(section)) {
                    if (!tp->runTest()) return false;
                }
            }

            return true;
        }

#define REQUIRE_TRUE(v) if (!v) { NBL_ERROR("section failed!") return false; }

        bool run_all_tests(const char * section) {
            init_all_tests();

            if (section && (all_tests->find(section) != all_tests->end())) {
                REQUIRE_TRUE(run_section(section));
                return true;
            } else if (section == NULL) {

                printf("%s%srunning all nbl tests (%zd total)%s\n",
                       test_section_color, indent(8), test_count,  test_post_color);
                print_divider(0, 1, test_section_color);

                REQUIRE_TRUE(run_section( default_section_s ));
                all_tests->erase( default_section_s );

                for (auto& pair : *all_tests) {
                    printf("\n");
                    REQUIRE_TRUE(run_section(pair.first));
                }

                printf("\n%sdone%s\n", test_section_color, test_post_color);
                print_divider(0, 2, test_section_color);
                delete all_tests;
                return true;
            } else {
                NBL_ERROR("section %s did not designate any tests!", section);
                return false;
            }
        }

        /* all tests in this file must be added AT THE BOTTOM */
        NBL_ADD_TEST_TO(AddTest, nbl_tests) {
            return true;
        }
        NBL_ADD_TEST_TO_SECTION(PathSuffixTest, nbl_tests)
    }
}



























