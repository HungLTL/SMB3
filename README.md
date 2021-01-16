Poorly-implemented remake of one of the best platforming games ever made by an idiot who has absolutely no prior experience to game development and advanced C++. The owner of this repository should be ashamed.

UPDATE v1.2:
- Mario functions fleshed out: Now has the ability to carry koopa shells, charge up jumps and sprints, can now fly properly, and a tail that can identify overlaps instead of collisions
- Added paragoombas, koopa paratroopas and piranha plants
- CPipe: Certain pipes can now warp to the World 1-1 Secret Zone
- Added power ups, coins, the P-Switch and the End-of-World Roulette
- Added splash screen and overworld map
- Added HUD

UPDATE v1.2.1
- Added grid class, used to determine which objects in a scene gets to be updated
- Removed bool active attribute in most classes that was used to determined whether to render them or not; replaced with a function that directly erases an object from the objects vector

UPDATE v1.2.2
- Camera now has basic Y-axis tracking
- Completing World 1-1 will retain the previous form on the overworld map and will also set Mario's icon over the World 1-1 square

UPDATE v1.3
- Added World 1-4
- Added coin blocks (with specified use limit), boomerang bro, air lifts
- Added a koopa paratroopa variant that moves only on the y-axis
- Grid now contains lists of objects and background objects - all level files changed to reflect this
- Moving objects will now be automatically removed if they're no longer in a grid
- CMario: Added a proper tail flapping animation (when sprint < max charge) and "flying" animations for other forms
- CCoin: Now uses the spinning animation (except for those generated via the P-Switch)
- CPowerUp: Added Fire Flower
- CPowerUp, CCoin: No longer pre-added; activating a power block creates the bonus item instead
- CGoldBlock, CPowerBlock: Now bounces when activated
- CKoopa: When dormant, will change into a "warning" animation at the last 2 seconds

Controls:

AD: move left/right
S: crouch (game scene, non-normal forms)/move down (overworld map)
W: warp up (game scene, if possible)/move up (overworld map)
I: start game at splash screen
J: attack(fire or raccoon)/sprint/carry
K: jump/fly/select level
