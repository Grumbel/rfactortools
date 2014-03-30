rFactorTools
============

rFactorTools is a collections of tools for manipulating and converting
rFactor related files. The main use of these tools right now is
automatic conversion of rFactor mods over to GameStockCar2013.
Conversion is at this point completely automatic for most mods, but
some problems might remain. The file `FAQ` provides solutions for
common problems.

Full source code for rfactortools can be found at:

* https://github.com/Grumbel/rfactortools

Bug reports can be submitted at:

* https://github.com/Grumbel/rfactortools/issues


Features
--------

* conversion of track and vehicle mods from rFactor to GSC2013
* ablity to tweak class and categories
* automatic thumbnail generation for tracks
* automatic resizing of track thumbnails and vehicle images
* automatic repacking and encryption of .mas files
* automatic fixing of the SearchPath in .gen and .scn files


Requirements
------------

* Python3.4 (`python3.4`)
* Pillow (`python3-pil`, `python3-pil.imagetk`)

Name of the Ubuntu package are in parenthesis.

On Windows Pillow can be installed with:

    C:\Python34\Scripts\easy_install-3.4.exe pillow

If Python3.3 is used `pathlib`, which is included in Python3.4, is required as well.

Windows binaries are build with `setup_cxfreeze.py`, which currently
only works with Python3.3.


Tools
-----

`rfactortools-gui` and `minised-gui` are the main tools of this
package. The `rfactortools-gui` is a GUI that allows you to convert rFactor
track and car mods to Game Stock Car 2013 and `minised-gui` is a text
search&replace tool that works across multiple files at once. The rest
of the tools don't provide a GUI and are meant to be used from command
line and are rarely needed.

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


Disclaimer
----------

rfactortools is a homebrew tool collection for rFactor, Race07 and
GSC2013 and in no way affiliated with Image Space Incorporated, SimBin
Studios or Reiza Studios.
