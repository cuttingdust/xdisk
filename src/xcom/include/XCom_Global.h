#ifndef XCOM_GLOBAL_H
#define XCOM_GLOBAL_H

#ifdef _WIN32
#ifdef xcom_EXPORTS
#define XCOM_EXPORT __declspec(dllexport)
#else
#define XCOM_EXPORT __declspec(dllimport)
#endif
#else
#define XCOM_EXPORT
#endif

#endif // XCOM_GLOBAL_H
