Writing your own application
============================

This chapter provides a practical description about how to implement your
own application, (*app* in brief). Basically, these are the steps:

- prepare a suitable config, e.g.:

.. code-block:: make

	ARCH := arm
	CPU := cortex-m0
	SOC := stm32f030
	SOC_VARIANT := stm32f030c8
	BOARD := wonderful-board
	APP := wonderful-app

- if your SoC is not yet supported, you need to implement low-level drivers,
  by adding a proper subdirectory in *arch* (see :doc:`arch`),
  at least for the peripherals you're about to use; *soc-stm32l4rxxx* or
  *soc-stm32f407xx* can be a taken as a model for your implementation;

.. note::

  Quite often, the SoC peripherals of chips produced by the same vendor are
  very similar, if not exactly the same; at present, KIM does has no concept
  of *generic vendor drivers*, but it's something to improve quite soon, in
  order to avoid, or at least reduce, code duplication.

- once you have all your low-level part implemented, create the proper *app*
  subdirectory, *app/wonderful-app* in our example;

- inside this directory, you need to add:

  * the ``.mk`` Makefile include, having the same name of the app itself;

  * one or more tasks with at least one task declared without the
    ``no_autorun`` flag set to 1;

- as a basic example, the clock app uses the default generic
  tasks/task-clock.c in order to provide the implementation of a simple task.
