#ifdef INFRASTRUCTURES_EXPORT_API
#define INFRASTRUCTURES_API __declspec(dllexport)
#else
#define INFRASTRUCTURES_API __declspec(dllimport)
#endif