#!/usr/bin/env python

import random
import sys

firstword = [
"abhorrent", "abysmal", "agonizing", "almighty", "appalling",
"atrocious", "awful", "awfully", "bloodcurdling", "bloodthirsty",
"broken", "cadaverous", "caustic", "charnel", "creepy",
"crucified", "cumberous", "deadly", "defeated", "desolate",
"devastating", "dire", "direful", "dismal", "disturbed",
"dreadful", "dreadfully", "fearful", "fearsome", "frightful",
"fulsome", "ghastful", "ghastly", "gory", "grisly",
"gruesome", "haggard", "hideous", "horrendous", "horrible",
"horrid", "horrific", "immortal", "macabre", "moribund",
"mortal", "murderous", "nauseating", "ogreish", "outrageous",
"perishing", "reanimated", "redoubtable", "repelling", "repulsive",
"sanguinary", "scary", "slaughterous", "sombrous", "terrible",
"terribly", "terrific", "terrifying", "terror", "tormented",
"tremendous", "tremendously", "ugly", "unholy", "vile",
"warped", "withering",
]

secondword = [
"academy", "acapulco", "bath", "bilge", "castillo",
"castle", "cauldron", "chamber", "colony", "columbarium",
"conflux", "crematorium", "crusader", "crypt", "dedeman",
"demesne", "dimension", "dome", "dungeon", "factory",
"flock", "fortress", "galaxy", "garden", "graveyard",
"grove", "haven", "hotel", "house", "inferno",
"inn", "legion", "liman", "limbo", "malpas",
"medieval", "menagerie", "mercure", "morgue", "mosque",
"museum", "nature", "necropolis", "nest", "neutral",
"nightclub", "oscar", "ossuary", "other", "palace",
"park", "pit", "rampart", "religious", "restaurant",
"rocks", "rook", "salamis", "sanctuary", "savoy",
"sephulchre", "shaft", "square", "steeple", "stronghold",
"sylvan", "tabernacle", "tenement", "tomb", "torre",
"tour", "tower", "tumulus", "vault", "viva",
"warehouse", "zeus", "zodiac",
]

seed = int(sys.argv[1])
r = random.Random(seed)

w1 = firstword[int(r.random() * len(firstword))]
w2 = secondword[int(r.random() * len(secondword))]

print "The %s %s" % (w1.capitalize(), w2.capitalize())

