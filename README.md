This is a port of Dean Camera's LUFA to NXP LPC1343. You probably don't want to use it yet.
While I'm trying to stick to the LUFA API, drivers won't be 100% portable due to differences
in the USB hardware between the architectures.

Status
------
  * Working
    * Initialization and enumeration
    * Descriptors and configurations
    * Control transfers
  * Mostly working
    * Bulk and interrupt transfers
  * Not working
    * Class drivers
