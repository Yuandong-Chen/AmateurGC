# csapp-gc-calloc
A toy C garbage collector based on csapp malloc lab</br>

<h4>1.How to use it?</h4>
Testing environment: Ubuntu 12.04.5</br>

Suppose you write a program like this one:</br>

#include <...></br>
int main()</br>
{</br>
  ...</br>
  void *hatetofree = (void *)malloc(...);</br>
  ...</br>
  free(hatetofree);</br>
  return 0;</br>
}</br>

then transfer it into the following:</br>

#include "interface3.h" </br>
#include <...> </br>
</br>
int main()</br>
{</br>
  gc_init(); /* to init gc */</br>
  ...</br>
  void *hatetofree = (void *)gc_calloc(...);</br>
  </br>
  ...</br>
  </br>
  /* please forget free(hatetofree); */</br>
  </br>
  gc_exit(); /* to destroy gc */</br>
  return 0;</br>
}</br>

Our tardy garbage collector will help you take care of it.</br>

Another example, please see file "example.c"</br>
<p>> make; ./example;</p></br>

<h3>2.Limits:</h3>
Space Limits: ONLY 14M effective memory.</br>
Only support 0 up to 10000 allocated pointers.</br>
Conservative C collector.</br>
