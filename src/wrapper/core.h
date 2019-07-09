/* Generated by Cython 0.29.11 */

#ifndef __PYX_HAVE__lagrangian__core
#define __PYX_HAVE__lagrangian__core


#ifndef __PYX_HAVE_API__lagrangian__core

#ifndef __PYX_EXTERN_C
  #ifdef __cplusplus
    #define __PYX_EXTERN_C extern "C"
  #else
    #define __PYX_EXTERN_C extern
  #endif
#endif

#ifndef DL_IMPORT
  #define DL_IMPORT(_T) _T
#endif

__PYX_EXTERN_C int PythonFieldCompute(PyObject *, double, double, double, double *, double *, bool *, std::string *);
__PYX_EXTERN_C int PythonReaderOpen(PyObject *, std::string, std::string *);
__PYX_EXTERN_C int PythonReaderLoad(PyObject *, std::string, std::string, std::string *);
__PYX_EXTERN_C int PythonFieldInterpolate(PyObject *, double, double, double, lagrangian::CellProperties, double *, std::string *);
__PYX_EXTERN_C int PythonFieldGetDateTime(PyObject *, std::string, lagrangian::DateTime *, std::string *);

#endif /* !__PYX_HAVE_API__lagrangian__core */

/* WARNING: the interface of the module init function changed in CPython 3.5. */
/* It now returns a PyModuleDef instance instead of a PyModule instance. */

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC initcore(void);
#else
PyMODINIT_FUNC PyInit_core(void);
#endif

#endif /* !__PYX_HAVE__lagrangian__core */
