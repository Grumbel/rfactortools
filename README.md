# rFactor Tools

rFactorTools is a collections of tools for manipulating and converting
rFactor related files. Main use of these tools right now is
semi-automatic conversion of rFactor mods over to GameStockCar2013.
Conversion is at this point not 100% automatic for all mods, but it
will get you most of the way. The file `FAQ.md` provides solutions for
common problems.

These tools are written and tested in Linux only at this point.

## Requirements

* Python3 (`python3`)
* Pillow (`python3-pil`)
* Cairo (`python3-cairo`)

Name of the Ubuntu package is in parenthesis.

## Tools

### `rfactor-to-gsc2013.py`

This script will convert a rFactor mod's `GameData/` directory to GameStockCar2013.

### `masunpack.py`

Allows you to unpack a `.mas` file or list its content.

### `maspack.py`

Allows you to pack files into a `.mas` file.

### `imgtool.py`

Allows you to rescale overlarge menu graphics to fit properly into GameStockCar2013.

### `gentool.py`

Allows you to inspect `.gen` and `.scn` files and quickly see if
something wrong with the `.mas` files or the `SearchPath`.

### `vehtool.py`

Allows you to inspect `.veh` files and verify that `Category`, `Team`
and `Classes` are set properly.

### `rfactorcrypt.py`

Used for encryption, decryption or inspection of rFactor related files.

### `rfactordec`

Used by rfactorcrypt.py for the actual encryption/decryption, downloaded from:

* http://aluigi.altervista.org/search.php?src=decrypter

### `other/MAS2Files.exe`, `other/Files2MAS.exe`

Mostly the same as maspack.py/masunpack.py, provided here for backup, originally downloaded from:

* http://aluigi.org/misc/masfiles.zip

### `other/quickbms/`

Another `.mas` unpacker, originally downloaded from:

* http://aluigi.altervista.org/quickbms.htm

## Converting Mods to GameStockCar2013

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
