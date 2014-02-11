Frequently Asked Questions about converting rFactor mods 
========================================================

## What is the meaning the SearchPath in .gen files and of the VEHDIR and TEAMDIR variables?

    SearchPath=<VEHDIR>
    SearchPath=<TEAMDIR>

SearchPath is completely ignored by GSC2013, instead it searches
from the .gen file downwards the directory tree till it hits
GameData/Vehicle/.


## "Error loading texture *name*"

A texture file is missing, if you are sure that it is contained in
the .mas, check for .mas file naming conflicts.


## "Error loading mesh *name* to memory"

A mesh file can't be found, either missing or a .mas name conflict.


## "Wrong GMT Version(or GMT version not allowed)"

The .gmt file is not encrypted or encrypted with the wrong key, use
rfactorcrypt.py to check what the status of the file is and fix it.


## What are all the different file formats in rFactor/Race07/GameStockCar2013?

* .aiw: AI waypoints
* .ani: animated flag marshals
* .cam: camera positions
* .cin: same as .grd in GameStockCar2013
* .dds: DDS textures
* .gdb: track file, contains rules, location, lighting, names, etc.
* .gen: Same as .scn, used for cars
* .gmt: 3D models
* .grd: pre race cutscene presentation script in Race07
* .hat: compiled form of the .aiw file
* .mas: zip-like archive file containing textures and 3D models
* .scn: scene defintion file, used for tracks
* .tdf: defines grip and track surfaces
* .tga: textures
* .trk: Race07 analog to .scn files
* .veh: vehicle file


## How are vehicle menu graphics done in GameStockCar2013?

A 252x64 TGA file placed next to the `.veh` file. For:

`GameData/Vehicles/F_Reiza/Brackley GP/Brackley8.veh`

this becomes:

`GameData/Vehicles/F_Reiza/Brackley GP/Brackley8number.tga`


## How are track menu graphics done in GameStockCar2013?

If the track file is
`GameData/Locations/Jacarepagua/Jacarepagua1/Jacarepagua1.gdb` then
the loading screen becomes:

`GameData/Locations/Jacarepagua/Jacarepagua1/Jacarepagua1_loading.jpg`

and the menu thumbnail becomes

`GameData/Locations/Jacarepagua/Jacarepagua1/Jacarepagua1mini.tga`

Loading screens are ~1024x768 (size can varry)

Menu thumbnails are ~252x249 (size can varry)

