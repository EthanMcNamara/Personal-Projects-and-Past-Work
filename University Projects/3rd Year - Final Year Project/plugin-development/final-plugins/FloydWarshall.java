import gralog.algorithm.*;

import gralog.progresshandler.ProgressHandler;
import gralog.structure.Edge;
import gralog.structure.Structure;
import gralog.structure.Vertex;
import gralog.structure.UndirectedGraph;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashSet;
import java.util.Set;

/**
 *
 */
@AlgorithmDescription(
        name = "Floyd-Warshall Shortest Path",
        text = "Finds the shortest path between any two Vertices",
        url = "https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm"
)
public class FloydWarshall extends Algorithm {
    private static ArrayList<HashSet<Object>> stateList = new ArrayList<HashSet<Object>>();
    private static ArrayList<String> stateAnnotations = new ArrayList<String>();
    private static Object visData[] = new Object[2];

    private static boolean visualize = false;
    private static boolean negativeCycle = false;

    public static Double floydWarshallShortestPath(Structure s, Vertex start, Vertex target, HashSet<Object> pathEdges) {
        // Based on Floyd-Warshall and Path Reconstruction implementation from https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm#Path_reconstruction
        // This in turn was based on "Algorithms Notes for Professionals" - https://books.goalkicker.com/AlgorithmsBook/

        negativeCycle = false; // Used to check if a negative cycle is present after main loop

        int vertexCount = s.getVertices().size();

        List<List<Double>> distanceMatrix = new ArrayList<List<Double>>(); // Used to find shortest distance
        for(int i=0; i<vertexCount; i++) { // Create an n x n matrix 
            List<Double> distanceFromVertex = new ArrayList<Double>();
            for(int j=0; j<vertexCount; j++) {
                distanceFromVertex.add(Double.POSITIVE_INFINITY); // Initialize all distances as infinity
            }

            distanceMatrix.add(distanceFromVertex);
        }

        List<List<Vertex>> prevVertMatrix = new ArrayList<List<Vertex>>(); // Used for path reconstruction
        for(int i=0; i<vertexCount; i++) { // Create an n x n matrix 
            List<Vertex> prevVertex = new ArrayList<Vertex>();
            for(int j=0; j<vertexCount; j++) {
                prevVertex.add(null); // Initialize all Vertices as null
            }

            prevVertMatrix.add(prevVertex);
        }

        List<List<Edge>> edgeMatrix = new ArrayList<List<Edge>>(); // Used for path reconstruction
        for(int i=0; i<vertexCount; i++) { // Create an n x n matrix
            List<Edge> edgesFromVertex = new ArrayList<Edge>();
            for(int j=0; j<s.getVertices().size(); j++) {
                edgesFromVertex.add(null); // Initialize all edges as null
            }

            edgeMatrix.add(edgesFromVertex);
        }


        ArrayList<Edge> edges = new ArrayList<Edge>();
        edges.addAll(s.getEdges());
        edges.sort((o1, o2) -> o2.weight.compareTo(o1.weight)); // Order the edges from highest weight to lowest, meaning lowest basic edge is always chosen
        for(Edge e : edges) { // Assign the initial distances between Vertices with edges directly connecting them
            Vertex u = e.getSource();
            Vertex v = e.getTarget();

            distanceMatrix.get(u.getId()).set(v.getId(), e.weight); // delta^0_(u,v) = uv for every edge uv in Edges
            prevVertMatrix.get(u.getId()).set(v.getId(), u); // To build path from Vertex u to v, go through Vertex u
            edgeMatrix.get(u.getId()).set(v.getId(), e); // Use edge uv for path from Vertex u to v

            if(!e.isDirected) { // Also apply the reverse if the edge is undirected
                distanceMatrix.get(v.getId()).set(u.getId(), e.weight); // delta^0_(u,v) = uv for every edge uv in Edges
                prevVertMatrix.get(v.getId()).set(u.getId(), v); // To build path from Vertex v to u, go through Vertex v
                edgeMatrix.get(v.getId()).set(u.getId(), e); // Use edge vu for path from Vertex v to u
            }
        }

        ArrayList<Vertex> vertices = new ArrayList<Vertex>();
        vertices.addAll(s.getVertices());
        for(Vertex v : vertices) { // Assign the distances between a Vertex and itself as 0
            distanceMatrix.get(v.getId()).set(v.getId(), 0.0); // delta^0_(v,v) = 0 for every Vertex v in Vertices
            prevVertMatrix.get(v.getId()).set(v.getId(), v); // To build path from Vertex v to v, remain at v.
        }

        int changeCount = 0;

        // Instead of copying and creating a new matrix each time, iterate over the same matrix k times
        for(int k=0; k<vertexCount; k++) {
            String currentAnnotation = "k="+k+". Any Distances directly connected to the highlighted Vertex will remain unchanged.\n"+
            "If the Distance i-j is greater than the Distances i-k + k-j, then the former value is replaced with the latter sum";
            String annotationConcat = "";

            for(int i=0; i<vertexCount; i++) {
                for(int j=0; j<vertexCount; j++) {
                    Double distIJ = distanceMatrix.get(i).get(j); // Distance from i to j
                    Double distIK = distanceMatrix.get(i).get(k); // Distance from i to k
                    Double distKJ = distanceMatrix.get(k).get(j); // Distance from j to k

                    if(distIJ > distIK + distKJ) { // if delta^(n-1)_(i,j) > delta^(n-1)_(i,k) + delta^(n-1)_(k,j)
                        String distChange1 = "\nDistance "+i+"-"+j+" changed from "+distanceMatrix.get(i).get(j)+" to "+
                            distanceMatrix.get(i).get(k)+" + "+distanceMatrix.get(k).get(j)+" = ";

                        distanceMatrix.get(i).set(j, distIK + distKJ); // then delta^(n)_(i,j) = delta^(n-1)_(i,k) + delta^(n-1)_(k,j)
                        prevVertMatrix.get(i).set(j, prevVertMatrix.get(k).get(j)); // To build a path from Vertex i to j, go through Vertex at k-j in matrix

                        String distChange2 = ""+distanceMatrix.get(i).get(j)+"\n\t("+i+"-"+k+" + "+k+"-"+j+")";

                        annotationConcat = annotationConcat.concat(distChange1);
                        annotationConcat = annotationConcat.concat(distChange2);

                        changeCount++;
                    }
                }
            }

            // For each iteration, highlight the Vertex who's connections stay the same and annotate the other changes
            if(visualize) {
                HashSet<Object> currentVertex = new HashSet<>();
                currentVertex.add(vertices.get(k));
                stateList.add(currentVertex);
                if(changeCount == 0) {
                    annotationConcat = "\nNo Distances changed.";
                }

                if(k == vertexCount - 1) {
                    currentAnnotation = currentAnnotation.concat("\nThe shortest Distance between every Vertex has now been found.");
                }

                currentAnnotation = currentAnnotation.concat(annotationConcat);

                stateAnnotations.add(currentAnnotation);
            }

            changeCount = 0;
        }

        // Inspired by check from https://stackoverflow.com/questions/15709277/floyd-warshall-with-negative-cycles-how-do-i-find-all-undefined-paths#:~:text=If%20two%20nodes%20cannot%20reach,are%20in%20a%20negative%20cycle.
        // A negative cycle is present if the path from a Vertex to itself is less than 0 
        for(int i=0; i<vertexCount; i++) {
            if(distanceMatrix.get(i).get(i) < 0) {
                negativeCycle = true;
                return Double.POSITIVE_INFINITY;
            }
        }

        HashSet<Object> path = pathReconstruction(start, target, prevVertMatrix, edgeMatrix);

        if(path == null) { // Return infinity if there is no path
            return Double.POSITIVE_INFINITY;
        }

        if(visualize) { // Finally, highlight the shorest path and add its distance to the annotations
            stateList.add(path);
            stateAnnotations.add("The shortest Distance "+start.getId()+"-"+target.getId()+" is "+
                distanceMatrix.get(start.getId()).get(target.getId()));
        }

        pathEdges.addAll(path);

        if(visualize) { // Add additional details to the first annotation and penultimate annotation
            String firstAnnotation = stateAnnotations.get(0);
            firstAnnotation = "Initial distances are taken as the weight of the Edges connecting the two Vertices. "+
                "If no such Edge exists, the Distance is Infinity\n"+
                "Distances are noted as i-j where i and j are the Vertices' ids.\n".concat(firstAnnotation);
            stateAnnotations.set(0, firstAnnotation);

            String finalAnnotation = stateAnnotations.get(stateAnnotations.size() - 2);
            finalAnnotation = finalAnnotation.concat("\nThe shortest distance between every Vertex has now been found.");
            stateAnnotations.set(stateAnnotations.size() - 2, finalAnnotation);
        }

        return distanceMatrix.get(start.getId()).get(target.getId());
    }

    public static HashSet<Object> pathReconstruction(Vertex start, Vertex target, List<List<Vertex>> prevVertMatrix, List<List<Edge>> edgeMatrix) {
        Vertex u = start;
        Vertex v = target;

        if(prevVertMatrix.get(u.getId()).get(v.getId()) == null) { // No path exists, return null
            return null;
        }

        ArrayList<Integer> path = new ArrayList<Integer>();
        path.add(v.getId());
        // For Vertex i = prevVertMatrix[u][v], prepend i to the path and find prevVertMatrix[u][i] until i == u 
        while(!u.equals(v)) {
            v = prevVertMatrix.get(u.getId()).get(v.getId());
            path.add(0, v.getId());
        }
        
        HashSet<Object> pathEdges = new HashSet<>();

        for(int i=0; i<path.size()-1; i++) { // For every Vertex in the path
            // Get the edge connecting every Vertex pair
            Edge e = edgeMatrix.get(path.get(i)).get(path.get(i+1));

            // Put the edge into pathEdges
            pathEdges.add(e);
        }

        return pathEdges;
    }

    public Object runVis(Structure s, AlgorithmParameters p, Set<Object> selection, ProgressHandler onprogress) throws Exception {
        // Algorithm that specifies how to run differently when called to visualize
        visualize = true;
        return run(s, p , selection, onprogress);
    }

    public Object run(Structure s, AlgorithmParameters p, Set<Object> selection, ProgressHandler onprogress) throws Exception {
        if (s.getVertices().size() == 0) // Check the structure is empty, return an error if true
            return ("The structure should not be empty.");

        int vertexCount = 0;
        Vertex start = null;
        Vertex target = null;

        for(Object o : selection) { // Count number of selected Vertices
            if(o instanceof Vertex && vertexCount == 0) {
                start = (Vertex) o;
                vertexCount++;
            } else if(o instanceof Vertex) {
                target = (Vertex) o;
                vertexCount++;
            }
        }

        if(vertexCount != 2) // If the number of selected Vertices is not two, return an error
            return ("Please select exactly two Vertices to find the shortest path between\n"+
            "(shift+click to select multiple Vertices)");

        if(visualize) { // Clear data that may have been provided from previous Visualizations
            stateList.clear();
            stateAnnotations.clear();
        }
        
        HashSet<Object> pathEdges = new HashSet<>();

        Double shortestDistance = FloydWarshall.floydWarshallShortestPath(s, start, target, pathEdges);

        if(negativeCycle) // If a negative cycle was detected, return an error
            return("Floyd-Warshall cannot be executed as there is a Negative Cycle in the graph.");

        // If distance is infinity, no path exists. User possibly selected start/target in incorrect order, prompt them to select start first followed by target
        if(shortestDistance == Double.POSITIVE_INFINITY)
            return ("No shortest path exists.\n"+
            "Make sure to select the 'start' Vertex first, followed by the 'target' Vertex");

        // Label every Vertex as its id for annotation purposes
        HashSet<Vertex> vertices = new HashSet<Vertex>();
        vertices.addAll(s.getVertices());
        for(Vertex v : vertices)
            v.label = ""+v.getId();

        for(Edge e : (Set<Edge>) s.getEdges())
            e.label = ""+e.weight;

        if(visualize) {
            visualize = false;

            visData[0] = stateList;
            visData[1] = stateAnnotations;

            return visData;
        } else {
            return pathEdges;
        }
        
    }
}
