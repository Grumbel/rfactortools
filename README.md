rFactor Tools
=============

rFactorTools is a collections of tools for manipulating and converting
rFactor related files. Main use of these tools right now is
semi-automatic conversion of rFactor mods over to GameStockCar2013.
Conversion is at this point not 100% automatic for all mods, but it
will get you most of the way. The file `FAQ.md` provides solutions for
common problems.

Source code for rfactortools can be found at:

* https://github.com/Grumbel/rfactortools


Features
--------

* conversion of track and vehicle mods from rFactor to GSC2013
* automatic thumbnail generation for tracks
* automatic resizing of track thumbnails and vehicle images
* automatic repacking and encryption of .mas files
* automatic fixing of the SearchPath


Requirements
------------

* Python3.4 (`python3.4`)
* Pillow (`python3-pil`, `python3-pil.imagetk`)

Name of the Ubuntu package are in parenthesis.

On Windows Pillow can be installed with:

    C:\Python34\Scripts\easy_install-3.4.exe pillow

If Python3.3 is used `pathlib`, which is included in Python3.4, is required as well.

Windows binaries are build with `cx_freeze`, which currently only works with Python3.3.


Tools
-----

### `rfactortools-gui`

A GUI that includes the functionality of `rfactor-to-gsc2013`, `vehtool` and `gentool`.

### `rfactor-to-gsc2013`

This script will convert a rFactor mod's `GameData/` directory to GameStockCar2013.

### `masunpack`

Allows you to unpack a `.mas` file or list its content.

### `maspack`

Allows you to pack files into a `.mas` file.

### `imgtool`

Allows you to rescale overlarge menu graphics to fit properly into GameStockCar2013.

### `gentool`

Allows you to inspect `.gen` and `.scn` files and quickly see if
something wrong with the `.mas` files or the `SearchPath`.

### `vehtool`

Allows you to inspect `.veh` files and verify that `Category`, `Team`
and `Classes` are set properly.

### `rfactorcrypt`

Used for encryption, decryption or inspection of rFactor related files.

### `rfactordec`

Used by rfactorcrypt.py for the actual encryption/decryption, downloaded from:

* http://aluigi.altervista.org/search.php?src=decrypter

### `minised`

A command line tool that allows search&replace across multiple files,
similar to the Unix tools `find` and `sed`.

### `minised-gui`

A GUI tool that allows to search&replace across multiple files,
similar to the Unix tools `find` and `sed`.

### `other/MAS2Files.exe`, `other/Files2MAS.exe`

Mostly the same as maspack.py/masunpack.py, provided here for backup, originally downloaded from:

* http://aluigi.org/misc/masfiles.zip

### `other/quickbms/`

Another `.mas` unpacker, originally downloaded from:

* http://aluigi.altervista.org/quickbms.htm


Converting Mods to GameStockCar2013
-----------------------------------

First step to convert a rFactor mod is getting a copy of it's
`GameData/` directory, as that is the only directory relevant for
GameStockCar2013. If the mod comes as `.exe` file, installing it into
an empty directory will do the job even if no copy of rFactor is
available. If the mod comes as `.7z`, `.rar` and `.zip` it has to be
extracted as usual.

Second step is running:

    ./rfactor-to-gsc2013.py .../GameData/ -o gsc/GameData

This will convert the `GameData/` from the mod and save the results in
`gsc/GameData`.

This will rescale all the overlarge menu graphics so that they fit
properly into GameStockCar2013.

The fourth step is:

    ./gentool.py .../GameData/

This won't modify anything, but instead will show you some information
about the mod, namely filenames of `.mas` files and which files
reference them. This lets you quickly see if further work is needed.
See `FAQ.md` for frequently occuring problems.


Disclaimer
----------

rfactortools is a homebrew tool collection for rFactor, Race07 and
GSC2013 and in no way affiliated with Image Space Incorporated, SimBin
Studios or Reiza Studios.
