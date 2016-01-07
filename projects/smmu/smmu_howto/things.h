/***************************************************************************
 *      things.h for SMMU FraggleScript                                    *
 *              All the things and their thing numbers                     *
 *              Also thing flags                                           *
 *              Just include this file and use the consts provided!        *
 ***************************************************************************/

        /************* thing flags ***********/

        // player
const PLAYER = 0;

        // enemies
const TROOPER = 1;
const SHOTGUY = 2;
const ARCHVILE = 3;
const REVENANT = 5;
const MANCUBUS = 8;
const CHAINGUY = 10;
const IMP = 11;
const DEMON = 12;
const SPECTRE = 13;
const CACODEMON = 14;
const BARONOFHELL = 15;
const HELLKNIGHT = 17;
const LOSTSOUL = 18;
const SPIDERMASTERMIND = 19;
const ARACHNOTRON = 20;
const CYBERDEMON = 21;
const PAINELEMENTAL = 22;
const WOLFSS = 23;
const KEEN = 24;
const BOSSBRAIN = 25;
const BOSSSPIT = 26;
const BOSSTARGET = 27;

        // missiles in air
const REVENANTMISL = 6;
const IMPSHOT = 31;
const CACOSHOT = 32;
const FLYINGROCKET = 33;
const FLYINGPLASMA = 34;
const FLYINGBFG = 35;
const ARACHPLAZ = 36;
const MANCUBUSSHOT = 9;
const BARONSHOT = 16;
const SPAWNSHOT = 28;

        // fog and splats
const PUFF = 37;
const BLOOD = 38;
const TFOG = 39;
const IFOG = 40;
const TELEPORTMAN = 41;
const EXTRABFG = 42;    // bfg flash
const VILEFIRE = 4;
const SMOKE = 7;
const SPAWNFIRE = 29;
const PARTICLE = 145;

        // health+armor
const GREENARMOR = 43;
const BLUEARMOR = 44;
const HEALTHPOTION = 45;
const ARMORHELMET = 46;
const STIMPACK = 53;
const MEDIKIT = 54;

        // keys
const BLUEKEYCARD = 47;
const REDKEYCARD = 48;
const YELLOWKEYCARD = 49;
const YELLOWSKULLKEY = 50;
const REDSKULLKEY = 51;
const BLUESKULLKEY = 52;

        // super powerups
const SUPERCHARGE = 55;
const INVULNERABILITY = 56;
const BESERKPACK = 57;
const INVISIBILITY = 58;
const RADSUIT = 59;
const AUTOMAP = 60;
const LITEAMP = 61;
const MEGASPHERE = 62;

        // ammo
const CLIP = 63;
const BULLETBOX = 64;
const ROCKET = 65;
const ROCKETBOX = 66;
const ECELL = 67;
const ECELLPACK = 68;
const SHELLS = 69;
const SHELLBOX = 70;
const BACKPACK = 71;

        // weapons
const BFG = 72;
const CHAINGUN = 73;
const CHAINSAW = 74;
const RLAUNCHER = 75;
const PLASMAGUN = 76;
const SHOTGUN = 77;
const SUPERSHOTGUN = 78;

        // scenery
const BARREL = 30;
const TALLTECHLAMP = 79;
const SHORTTECHLAMP = 80;
const FLOORLAMP = 81;
const TALLGRNPILLAR = 82;
const SHRTGRNPILLAR = 83;
const TALLREDPILLAR = 84;
const SHRTREDPILLAR = 85;
const SKULLCOLUMN = 86;
const HEARTCOLUMN = 87;
const EVILEYE = 88;
const SKULLROCK = 89;
const GRAYTREE = 90;
const TALLBLUFIRESTICK = 91;
const TALLGRNFIRESTICK = 92;
const TALLREDFIRESTICK = 93;
const SHRTBLUFIRESTICK = 94;
const SHRTGRNFIRESTICK = 95;
const SHRTREDFIRESTICK = 96;
const STALAGMITE = 97;
const TALLTECHPILLAR = 98;
const CANDLE = 99;
const CANDELABRA = 100;
const TREE = 126;
const BURNINGBARREL = 127;

        // hanging corpses
const TWITCHCORPSE1 = 101;
const TWITCHCORPSE2 = 110;
const HANGINGMAN1 = 102;
const HANGINGMAN2 = 103;
const HANGINGMAN3 = 104;
const HANGINGMAN4 = 105;
const HANGINGMAN5 = 106;
const HANGINGMAN6 = 107;
const HANGINGMAN7 = 108;
const HANGINGMAN8 = 109;
const HANGINGMAN9 = 128;
const HANGINGMAN10 = 129;
const HANGINGMAN11 = 130;
const HANGINGMAN12 = 131;
const HANGINGMAN13 = 132;
const HANGINGMAN14 = 133;

        // dead bodies
const DEADCACO = 111;
const DEADPLAYER = 112;
const DEADTROOPER = 113;
const DEADDEMON = 114;
const DEADLOSTSOUL = 115;
const DEADIMP = 116;
const DEADSERGEANT = 117;
const SLOP = 118;
const SLOP2 = 119;
const SKULLPOLE1 = 120;
const SKULLPOLE2 = 122;
const BLOODPOOL1 = 121;
const SKULLPILE = 123;
const BLOODPOOL2 = 134;
const BLOODPOOL3 = 135;
const BLOODPOOL4 = 136;

        // impaled
const DEADCORPSE1 = 124;
const TWITCHCORPSE3 = 125;

        // boom/mbf/smmu stuff
const PUSH = 137;
const PULL = 138;
const DOGS = 139;
const CAMERA = 144;

        // beta version stuff
const PLASMA1 = 140;
const PLASMA2 = 141;
const SCEPTRE = 142;
const BIBLE = 143;


/************* Thing flags ****************/

    // Call P_SpecialThing when touched.
const MF_SPECIAL          = 0;
    // Blocks.
const MF_SOLID            = 1;
    // Can be hit.
const MF_SHOOTABLE        = 2;
    // Don't use the sector links (invisible but touchable).
const MF_NOSECTOR         = 3;
    // Don't use the blocklinks (inert but displayable)
const MF_NOBLOCKMAP       = 4;

    // Not to be activated by sound, deaf monster.
const MF_AMBUSH           = 5;
    // Will try to attack right back.
const MF_JUSTHIT          = 6;
    // Will take at least one step before attacking.
const MF_JUSTATTACKED     = 7;
    // On level spawning (initial position),
    //  hang from ceiling instead of stand on floor.
const MF_SPAWNCEILING     = 8;
    // Don't apply gravity (every tic),
    //  that is, object will float, keeping current height
    //  or changing it actively.
const MF_NOGRAVITY        = 9;

    // Movement flags.
    // This allows jumps from high places.
const MF_DROPOFF          = 10;
    // For players, will pick up items.
const MF_PICKUP           = 11;
    // Player cheat. ???
const MF_NOCLIP           = 12;
    // Player: keep info about sliding along walls.
const MF_SLIDE            = 13;
    // Allow moves to any height, no gravity.
    // For active floaters, e.g. cacodemons, pain elementals.
const MF_FLOAT            = 14;
    // Don't cross lines
    //   ??? or look at heights on teleport.
const MF_TELEPORT         = 15;
    // Don't hit same species, explode on block.
    // Player missiles as well as fireballs of various kinds.
const MF_MISSILE          = 16;
    // Dropped by a demon, not level spawned.
    // E.g. ammo clips dropped by dying former humans.
const MF_DROPPED          = 17;
    // Use fuzzy draw (shadow demons or spectres),
    //  temporary player invisibility powerup.
const MF_SHADOW           = 18;
    // Flag: don't bleed when shot (use puff),
    //  barrels and shootable furniture shall not bleed.
const MF_NOBLOOD          = 19;
    // Don't stop moving halfway off a step,
    //  that is, have dead bodies slide down all the way.
const MF_CORPSE           = 20;
    // Floating to a height for a move, ???
    //  don't auto float to target's height.
const MF_INFLOAT          = 21;

    // On kill, count this enemy object
    //  towards intermission kill total.
    // Happy gathering.
const MF_COUNTKILL        = 22;
    
    // On picking up, count this item object
    //  towards intermission item total.
const MF_COUNTITEM        = 23;

    // Special handling: skull in flight.
    // Neither a cacodemon nor a missile.
const MF_SKULLFLY         = 24;

    // Don't spawn this object
    //  in death match mode (e.g. key cards).
const MF_NOTDMATCH        = 25;

    // Player sprites in multiplayer modes are modified
    //  using an internal color lookup table for re-indexing.
    // If 0x4 0x8 or 0xc,
    //  use a translation table for player colormaps
const MF_TRANSLATION      = 26;


        // mbf/boom
const MF_TOUCHY  = 28;       // dies when solids touch it
const MF_BOUNCES = 29;       // for beta BFG fireballs
const MF_FRIEND =  30;       // friendly monsters

    // Translucent sprite?
const MF_TRANSLUCENT =  31;

