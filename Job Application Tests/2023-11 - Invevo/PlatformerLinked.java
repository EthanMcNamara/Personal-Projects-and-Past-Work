/*
 * An alternate version of Platformer.java implemented following my interview at Invevo.
 * In the interview it was mentioned that a LinkedList would be a more efficient data structure to use, and this is the implementation of that feedback.
 * This also uses a larger test array, but works with the original test data found in Platformer.java
 */

import java.util.LinkedList;

public class PlatformerLinked {
    private int position;
    private LinkedList<Integer> tiles;

    public PlatformerLinked(int n, int position) {
        this.position = position;

        tiles = new LinkedList<Integer>();
        for (int i=0; i<n; i++)
            tiles.add(i);
    }

    public void jumpLeft() {
        int currPosition = position;
        position -= 2;
        tiles.remove(currPosition);
    }

    public void jumpRight() {
        int currPosition = position;
        position++;
        tiles.remove(currPosition);
    }

    public int position() {
        return tiles.get(position);
    }

    public static void main(String[] args) {
        PlatformerLinked platformer = new PlatformerLinked(10, 3);
        System.out.println(platformer.position()); // should print 3

        platformer.jumpLeft();
        System.out.println(platformer.position()); // should print 1

        platformer.jumpRight();
        System.out.println(platformer.position()); // should print 4

        platformer.jumpRight();
        System.out.println(platformer.position()); // should print 6

        platformer.jumpLeft();
        System.out.println(platformer.position()); // should print 2

        System.out.println(platformer.tiles.toString());
        // Should be [0, 2, 5, 7, 8, 9]
    }

}
