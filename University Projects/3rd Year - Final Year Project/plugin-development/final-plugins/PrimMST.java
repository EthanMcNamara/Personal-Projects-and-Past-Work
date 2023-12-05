import gralog.algorithm.*;

import gralog.progresshandler.ProgressHandler;
import gralog.rendering.GralogGraphicsContext;
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
        name = "Minimum Spanning Tree (Prim's)",
        text = "Creates a Minimum Spanning Tree starting from a chosen node",
        url = "https://en.wikipedia.org/wiki/Prim%27s_algorithm"
)
public class PrimMST extends Algorithm {
    private static ArrayList<HashSet<Object>> stateList = new ArrayList<HashSet<Object>>();
    private static ArrayList<String> stateAnnotations = new ArrayList<String>();
    private static Object visData[] = new Object[2];
    
    private static boolean completeTree = true;
    private static boolean visualize = false;

    public static void primMinSpanTree(Structure s, Vertex start, HashSet<Object>  treeEdges) {
        HashSet<Vertex> setQ = new HashSet<>(); // Set of all Vertices
        setQ.addAll(s.getVertices());

        HashSet<Vertex> setS = new HashSet<>(); // Set of Vertices in the Tree
        setS.add(start);
        setQ.remove(start);
        HashSet<Edge> setF = new HashSet<>();

        completeTree = true;

        String noCycle = "\nThis edge does not create a cycle and can be considered for the tree.";
        String createsCycle = "\nThis edge creates a cycle and is disregarded.";

        while (!setQ.isEmpty()) { // While the set of all Vertices is not empty
            // Find lowest weight edge connected to S:
            double lowestWeight = Double.POSITIVE_INFINITY; // Impossible weight for any node, always allows nodes to be selected
            Edge edgeToAdd = null;

            HashSet<Edge> checkedEdges = new HashSet<Edge>();

            for(Vertex v : setS) {
                boolean validEdge = false;
                for(Edge e : v.getIncidentEdges()) {
                    // Cycle through every edge connected to S
                    String currentAnnotation = "Search through every edge connected to the current tree to find the lowest weight that does not create a cycle.";
                    if(!setF.contains(e) && !(setS.contains(e.getSource()) && setS.contains(e.getTarget())) // Checks if edge is valid (not in F, doesn't make cycle)
                    && e.weight < lowestWeight) { // Also checks if edge has a lower weight than previous valid edge
                        lowestWeight = e.weight; // If it has the lowest weight, add it to F
                        edgeToAdd = e;
                        currentAnnotation = currentAnnotation.concat(noCycle);
                        validEdge = true;
                    } else if(!setF.contains(e) && (setS.contains(e.getSource()) && setS.contains(e.getTarget()))) {
                        currentAnnotation = currentAnnotation.concat(createsCycle);
                        validEdge = true;
                    } else if(!setF.contains(e) && !(setS.contains(e.getSource()) && setS.contains(e.getTarget()))) {
                        currentAnnotation = currentAnnotation.concat(noCycle);
                        validEdge = true;
                    }

                    if(validEdge && visualize) {             
                        if(!checkedEdges.contains(e)) {
                            HashSet<Object> currentState = new HashSet<>();
                            currentState.addAll(treeEdges);
                            currentState.add(e);
                            stateList.add(currentState);
                            stateAnnotations.add(currentAnnotation);

                            checkedEdges.add(e);
                        }

                        validEdge = false;
                    }
                }
            }

            if(edgeToAdd == null) { // If a new Edge is not found, a complete Minimum Spanning Tree cannot be made, and so an error is returned
                completeTree = false;
                break;
            }

            // After looking for every possible edge, add new best edge to F
            setF.add(edgeToAdd);

            // Add the new Vertex to S, and the Edge to the tree
            if(setS.contains(edgeToAdd.getSource())) {
                setS.add(edgeToAdd.getTarget());
                setQ.remove(edgeToAdd.getTarget());
                treeEdges.add(edgeToAdd);
            } else if(setS.contains(edgeToAdd.getTarget())) {
                setS.add(edgeToAdd.getSource());
                setQ.remove(edgeToAdd.getSource());
                treeEdges.add(edgeToAdd);
            }

            if(visualize) {
                HashSet<Object> currentState = new HashSet<>();
                currentState.addAll(treeEdges);
                stateList.add(currentState);
                stateAnnotations.add("The Lowest weighted edge that can be considered for the tree has weight "+edgeToAdd.weight+
                    " and is added to the tree.");
            }
        }

        if(visualize) {
            String firstAnnotation = stateAnnotations.get(0);
            firstAnnotation = "The Minimum Spanning Tree initially contains the highlighted Vertex.\n".concat(firstAnnotation);
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

    // Add functionality to label Edges please!!
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

        if(!connectedVertices.equals(allVertices)) // If any Vertices are disconnected, return an error
            return("Minimum Spanning Tree cannot be created as there is a disconnected Vertex");

        // Check that only one Vertex is selected
        Vertex v = selectedUniqueVertex(selection);
        if (v == null)
            return("Please select exactly one Vertex as a starting point for Prim's MST");

        HashSet<Object> treeEdges = new HashSet<>();

        if(visualize) {
            stateList.clear();
            stateAnnotations.clear();
        }           

        PrimMST.primMinSpanTree(s, v, treeEdges);

        if(completeTree == false) {
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
                return treeEdges;
            }
        }
    }
}
