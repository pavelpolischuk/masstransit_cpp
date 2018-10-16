#pragma once

#define MASSTRANSIT_CPP_VERSION_MAJOR 1
#define MASSTRANSIT_CPP_VERSION_MINOR 1
#define MASSTRANSIT_CPP_VERSION_PATCH 0

#if defined _WIN32 || defined __CYGWIN__
#define MASSTRANSIT_CPP_HELPER_DLL_IMPORT __declspec(dllimport)
#define MASSTRANSIT_CPP_HELPER_DLL_EXPORT __declspec(dllexport)
#define MASSTRANSIT_CPP_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define MASSTRANSIT_CPP_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define MASSTRANSIT_CPP_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define MASSTRANSIT_CPP_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define MASSTRANSIT_CPP_HELPER_DLL_IMPORT
#define MASSTRANSIT_CPP_HELPER_DLL_EXPORT
#define MASSTRANSIT_CPP_HELPER_DLL_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define MASSTRANSIT_CPP_API and MASSTRANSIT_CPP_LOCAL.
// MASSTRANSIT_CPP_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// MASSTRANSIT_CPP_LOCAL is used for non-api symbols.

#ifdef MASSTRANSIT_CPP_DLL // defined if MASSTRANSIT_CPP is compiled as a DLL
#ifdef MASSTRANSIT_CPP_DLL_EXPORTS // defined if we are building the MASSTRANSIT_CPP DLL (instead of using it)
#define MASSTRANSIT_CPP_API MASSTRANSIT_CPP_HELPER_DLL_EXPORT
#else
#define MASSTRANSIT_CPP_API MASSTRANSIT_CPP_HELPER_DLL_IMPORT
#endif // MASSTRANSIT_CPP_DLL_EXPORTS
#define MASSTRANSIT_CPP_LOCAL MASSTRANSIT_CPP_HELPER_DLL_LOCAL
#else // MASSTRANSIT_CPP_DLL is not defined: this means MASSTRANSIT_CPP is a static lib.
#define MASSTRANSIT_CPP_API
#define MASSTRANSIT_CPP_LOCAL
#endif // MASSTRANSIT_CPP_DLL

