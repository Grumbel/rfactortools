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

A mesh file can't be found, either missing or a `.mas` name conflict.


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

All the converted vehicle mods currently go into the `reiza5` class,
which is used for the Minis. To fix that edit the `.veh` files in 
`GameData/Vehicles/Mini_Challenge/` and change:

    Classes="reiza5"

to:

    Classes="reiza5, MiniChallenge"

There might be cleaner ways without modifying those files, but this
seems to work, at least for single player.

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

## Some files need modifications, how do I automate that?

If a `.gen` files contains the line `MASFile=Season_1990` and you want
to change it to `MASFile=SRM_1990\Season_1990` for all the .gen files
you can do that with a simple call to `find` and `sed`, like this:

    find .../GameData/ -name "*.gen" -exec sed -i "s/MASFile=Season_1990/MASFile=SRM_1990\\Season_1990/i" {} \;

Adding a new `SearchPath` entry works something like this: 

    find .../GameData/ -name "*.gen" -exec sed -i 's#SearchPath=<VEHDIR>#SearchPath=<VEHDIR>\nSearchPath=<VEHDIR>\\WRC_By_TeamFMR#i' {} \;

Note that these are just rough examples, they might not work for your
case.

## What are the most common issues with vehicle conversion?

Many mods contain a `cmaps.mas` that conflicts with the `cmaps.mas`
provided by GameStockCar2013. Solution to that is to change all
references to `cmaps.mas` in the mod by appending the name of the mod
directory, i.e. in the `.gen` files `MASFile=cmaps.mas` would be
changed into `MASFile=SRM_1990\\cmaps.mas`.

The `SearchPath` also often needs the addition of the mod directory
name, i.e. `SearchPath=<VEHDIR>\SomeDir` would become
`SearchPath=<VEHDIR>\Modname\SomeDir`.

Those two changes are all that should be needed for most vehicle mods.

## What are the most common issues with track conversion?

The skybox is handled slightly differently in rFactor and
GameStockCar2013 and you will often get messages about `horizont.gmt`
or broken `skyboxi`. 

Editing the `.scn` file of the track and removing the
`INSTANCE=skyboxi { ... }` will make the error messages go away, it
will however leave you without a sky. You can replace the `skyboxi`
section and add a `sky.mas` file from another mod to get a sky back.
See these links for more detailed instructions:

* http://www.gtr3.co.uk/forum/index.php?t=msg&th=102&goto=710&#msg_710
* http://www.gtr3.co.uk/forum/index.php?t=msg&th=114&goto=793&#msg_793

In the `.gdb` file the `TestDaystart` might not be set, add a line
`TestDaystart = 14:00` to cause tests not to start at the default
value (9:00).

In the `.tdf` the grip levels might need adjustments, i.e. change `RoadDryGrip=1.00` to something like 1.02-1.04

## Part of the cockpit is cut off

In the cars `.cam` file adjust the `ClipPlanes` for the `COCKPIT` camera:

    LocalCam=COCKPIT
    {
      [...]
      ClipPlanes=(0.0225000, 500.00000)
      [...]
    }
    
Making the left value smaller should reduce the distance where objects
are cut off. The value must be positive and not null.

## Parts of the car are visible in the mirror

Editing your `.plr` file inside `USERDATA\(your name)` and changing
the 15 in `Self In Cockpit Rearview="15"` to 0 might help. Not sure
how to fix the mod itself, will probably need some tweaking of the
VISGROUP.

## Where can I find more info on conversion?

This thread contains a lot more info on how to tweak specific things about tracks:

* http://www.gtr3.co.uk/forum/index.php?t=msg&th=102
