This all started maybe two years ago when I first picked up c++ and wanted to make a game. I had this idea of a game like terraria, but every block was a pixel. I also wanted to build as much stuff as possible from scratch, so no using unity or something. I thought it would be fun and help me learn.

attempt 1:
I discovered SDL and tried to use that. I stored my world in big vectors and drew everything using some paint pixel function. It was really slow.
Then I started directly accessing the framebuffer and putting pixels there. After rewriting some code because it was still slow, it ran at 60 fps which was my target.
I wrote some simple terrain generation too. I got as far as making explosions and bombs that make them (this part was fun).

see attempt-1 branch for some gifs

attempt 2:
One day I was just on the internet when I heard about a game called Noita that just went into early access. I looked at the trailer and it blew my mind. They had a similar idea where every "block" is really small, but the key difference was that they had physics on each and every particle.
I was instantly hooked on that idea nd started researching it. I found some tech talks by the devs, and turns out they were using falling sand physics. This was like The Powder Toy, another game I played a ton when I was a kid.
Being overly ambitious, of course I had to try recreating what a professional game dev team did in several years.
For some reason, I also decided at the same time to learn some opengl. I wanted to move away from drawing pixels one by one in the framebuffer and start using shaders. At this point, using opengl made basically no difference.
Anyways, I just couldn't get the physics to feel right.

see attemp-2 branch for a gif

attempt 3 (current):
I decided that it wasn't viable for me to keep trying with the falling sand idea. I wanted to learn to use more of opengl too, when currently I was only using it to draw different colored pixels. Not really what it was meant to do.
So I turned to Liquid Fun (an extension of box2d) and started from scratch. I slowly learned how to set up opengl, write simple shaders, and use box2d.I stopped working on the game during the summer before my freshman year of college, but before that, I had a basic engine going. I had been playing with the water (which is so satisyfing), but also trying to get movement feeling right.
I still have plans to merge the pixel world idea in though (when I get back into working on this game), again borrowing ideas from Noita.

i'm just dumping projects to github right now

12/20/20
