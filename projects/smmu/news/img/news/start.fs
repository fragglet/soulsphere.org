[level info]

; a demonstration of some of the things possible..

levelname = Start Map: Entrance to doom
partime=50
gravity=85000
music=start
skyname=SKY3
creator=simon howard 'fraggle'
interpic=BOSSBACK
nextlevel=MAP01
defaultweapons = 1234

[scripts]

// include("fs-thing");

int times_run = 0;

script 0
{
        tip("\"i'm too young to die\" skill level \a");

        times_run++;

        wait(35);
        tip("this script has been run ", times_run, " times");
}

script 1
{
        tip("\"hey, not too rough\" skill level\a");
}

script 2
{
        tip("\"Hurt me plenty\" skill level \a");
}

script 3
{
        tip("\"Ultra-violence!\" skill level \a");
}

script 4
{
        tip("NIGHTMARE skill level! \a");
}

script 5
{
        tip( "Walk through the teleport to start doom.. \a");
}

const num_beeps = 10;

script 6
{
        int i;

        tip("this does not work!!\a");

        for(i=0, i<num_beeps, i++)
        {
                wait(2);
                beep();
        }
}

script 7
{
        int i;
        string s1, s2, s3;

        i = rnd() % 5;
        if(i == 0) s1 = "i is ere wiv ";
        if(i == 1) s1 = "respect to ";
        if(i == 2) s1 = "check ";
        if(i == 3) s1 = "for real: it is ";
        if(i == 4) s1 = "ride ";

        i = rnd() % 4;
        if(i == 0) s2 = "dis ";
        if(i == 1) s2 = "the wicked ";
        if(i == 2) s2 = "de ";
        if(i == 3) s2 = "me ";

        i = rnd() % 6;
        if(i == 0) s3 = "maiin man";
        if(i == 1) s3 = "punani";
        if(i == 2) s3 = "killion";
        if(i == 3) s3 = "west siiide";
        if(i == 4) s3 = "health service";
        if(i == 5) s3 = "wales";

        tip(s1, s2, s3);
}

script 8
{
        while(1)
        {
                message(trigger().objx, ", ", trigger().objy, "\n");
                wait(15);
        }
}

const BABY = 20;

script 9
{
        int arach;

        setcamera(-25, -63, 142);
        arach = spawn(BABY,-192,63);
        tip("oh no, an arachnotron!");
        wait(100);

        arach.kill();
        tip("bang");
        wait(70);

        arach.removeobj();
        tip("bye bye!");
        wait(70);

        clearcamera();
}

script 10
{
        trigger().silentteleport(10);   // teleport back to start
}

script 11
{
        consoleplayer.playerobj.kill();
}

consoleplayer.playertip("welcome to SMMU, ", consoleplayer.playername());
