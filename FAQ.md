Frequently Asked Questions about converting rFactor mods 
========================================================

## How do rFactor and GSC2013 differ?

The overall file structure is the same for both. In GSC2013 the
content of `.mas` files and `.gmt` files have to be encrypted. The
`SearchPath` for `.mas` files is also handled differently and needs to
be adopted when converting mods.

## What is the meaning the SearchPath in .gen files and of the VEHDIR and TEAMDIR variables?

* `<VEHDIR>` gets expanded to `[...]\GameData\Vehicles\`
* `<TEAMDIR>` gets expanded to the directory where the `.veh` file is located

In rFactor <VEHDIR> seems to get expanded to
`[...]\GameData\Vehicles\{MODNAME}\` which makes it necessary to tweak
the path in many mods.

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

## Add-on cars show up in the Mini Challenge races in GSC2013, how to fix that?

The `.veh` file contais `Category="PMC, CHEVROLET`

## How are tracks asigned to karts or race cars?

This is done via the `Filter Properties` setting in the `.gdb` file of
the track. GSC2013 uses these settings:

    Filter Properties = * StockV8

    Filter Properties = * Kart

rFactor mods use different ones, so they have to be changed to show up in GSC2013.

## Sounds are not working when using older/newer GSC mods

GSC2012 has sounds stored in `GameData\Sounds\StockCarV8`

GSC2013 has sounds stored in `GameData\Sounds\`

Move the sound into the proper directory to fix any issues.

## Sound not working in converted rFactor mods

The `.sfx` file defines which sounds are used, in some mods the file
path needs to be changed to include the mod name.

## GameStockCar2013 crashes when starting the mod

No idea, happens with a tiny handful of mods.
