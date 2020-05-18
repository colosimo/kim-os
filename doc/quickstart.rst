Quick start reference
=====================

The prerequisite to compile kim-os for ARM cores is to have basic tools for
development in C language on ARM:

- ARM gcc toolchain, e.g. (in my Ubuntu 18.10 system) gcc-arm-none-eabi package;
  check and fix ``CROSS_COMPILE`` variable in ``arch/arm/arm.mk`` if needed
- ``make``
- ``git``

If you want to compile the basic kim-os system from scratch, these are the
steps:

- go to a suitable local path and download the up-to-date git version:

.. code-block:: bash

  $ cd /your/local/path
  $ git clone git@github.com:colosimo/kim-os.git
  Cloning into 'kim-os'...
  remote: Enumerating objects: 286, done.
  remote: Counting objects: 100% (286/286), done.
  remote: Compressing objects: 100% (137/137), done.
  remote: Total 1537 (delta 157), reused 256 (delta 147), pack-reused 1251
  Receiving objects: 100% (1537/1537), 203.59 KiB | 0 bytes/s, done.
  Resolving deltas: 100% (851/851), done.
  Checking connectivity... done.

- go into downloaded directory and compile your config (e.g.
  ``discovery_f407vg`` is for STM32 F407 Discovery Board):

.. code-block:: bash

  $ cd kim-os
  $ make CONFIG=discovery_f407vg

The files ``cli.elf``, ``cli.hex`` and ``cli.bin`` will be created; their name
before prefix is given by the ``APP`` variable defined in the choosen config
(*cli* in our example regarding Discovery Board).

.. note::
  The configuration chosen by ``make CONFIG=etc`` is saved in local *.config* file;
  for this reason, all subsequent compile operations on the same configuration
  do not need ``CONFIG=`` to be specified. Before changing from one configuration
  to another, it is recommended to perform a ``make clean`` operation.
