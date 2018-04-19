 %module rtt_interface
 %{
 /* Includes the header in the wrapper code */
 #include "rtt_interface.h"
 %}
 
 /* Parse the header file to generate wrappers */
 %include "rtt_interface.h"
