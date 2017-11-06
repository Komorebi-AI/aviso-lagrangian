/* Generated by Cython 0.23.4 */

#ifndef __PYX_HAVE__lagrangian
#define __PYX_HAVE__lagrangian


#ifndef __PYX_HAVE_API__lagrangian

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

__PYX_EXTERN_C DL_IMPORT(int) PythonFieldCompute(PyObject *, double, double, double, double *, double *, bool *, std::string *);
__PYX_EXTERN_C DL_IMPORT(int) PythonReaderOpen(PyObject *, std::string, std::string *);
__PYX_EXTERN_C DL_IMPORT(int) PythonReaderLoad(PyObject *, std::string, std::string, std::string *);
__PYX_EXTERN_C DL_IMPORT(int) PythonFieldInterpolate(PyObject *, double, double, double, lagrangian::CellProperties, double *, std::string *);
__PYX_EXTERN_C DL_IMPORT(int) PythonFieldGetDateTime(PyObject *, std::string, lagrangian::DateTime *, std::string *);

#endif /* !__PYX_HAVE_API__lagrangian */

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC initlagrangian(void);
#else
PyMODINIT_FUNC PyInit_lagrangian(void);
#endif

#endif /* !__PYX_HAVE__lagrangian */
