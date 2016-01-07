
[scripts]
// Random enemy spawner, random health spawner
// (c) Copyright 1999 Michael Reid

include("things.h");  // don't forget your includes, they are crucial as I
                      // just found out =)

script 80
    {
        int i, enemytype;  // variables

        i = rnd() % 5;  // find the remainder of a random number divided by 5
        if(i==0) enemytype = BARONOFHELL;
        if(i==1) enemytype = CACODEMON;
        if(i==2) enemytype = ARACHNOTRON;
        if(i==3) enemytype = REVENANT;
        if(i==4) enemytype = PAINELEMENTAL;

        spawn(enemytype, -128, 0); // spawn enemy at coordinates
            // (-128, 0) in the map
    }

script 81
    {
        int i; // no int healthtype due to the direct spawning

        i = rnd(); // keep variable 'i' as a constant random number
        if(i % 10 == 0)
            {
                spawn(SUPERCHARGE, 128, 0);
                return();
            }
            // 10% of the time, a soul sphere will be spawned
        if(i % 2 == 0)
            {
                spawn(MEDIKIT, 128, 0);
                return();
            }
            // 40% of the time (50% divisible by 2 minus the 10% divisible by
            // 10) a medikit will be spawned
        spawn(STIMPACK, 128, 0);
        // if all else fails, stimpacks will prevail =)
    }
