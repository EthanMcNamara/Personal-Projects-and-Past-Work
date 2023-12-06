# Invevo Tech Test
This test was taken as part of my application as a Junior Software Engineer at Invevo. \
The premise of this test was to simulate a simple game. In the game, the player stands on one of a series of tiles - numbered 0 to n - and is able to jump to the left or to the right. Whenever a player jumps, they move to the second available tile in that direction, and the tile they were stood on is removed. \
Additionally, when the `position()` function was called, the game would return the tile the player was currently stood on. \
The game is initiated by specifying the number of tiles as well as the player's initial position.\
The skeleton for the class and the `Platformer()`, `jumpLeft()`, `jumpRight()` and `position()` methods were given, and I was expected to implement the functionality of these methods.

To further assist, the following example was given: \
For a game with 6 tiles and starting position 3, the game would start like so:
```
{0, 1, 2, [3], 4, 5}
```
If the player were to jump left, the game would update to the following:
```
{0, [1], 2, 4, 5}
```
If the player were to then jump right, the game would update to:
```
{0, 2, [4], 5}
```

## Test Criteria
The test checked for three criteria:
- That the game worked with the example given in the test's overview. 
- That the game worked with a larger array of 10 tiles.
- That the code would be optimised to run in some arbitrarily large dataset within a certain time limit.

## Implementations
### `Platformer.java`
This was my original submission for the test. I initially started by using an ArrayList, but after not meeting the optimisation criteria decided to switch to a standard array, knowing that the array would not grow in size, and that an array is typically faster than Java'a ArrayList.\
This method led to an equally large (if not larger) overhead, as instead of a simple change of increasing/decreasing the position counter, I now had to search for the next "valid" spaces, meaning for an array of size `n`, the search could take as many as `n` iterations when there are only two valid tiles. While it did not meet the required time for the optimisation criteria, I ended up submitting this version of the program due to time limits.

### `PlatformerLinked.java`
Following my test, I had another interview with Invevo where I was able to discuss the tech test amongst other topics. There I was made aware of the ideal "optimised" solution, which was to use a LinkedList due to its much faster speeds when inserting/removing values compared to the ArrayList.\
This version was produced after the interview to show the ideal implementation of the game.
