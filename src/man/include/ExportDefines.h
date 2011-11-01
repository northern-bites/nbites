
//required so that the compiler doesn't mangle symbol names
#ifdef __cplusplus
#define START_FUNCTION_EXPORT extern "C" \
{

#define END_FUNCTION_EXPORT }
#endif

