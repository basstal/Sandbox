#if defined(INFRASTRUCTURES_EXPORT_API)
#define INFRASTRUCTURES_API __declspec(dllexport)
#elif defined(INFRASTRUCTURES_EXPORT_API)
#define INFRASTRUCTURES_API __declspec(dllimport)
#else
#define INFRASTRUCTURES_API
#endif