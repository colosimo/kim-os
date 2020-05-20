Source code organization
========================

Directly going into the details of the code, this the result of ``ls`` command
run at KIM-OS root folder:

.. code-block:: bash

  $ ls -l
  total 60
  drwxrwxr-x 4 colosimo colosimo 4096 mag 18 15:32 app
  drwxrwxr-x 4 colosimo colosimo 4096 mag 18 15:32 arch
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 cli
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 config
  -rw-rw-r-- 1 colosimo colosimo  768 mag 18 15:32 COPYING
  -rw-rw-r-- 1 colosimo colosimo  176 mag 18 15:32 CREDITS
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 doc
  drwxrwxr-x 2 colosimo colosimo 4096 mag 19 15:32 drivers
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 include
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 kernel
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 lib
  -rw-rw-r-- 1 colosimo colosimo 1225 mag 18 15:32 LICENSE
  -rwxrwxr-x 1 colosimo colosimo  146 mag 18 15:32 makeall.sh
  -rw-rw-r-- 1 colosimo colosimo 3632 mag 18 15:32 Makefile
  -rw-rw-r-- 1 colosimo colosimo  326 mag 18 15:32 README
  drwxrwxr-x 2 colosimo colosimo 4096 mag 18 15:32 tasks

Here is a brief explanation for all of the most relevant files and
subdirectories you can find:

- *arch*: contains all the MCU-specific code. At present, it contains two
  subfolders, *unix* and *arm*: the first is intended to make the firmware
  compile on Unix systems, simulating the presence of low-level hardware; the
  second one contains the code for all supported ARM chips;
- *app*: this is the place where your high-level application(s) will reside; any
  line of code inside this folder should be arch-independent, so that the same
  application can be compiled for different SoCs;
- *cli*: it is the **C**\ommand **L**\ine **I**\nterface facility; both the programmer and the
  end user can have a (quite simple) shell-like user experience (e.g. on UART),
  to debug and interact with the firmware itself; a set of pre-defined
  commands is provided; in addition, it is easily extensible;
- *config*: contains all available configurations; a configuration file is
  a *.mk* file included by main Makefile, containing some variables which define
  the desired compile options;
- *drivers*: the code for peripherals and chips external to the SoC; the code
  here must be arch-independent, so that the same chip (e.g. a I2C temperature
  sensor) will be easily usable from whatever arch providing the needed bus
  (e.g. I2C);
- *kernel*: the core of KIM-OS; contains all the (hardware independent)
  functions needed to handle tasks and I/O;
- *include*: header files to be included when using the hardware independent
  code;
- *lib*: some useful generic functions; ``k_printf``, a printf-like function,
  is one of the most important, providing a way of writing formatted strings;
- *tasks*: some generic arch-independent tasks, directly usables and intended
  as an example about how to write your tasks.
