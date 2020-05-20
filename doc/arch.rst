The *arch* directory
====================

The *arch* directory is where all hardware-specific code should be: MCU register
definitions, drivers for SoC peripherals, code to boot a specific board with
required alternate GPIO settings, etc...

A quick look at its tree will clarify the logic behind it:

.. code-block:: bash

	$ tree -d arch/
	arch/
	├── arm
	│   ├── cpu-cortex-m0
	│   │   ├── include
	│   │   ├── soc-lpc111x
	│   │   │   ├── board
	│   │   │   └── include
	│   │   └── soc-stm32f030
	│   │       ├── board
	│   │       └── include
	│   ├── cpu-cortex-m3
	│   │   ├── include
	│   │   └── soc-stm32f103
	│   │       ├── board
	│   │       └── include
	│   └── cpu-cortex-m4
	│       ├── include
	│       ├── soc-stm32f407xx
	│       │   ├── board
	│       │   └── include
	│       ├── soc-stm32f469xx
	│       │   ├── board
	│       │   └── include
	│       └── soc-stm32l4rxxx
	│           ├── board
	│           └── include
	└── unix

The *arm* directory is where most of the support efforts have been made so far;
the subdirectories hieararchy reflects the hardware hierarchy itself:

- at top, we can find *arm*, and in this folder some common code for all
  Cortex-M processor is present;

- next level is the Cortex-M version (M0/M3/M4); the init function for these
  cores is here;

- next follows the SoC level: for each specific chip model, it basically hosts
  the internal peripherals drivers; in the *include* directory is the ``reg.h``
  file, where all registers for that SoC are listed;

- last level is the board one, which hosts the specific low-level initialization
  needed by a board, together with the devices declarations.

.. note::

  Despite other context (e.g. the separation between Kernel and User Space in
  Linux), in KIM there are no hard constraints in having all low-level code in
  arch: registers are accessible from wherever, and it is sometimes quicker to
  directly  configure a peripherals by writing in its registers instead of
  writing a driver and use it. This flexibility is left to developer's needs and
  feelings; the main suggestion is not to abuse of direct register calls,
  especially if the code is needed to maintained and reused over the time,
  and/or will run on different hardware platforms.
