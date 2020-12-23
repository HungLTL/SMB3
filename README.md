Poorly-implemented remake of one of the best platforming games ever made by an idiot who has absolutely no prior experience to game development and advanced C++. The owner of this repository should be ashamed.

UPDATE v1.2:
- Mario functions fleshed out: Now has the ability to carry koopa shells, charge up jumps and sprints, can now fly properly, and a tail that can identify overlaps instead of collisions
- Added paragoombas, koopa paratroopas and piranha plants
- Certain pipes can now warp to the World 1-1 Secret Zone
- Added power ups, coins, the P-Switch and the End-of-World Roulette
- Added splash screen and overworld map
- Added HUD

UPDATE v1.2.1
- Added grid class, used to determine which objects in a scene gets to be updated
- Removed bool active attribute in most classes that was used to determined whether to render them or not; replaced with a function that directly erases an object from the objects vector

Controls:

AD: move left/right
S: crouch (game scene, non-normal forms)/move down (overworld map)
W: warp up (game scene, if possible)/move up (overworld map)
I: start game at splash screen
J: attack(fire or raccoon)/sprint/carry
K: jump/fly/select level
