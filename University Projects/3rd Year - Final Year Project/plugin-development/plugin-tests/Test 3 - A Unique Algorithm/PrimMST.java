import gralog.algorithm.*;

import gralog.progresshandler.ProgressHandler;
import gralog.structure.Edge;
import gralog.structure.Structure;
import gralog.structure.Vertex;

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

    public static void primMinSpanTree(Structure s, Vertex start, Vertex target, HashMap<Vertex, Edge>  treeEdges) {
        HashSet<Vertex> setQ = new HashSet<>();
        setQ.addAll(s.getVertices());

        // Choose an arbitrary start node s in V
        // Done!
        HashSet<Vertex> setS = new HashSet<>();
        setS.add(start);
        setQ.remove(start);
        HashSet<Edge> setF = new HashSet<>();


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
        }
    }

    public static void primMinSpanTree(Structure s, Vertex start, HashMap<Vertex, Edge>  treeEdges) {
        PrimMST.primMinSpanTree(s, start, null, treeEdges);
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


        // deselect all edges
        HashSet<Object> selectedEdges = new HashSet<>();
        for (Object o : selection)
            if (o instanceof Edge)
                selectedEdges.add(o);
        selection.removeAll(selectedEdges);

        HashMap<Vertex, Edge> treeEdges = new HashMap<>();

        PrimMST.primMinSpanTree(s, v, treeEdges);

        selectedEdges.clear();

        selectedEdges.addAll(treeEdges.values());

        return selectedEdges;
    }
}
