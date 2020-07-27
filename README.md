# Smart8r
Runtime system in C++ to run on Raspberry Pi for X-10 and Philips Hue devices.

Rules must be written in the Rules language and compiled using "Compiler2" running on Windows.
Then the rules can be trialed on Windows using this code (X-10 emulated but Philips Hue works).
Then the rules can be executed on a Raspberry Pi using this code.
You need an XM10FL, TW523 or equivalent to talk to the X-10 switches.
The XM10FL is attached via the Raspberry Pi GPIO data pins.
You need a Philips Hue Hub attached to the ethernet LAN to talk to the Philips Hue lamps.
The runtime system comprises 3 threads.
1) Rules thread
2) Monitor XM10FL thread
3) Monitor Philips Hub

See documentation of the compiler2 and here for further details.
