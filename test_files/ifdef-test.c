#ifdef TEST
#error "TEST was not defined, so this should have never been reached"
#else
"else success";
#endif

#define BLOBCATCOZY
#ifdef BLOBCATCOZY
"ifdef success";
#endif

#ifndef BLOBCATCOZY
#error "BLOBCATCOZY is indeed defined, so this should have never been reached"
#else
"ifndef success";
#endif

#ifdef ABC
#error "ABC is not defined, so this should have never been reached"
#elifdef BLOBCATCOZY
"elifdef success";
#endif

#ifdef ABC
#elifndef BLOBCATCOZY
#error "ABC is not defined, so this should have never been reached"
#else
"elifndef success";
#endif

#ifndef ABC
#ifdef BLOBCATCOZY
"nested conditionals success 1";
#endif
#endif

#ifdef ABC
#else
#ifdef BLOBCATCOZY
"nested conditionals success 2";
#endif
#endif
