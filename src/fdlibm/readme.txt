Sources of the fdlibm library extracted from gcc 4.1.1 distributive (they contain 
some fixes, for example aliasing problem is fixed there, otherwise original 
optimized version is broken when compiled without '-fno-strict-aliasing' option)

As noted here: http://java.sun.com/j2se/1.5.0/docs/api/java/lang/StrictMath.html
fdlibm is a reference implementation of java math library and it ensures 
identical results for floating point arithmetics on all the platforms 
supportedby java.
