# **Credits, Acknowledgements, and More**

## Credits and Acknowledgements
Iosefka is created by Cory Oleson under the title Global Sequence.

Iosefka would not be possible without the explicit help of many helpful anonymous strangers on the internet, and the implicit assistance of countless open source projects on the internet.

An extra special shout out goes to Frédéric Meslin of Fred's Lab and Èmilie Gillet of the late Mutable Instruments. Also for Maffez's version of the 2140 pole mixing filter which was very helpful alongside Sound Semiconductor's documentation. 

This synthesizer is fully open source under the GPL-3.0 license.

## A Note on the Use of Artificial Intelligence

### What AI was used for in this project
- As an aide to answer general programming questions relating to design patterns, both C and C++ languages, Git/GitHub, interpreting some open source code, etc.
- Assistance understanding DSP concepts
- As a way of creating and editing large enums and arrays from existing values
- Occasionally detecting errors in code
- The one line of ASM for the phase overflow flag used in the oscillator sync functionality
- Generating the text that goes into every .h, .c, and .cpp file, with the basic includes at the top. Example:
```
#ifndef EXAMPLE_H  
#define EXAMPLE_H  
  
#include <stdio.h>  
#include "pico/stdlib.h"  
  
#ifdef __cplusplus  
extern "C"  
{  
#endif  
  
// C header here  
  
#ifdef __cplusplus  
}  
#endif  
  
#endif
```

### What AI was NOT used for in this project
- Vibe coding - No whole functions, classes, or files were written by AI tools 
- The concept, architecture, or structure of the synthesizer itself
- Any of the hardware design and layout
- Writing the manual
- 
... and for better or for worse, I believe it shows.