import gralog.algorithm.*;

import gralog.progresshandler.ProgressHandler;
import gralog.structure.Edge;
import gralog.structure.Structure;
import gralog.structure.Vertex;
import gralog.structure.UndirectedGraph;

import java.util.ArrayList;
import java.util.Map;
import java.util.HashSet;
import java.util.Set;

/**
 *
 */
@AlgorithmDescription(
        name = "Minimum Spanning Tree (Kruskal's)",
        text = "Creates a Minimum Spanning Tree starting from the lowest weighted Edge",
        url = "https://en.wikipedia.org/wiki/Kruskal%27s_algorithm"
)
public class KruskalMST extends Algorithm {
    private static ArrayList<HashSet<Object>> stateList = new ArrayList<HashSet<Object>>();
    private static ArrayList<String> stateAnnotations = new ArrayList<String>();
    private static Object visData[] = new Object[2];
    
    private static boolean completeTree = true;
    private static boolean visualize = false;
    
    public static void kruskalMinSpanTree(Structure s, HashSet<Object> selectedEdges) {
        // Based on Pseudocode Implementation from https://www.programiz.com/dsa/kruskal-algorithm

        // Create ordered list of all edges from lowest to highest weight
        ArrayList<Edge> edges = new ArrayList<Edge>();
        edges.addAll(s.getEdges());
        edges.sort((o1, o2) -> o1.weight.compareTo(o2.weight)); // Based on https://www.geeksforgeeks.org/how-to-sort-an-arraylist-of-objects-by-property-in-java/

        ArrayList<Vertex> vertices = new ArrayList<Vertex>();
        vertices.addAll(s.getVertices());

        HashSet<Vertex> setS = new HashSet<>(); // Set of Vertices in the tree
        HashSet<Edge> setF = new HashSet<>(); // Set of edges in the tree

        completeTree = true;

        HashSet<HashSet<Vertex>> trees = new HashSet<>(); // Set of every tree currently in the graph

        for(Vertex v : vertices) { // Initialise every Vertex as its own set
            HashSet<Vertex> vSet = new HashSet<>();
            vSet.add(v);
            trees.add(vSet);
        }

        String noCycle = "\nThis edge connects two separate trees and so does not create a cycle. The two trees are merged.";
        String createsCycle = "\nThis edge creates a cycle and is disregarded.";

        for(Edge e : edges) { // For every edge in order of ascending weight
            Vertex u = e.getSource();
            HashSet uTree = new HashSet<>();
            Vertex v = e.getTarget();
            HashSet vTree = new HashSet<>();

            // For every existing tree, find the one that contains u and the one that contains v
            for(HashSet<Vertex> t : trees) {
                if(t.contains(u))
                    uTree = t;

                if(t.contains(v))
                    vTree = t;
            }

            String currentAnnotation = "Search through every edge in increasing order of weight. Current weight being searched = "+e.weight;

            if(!uTree.equals(vTree)) { // Edge connects two separate trees, will not create a cycle
                setF.add(e);

                trees.remove(uTree); // Remove the two trees connected by new edge
                trees.remove(vTree);

                uTree.addAll(vTree); // Merge the two trees connected by new edges
                trees.add(uTree); // Reinsert the new, merged tree
                
                // Add Vertices to setS for error checking, add edges and Vertices to treeEdges to return values
                if(setS.contains(e.getSource())) {
                    setS.add(e.getTarget());
                    selectedEdges.add(e);
                } else if(setS.contains(e.getTarget())) {
                    setS.add(e.getSource());
                    selectedEdges.add(e);
                } else {
                    setS.add(e.getSource());
                    setS.add(e.getTarget());
                    selectedEdges.add(e);
                }

                if(visualize) {
                    HashSet<Object> currentState = new HashSet<>();
                    currentState.addAll(selectedEdges);

                    currentAnnotation = currentAnnotation.concat(noCycle);

                    stateList.add(currentState);
                    stateAnnotations.add(currentAnnotation);
                }
            } else { // Edge creates a cycle
                if(visualize && setS.size() != s.getVertices().size()) { // Only add these annotations if the tree is still incomplete
                    HashSet<Object> currentState = new HashSet<>();
                    currentState.addAll(selectedEdges);
                    currentState.add(e);

                    currentAnnotation = currentAnnotation.concat(createsCycle);

                    stateList.add(currentState);
                    stateAnnotations.add(currentAnnotation);
                }
            }
        }

        if(setS.size() != s.getVertices().size()) {
            // Tree does not contain every Vertex
            completeTree = false;
        }

        if(visualize) {
            String firstAnnotation = stateAnnotations.get(0);
            firstAnnotation = "Initially, every Vertex is considered its own tree. As the Minimum Spanning Tree grows, these trees are merged together.\n".concat(firstAnnotation);
            stateAnnotations.set(0, firstAnnotation);

            String finalAnnotation = stateAnnotations.get(stateAnnotations.size() - 1);
            finalAnnotation = finalAnnotation.concat("\nThe Minimum Spanning Tree is now complete.");
            stateAnnotations.set(stateAnnotations.size() - 1, finalAnnotation);
        }
    }

    public Object runVis(Structure s, AlgorithmParameters p, Set<Object> selection, ProgressHandler onprogress) throws Exception {
        // Algorithm that specifies how to run differently when called to visualize
        visualize = true;
        return run(s, p , selection, onprogress);
    }

    public Object run(Structure s, AlgorithmParameters p, Set<Object> selection, ProgressHandler onprogress) throws Exception {
        if (s.getVertices().size() == 0) // Check the structure is empty, return an error if true
            return ("The structure should not be empty.");

        // Check every connected Vertex. If there is a disconnected Vertex, return false as MST cannot be created
        HashSet<Vertex> connectedVertices = new HashSet<>();
        HashSet<Vertex> allVertices = new HashSet<>(s.getVertices());

        for (Edge e : (Set<Edge>) s.getEdges()) {
            if(e.isDirected) // Also check for directed edges during this process
                return ("Minimum Spanning Tree Algorithm requires undirected edges");

            connectedVertices.add(e.getSource());
            connectedVertices.add(e.getTarget());
        }

        if(!connectedVertices.equals(allVertices))
            return("Minimum Spanning Tree cannot be created as there is a disconnected Vertex");

        if(visualize) { // Clear data that may have been provided from previous Visualizations
            stateList.clear();
            stateAnnotations.clear();
        }

        HashSet<Object> selectedEdges = new HashSet<>();

        KruskalMST.kruskalMinSpanTree(s, selectedEdges);

        if(completeTree == false) { // If the tree does not contain every edge, return an error
            return("Minimum Spanning Tree cannot be created as there is not a possible tree connecting all Vertices");
        } else {
            // Label every Edge as its weight
            for(Edge e : (Set<Edge>) s.getEdges())
                e.label = ""+e.weight;

            if(visualize) {
                visualize = false;

                visData[0] = stateList;
                visData[1] = stateAnnotations;

                return visData;
            } else {
                return selectedEdges;
            }
        }
    }
}
