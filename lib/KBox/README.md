# KBox library

This library contains all the core components of the KBox project.

 - drivers/: low-level drivers to interact with hardware
 - tasks/: Task to run regularly (via TaskManager.h) to stay in touch with
   events
 - pages/: Pages to display on screen (via MFD.h)

A typical `main.cpp` (or `main.ino`) will:

 - initialize the drivers you want to use,
 - create the tasks and add them to the task manager,
 - create the pages and add them to the MFD controller,
 - run taskmanager->loop() and mfd->loop() regularly.

