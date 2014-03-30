rfactortools-0.2.1
------------------

* added "Filter Properties" to GUI (allows you to race Karts on the tracks)
* removed a lot of command line .exe tools, as they aren't really needed anymore (they are still in the Github repo if you need them)
* added detection magic for track install directory
* added support for converting multiple mods in one go


rfactortools-0.2.0
------------------

* now tries to automatically fix .sfx files, so sound should be working automatically in most mods
* added GUI options to add a track and vehicle category
* added GUI option to change vehicle classes
* fixed problem with mods containing cmaps.mas, this fixes F1SR 1994 among other mods
* appends now the proper modname to the Team name instead of just a random number
* better exclusion of unneeded files, so less risk of breaking your GSC2013 install
* full log files are written to logs/
Note: If you try to convert a track, you have to place it now inside an empty "GameData/Locations/" directory, otherwise the tool won't recognize it.

### Known Bugs:

* Vehicle mods containing multiple .mas files with the same name aren't handled properly (e.g. one global FORM.mas and one FORM.mas in the TeamDir)
* SearthPath in track files aren't adjusted when they are installed outside of their intended location (i.e. some mods expect to be placed in Locations/70tracks/... but don't contain the 70tracks/ directory)
