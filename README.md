# Kontagion

Source code contained in: `Kontagion/`

The highlights of my implementation are in the following files:

- `Actor.cpp` and `Actor.h`: Control all the actors in the game which is almost everything visible on the screen, including the player, the bacteria, the dirt piles, the goodies, the sprays, the flamethrowers, etc.
- `StudentWorld.cpp` and `StudentWorld.h`: Control the game itself, i.e. whether the game has ended, which actors are present in the game, adding and removing actors, moving actors, cleaning up memory after the game has ended, etc.

Kontagion is an action game implemented in C++.

This implementation will only work on macOS. It requires that XQuartz is installed.

WARNING: This game is large in size ~103 MB, so make sure you can download it and have enough storage space before attempting to run it.

### Steps for installing and running

1. Install [XQuartz](https://www.xquartz.org/)
2. Clone this repository using

   ```
   $ git clone https://github.com/ksj1602/kontagion.git
   ```
   
   This will download all the code and game files to your current directory.
   
3. Open the `Kontagion.xcodeproj` file using Xcode
4. Build and run the game from Xcode
5. Enjoy!

### How to Play

The game will start with Socrates on the edge of a petri dish. There will be one bacterial pit and several dirt piles randomly scattered. There will also be pieces of pizza which act as food for the bacteria.

Your goal is to eliminate as many bacteria as you can, within the 3 lives that you have. Bacteria can be attacked using sprays or flamethrower charges.

You have an unlimited quantity of sprays but only 5 flamethrower charges. Sprays travel a longer distance but cause lesser damage and only travel in the direction Socrates is pointing towards. Flamethrower charges cause much more damage and are fired in a circle in all directions, but they travel a much lesser distance than sprays.

There are 3 types of bacteria in this game:

1. Regular Salmonella : Their main goal is to eat food and multiply. Upon eating 3 pieces of food, will split into more. Move randomly in any direction and only cause damage if they're right next to Socrates.
2. Aggressive Salmonella : Same as Regular Salmonella but if they are within a certain radius of Socrates, will try to attack him.
3. Ecoli : Actively try to attack Socrates no matter where they are on the dish. Food is secondary to these lethal bacteria.

There are also "Goodies" in the game which appear randomly, both timing and position wise.

1. Fungus : The only "negative" goodie. Cause damage to Socrates if you make contact with them, then disappear.
2. Health Goodie : Restores Socrates' health to full
3. Flamethrower Goodie : Supplies 5 flamethrower charges
4. Extra life Goodie : Grants an extra life to the player

Eliminating bacteria and/or picking up goodies grant points to the player.   Dieing and/or being hit by the fungus cause a loss in points.

These are the game controls:

`A`     : Move Socrates counterclockwise  
`D`     : Move Socrates clockwise  
`Space` : Fire spray  
`Return`: Fire flamethrower charges  
`Q`     : Quit
