# gameframework2d (GF2D)
a collection of utlitity functions designed to facilitate creating 2D games with SDL2
This project is specifically intended to function as an educational tool for my students taking 2D Game Programming.

Currently the project is in a WIP state, however it is functional with sample projects showcasing rudimentary scrolling shooters,
platformers, and adventure games.

The master branch is kept deliberately minimal to allow students to build out systems as they are learning.
Other branches have more complete system: Collisions, menus, input abstraction.

# Build Process

Before you can build the example code we are providing for you, you will need to obtain the libraries required
by the source code
 - SDL2
 - SDL2_image
 - SDL2_mixer
 - SDL2_ttf
There are additional sub modules that are needed for this project to work as well, but they can be pulled right from within the project.
Performable from the following steps from the root of the cloned git repository within a terminal. 

Make sure you fetch submodules: `git submodule update --init --recursive`
Go into each submodule's src directory and type:
`make`
`make static`

Once each submodule has been made you can go into the base project src folder anre simply type:
`make`

You should now have a `gf2d` binary within the root of your git repository. Executing this will start your game.

# Knockback Game

## How to play:

Use WASD to move, walls will stop you, pits will kill you, enemies move and will kill you
Move your mouse around to aim your attacks.
After picking up a sword, left click to swing it.  If you picked up the upgrade and have full health, you will also shoot a beam.
After picking up the bomb, right click to drop it.
### Midterm Deliverables:
#### Common:
1. Functioning Entity System: Everything that interacts with the player or the world is an entity.
2. Fully Interactable Environment:  Walls stop players, enemies harm player.
3. User Interface: Hearts in the lower-left show the player's current health.
4. Read / Write Access: Collected Upgrades stick around after reloads.
5. README: This document
#### Personal:
1. Top down puzzle world: The basic format of the world.
2. 5 dungeon features/puzzles: Walls, pits, spikes, moving walls, lasers, and locked doors.
3. 5 different enemy behaviors: Robots (move in one direction), Drones (move in one direction, can move over some obstacles), Automata (moves in one direction, occasionally stops), Sweeper (changes direction), Orb (move in one direction, properties change).
4. Upgradable Player (5 ways): Health Upgrade (go up one heart), Sword Upgrade (unlock attack), Sword Beam (at full health shoot sword beam), Bomb Upgrade (drop bombs), Key Upgrade (can now unlock doors).
5. 5 pickups: Health Potion 1 (heal one heart, can be found in room 3), Health Potion 2 (heal 2 hearts, can be found in room 6), Health Potion 3 (heal all hearts, can be found in room 1), Invincibility 1 (become invincible for a short time, can be found in room 2), Invincibility 2 (become invincible for a longer time, can be found in room 3).

### Final Deliverables:
#### Common:
1. Code Compiles Clean: Build the project and see there are no errors.
2. Steady Frame Rate of Animation: No frame slowdowns in the game.
3. Audio Implementation: Background music that changes and some sound effects can be heard.
4. Menu System: Move around menus with wasd and select options with enter or clicks.
5. Research Component: Particles activated partway through boss.  Particle system made more efficient and can handle many more than initailly described.  There is also a radial option for particles now and particles can emit from a moving source.
6. Seamless Level Transitions: Can find them throughout the game.
7. First few levels: Level up to boss fight is created.
8. All Assets should agree: Almost all assets made by me, if they don't agree that's due to my lack of skills not lack of thought.
9. Content Editor: Level editor accessible through the main menu. 
#### Personal:
1. Save points / Multiple Saves: Save points throughout levels, click to activate and save.  Main menu allows selection of save file.
2. NPC Dialog: In certain areas dialog/cutscenes will begin and conversation begins.
3. Big Elaborate Boss Fight: At the end of the level there is a big boss fight.