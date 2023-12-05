import gralog.algorithm.*;

import gralog.progresshandler.ProgressHandler;
import gralog.structure.Edge;
import gralog.structure.Structure;
import gralog.structure.Vertex;
import gralog.structure.UndirectedGraph;

import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

/**
 *
 */
@AlgorithmDescription(
        name = "Minimum Spanning Tree (Prim's)",
        text = "Creates a Minimum Spanning Tree starting from a chosen node",
        url = "[NEEDS URL]"
)
public class PrimMST extends Algorithm {

    public static final String ANSI_RED = "\u001B[31m";
    public static final String ANSI_RESET = "\u001B[0m";

    private static ArrayList<HashSet<Object>> stateList = new ArrayList<HashSet<Object>>();
    
    private static boolean completeTree = true;
    private static boolean newVert = false;
    private static boolean visualise = false;
    private static boolean prevVisState = false;
    
    private static Vertex selVert = null;
    private static Vertex prevVisVert = null;
    

    public static void primMinSpanTree(Structure s, Vertex start, Vertex target, HashMap<Vertex, Edge>  treeEdges) {
        HashSet<Vertex> setQ = new HashSet<>();
        setQ.addAll(s.getVertices());

        // Choose an arbitrary start node s in V
        // Done!
        HashSet<Vertex> setS = new HashSet<>();
        setS.add(start);
        setQ.remove(start);
        HashSet<Edge> setF = new HashSet<>();

        completeTree = true;

        while (!setQ.isEmpty()) { // While the set of all vertices is not empty
            // Find lowest weight edge connected to S:
            double lowestWeight = Double.POSITIVE_INFINITY; // Impossible weight for any node, always allows nodes to be selected
            Edge edgeToAdd = null;

            for(Vertex v : setS) {
                for(Edge e : v.getIncidentEdges()) {
                    // Cycle through every edge connected to S
                    if(!setF.contains(e) && !(setS.contains(e.getSource()) && setS.contains(e.getTarget())) // Checks if edge is valid (not in F, doesn't make cycle)
                    && e.weight < lowestWeight) { // Also checks if edge has a lower weight than previous valid edge
                        lowestWeight = e.weight; // If it has the lowest weight, add it to F
                        edgeToAdd = e;
                    }
                }
            }

            if(edgeToAdd == null) {
                completeTree = false;
                break;
            }

            // After looking for every possible edge, add new best edge to F
            setF.add(edgeToAdd);

            if(setS.contains(edgeToAdd.getSource())) {
                setS.add(edgeToAdd.getTarget());
                setQ.remove(edgeToAdd.getTarget());
                treeEdges.put(edgeToAdd.getTarget(), edgeToAdd);
            } else if(setS.contains(edgeToAdd.getTarget())) {
                setS.add(edgeToAdd.getSource());
                setQ.remove(edgeToAdd.getSource());
                treeEdges.put(edgeToAdd.getSource(), edgeToAdd);
            }

            if(visualise) {
                HashSet<Object> currentState = new HashSet<>();
                currentState.addAll(treeEdges.values());
                stateList.add(currentState);
            }
        }
    }

    public static void primMinSpanTree(Structure s, Vertex start, HashMap<Vertex, Edge>  treeEdges) {
        PrimMST.primMinSpanTree(s, start, null, treeEdges);
    }

    public Object runVis(Structure s, AlgorithmParameters p, Set<Object> selection, ProgressHandler onprogress) throws Exception {
        // Algorithm that specifies how to run differently when called to visualize
        visualise = true;
        return run(s, p , selection, onprogress);
    }

    public Object run(Structure s, AlgorithmParameters p, Set<Object> selection,
                      ProgressHandler onprogress) throws Exception {
        if (s.getVertices().size() == 0)
            return ("The structure should not be empty.");

        // Check every connected vertex. If there is a disconnected vertex, return false as MST cannot be created
        HashSet<Vertex> connectedVertices = new HashSet<>();
        HashSet<Vertex> allVertices = new HashSet<>(s.getVertices());

        for (Edge e : (Set<Edge>) s.getEdges()) {
            if(e.isDirected)
                return ("Minimum Spanning Tree Algorithm requires undirected edges");

            connectedVertices.add(e.getSource());
            connectedVertices.add(e.getTarget());
        }

        if(!connectedVertices.equals(allVertices))
            return("Minimum Spanning Tree cannot be created as there is a disconnected Vertex");

        // As confusing as this is, only checks that ONE vertex is selected, any number of edges can also be selected - Ethan
        Vertex v = selectedUniqueVertex(selection);
        if (v == null)
            return("Please select exactly one vertex as a starting point for Prim's MST");


        if(selVert != v || (visualise && prevVisVert == v)) {
            // Clear stateList, different vertex selected.
            stateList.clear();
            newVert = true;
        } else {
            if(!(prevVisState && visualise))
                newVert = true;
            else
                newVert = false;
        }

        HashSet<Object> selectedEdges = new HashSet<>();
        HashMap<Vertex, Edge> treeEdges = new HashMap<>();

        selVert = v;

        if(visualise) {
            prevVisVert = v;
        }
            

        if(newVert == true) { // || (newVert == false && visualise == true)
            PrimMST.primMinSpanTree(s, v, treeEdges);
        }

        prevVisState = visualise;

        if(completeTree == false) {
            return("Minimum Spanning Tree cannot be created as there is not a possible tree connecting all Vertices");
        } else {
            selectedEdges.addAll(treeEdges.values());

            if(visualise) {
                visualise = false;
                return stateList;
            } else {
                return selectedEdges;
            }
        }
    }
}
