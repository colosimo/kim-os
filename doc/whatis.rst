What is KIM-OS
==================

.. index:: KIM

KIM-OS (briefly, KIM) stands for
"**K**\eep-**I**\t-**M**\inimal **O**\perating **S**\ystem". It is a
state-machine based operating system targeted to Micro Controller Units. Its
architecture is intended to be fully cross-platform, though the MCUs currently
supported are all ARM Cortex-M core based.

The basic characteristic of KIM is that it can be compiled with simple,
opensource, tools, like *gcc*-based toolchain, ``make`` and basic ``bash``
commands. A laptop with your favourite Linux distribution is all you need to
compile it.

The idea of starting with this project, and most of its contents, came from my
professional activities. I am a freelance firmware engineer, with a background
in Linux Embedded. I started to work on MCUs a few years ago, and my first
understanding about how to approach was provided by the
BATHOS [#f1]_ project, which was my first
reference for experiments and tests, and is the model I adopted for KIM
itself.

When having to release specific firmware for my clients, I began to collect some
code snippets and conceptual ideas in a library that was growing over the time,
and eventually ended up in KIM project. So, I can say KIM is the sum of all of
my experience in microcontrollers, put together in a (hopefully) orderly form.

All of the source code is provided on MIT-style [#f2]_ license, so that you can
do whatever you want with it, as long as you declare that you are using it (see
LICENSE for more information).

.. rubric:: Footnotes
.. [#f1]  **B**\orn-**A**\gain **T**\wo **H**\our **O**\perating **S**\ystem,
   see `<https://github.com/a-rubini/bathos>`_
.. [#f2] See `<https://opensource.org/licenses/MIT>`_
