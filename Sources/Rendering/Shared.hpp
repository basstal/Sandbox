#if defined(RENDERING_EXPORT_API)
#define RENDERING_API __declspec(dllexport)
#elif defined(RENDERING_IMPORT_API)
#define RENDERING_API __declspec(dllimport)
#else
#define RENDERING_API
#endif