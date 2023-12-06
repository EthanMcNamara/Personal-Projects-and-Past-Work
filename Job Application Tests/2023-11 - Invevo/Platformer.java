public class Platformer {
    private int n;
    private int position;
    private int tiles[];

    public Platformer(int n, int position) {
        this.n = n;
        this.position = position;

        tiles = new int[n];
        for (int i=0; i<n; i++)
            tiles[i] = i;
    }

    public void jumpLeft() {
        int currPosition = position;
        int posCount = 0; // Counts number of positive integers
        for(int i=position-1; i>-1; i--) {
            if(tiles[i] != -1)
                posCount += 1;
            
            if(posCount == 2) {
                position = i;
                break;
            }
        }

        tiles[currPosition] = -1;
    }

    public void jumpRight() {
        int currPosition = position;
        int posCount = 0; // Counts number of positive integers
        for(int i=position+1; i<n; i++) {
            if(tiles[i] != -1)
                posCount += 1;
            
            if(posCount == 2) {
                position = i;
                break;
            }
        }

        tiles[currPosition] = -1;
    }

    public int position() {
        return tiles[position];
    }

    public static void main(String[] args) {
        Platformer platformer = new Platformer(6, 3);
        System.out.println(platformer.position()); // should print 3

        platformer.jumpLeft();
        System.out.println(platformer.position()); // should print 1

        platformer.jumpRight();
        System.out.println(platformer.position()); // should print 4
    }

}