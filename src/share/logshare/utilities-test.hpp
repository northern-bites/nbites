//
//  utilities-test.hpp
//  xcode-visibility
//
//  Created by Philip Koch on 4/26/16.
//

#ifndef utilities_test_h
#define utilities_test_h

#if defined(NBL_UTILITIES_VERSION) && NBL_UTILITIES_VERSION != 2
#error "wrong NBL_UTILITIES_VERSION"
#endif

#define NBL_UTILITIES_VERSION 2

#include "utilities.hpp"
#include <memory>

namespace nbl {

    namespace test {

        class TestBase {
        public:
            const char * file, * name;
            int line;

            TestBase(const char * f, const char * n, int l) :
                file(f), name(n), line(l) {}
            bool runTest();
            virtual bool testBody() = 0;
        };

        bool run_all_tests(const char * section);
        void add_test_to(const char * section, TestBase * test);

        struct static_test_adder {
            static_test_adder(const char * section, const char * name, TestBase * test) {
                add_test_to(section, test);
            }
        };
    }
}


#ifndef NBL_NO_TESTS

#define NBL_ADD_TEST_TO_SECTION(name, section) \
    struct nbl::test::static_test_adder nbl_test_adder ## name ## __LINE__ \
        { #section, #name, new name ## _TestClass() };

#define NBL_RUN_TESTS_IN(section)   \
    nbl::test::run_all_tests(#section)

#define NBL_RUN_ALL_TESTS() nbl::test::run_all_tests( nullptr )

#else

#define NBL_ADD_TEST_TO_SECTION(name, section)

#define NBL_RUN_TESTS_IN(section)
#define NBL_RUN_ALL_TESTS()

#endif

#define NBL_DECLARE_TEST(name) \
    class name ## _TestClass : public nbl::test::TestBase { \
        public: name ## _TestClass() : nbl::test::TestBase( __FILE__ , #name , __LINE__ ) {}    \
        bool testBody();    };

/* requires that definition follows immediately */
#define NBL_DEFINE_TEST(name)   \
    bool name ## _TestClass::testBody()

#define NBL_MAKE_TEST(name) \
    NBL_DECLARE_TEST(name)  \
    NBL_DEFINE_TEST(name)   \

#define NBL_ADD_TEST(name) \
    NBL_DECLARE_TEST(name)  \
    NBL_ADD_TEST_TO_SECTION(name, DEFAULT_SECTION)   \
    NBL_DEFINE_TEST(name)

#define NBL_ADD_TEST_TO(name, section)  \
    NBL_DECLARE_TEST(name)  \
    NBL_ADD_TEST_TO_SECTION(name, section)  \
    NBL_DEFINE_TEST(name)

#define NBL_ASSERT_THROW(statement, failure_msg_cstr)   \
    NBL_ASSERT_THROW_OF(..., statement, failure_msg_cstr)

#define NBL_ASSERT_THROW_OF(exception, statement, failure_msg_cstr)               \
    { bool _nbl_test_found_exception_ = false;                                  \
    try { statement ; } catch( exception ) {_nbl_test_found_exception_=true;}   \
    if (!_nbl_test_found_exception_) {                                          \
    NBL_ERROR("\n  statement %s failed to generate exception<%s>:\n\t%s", #statement, #exception, failure_msg_cstr)    \
    assert(false);   } }

#endif /* utilities_test_h */
