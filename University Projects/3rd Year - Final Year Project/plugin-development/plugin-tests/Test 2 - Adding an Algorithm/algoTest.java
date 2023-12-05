// package gralog.algorithm;

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
        name = "Ethan's First Algorithm",
        text = "Finds a shortest path between two selected vertice, using Dijkstra's Algorithm",
        url = "https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm"
)
public class algoTest extends Algorithm {

    public static final String ANSI_RED = "\u001B[31m";
    public static final String ANSI_RESET = "\u001B[0m";


    public static void dijkstraShortestPath(Structure s, Vertex start,
                                            Vertex target,
                                            HashMap<Vertex, Vertex> predecessor,
                                            HashMap<Vertex, Edge> edgeFromPredecessor,
                                            HashMap<Vertex, Double> distances) {
        HashSet<Vertex> setQ = new HashSet<>();
        setQ.addAll(s.getVertices());
        predecessor.put(start, null);
        edgeFromPredecessor.put(start, null);
        distances.put(start, 0d);

        while (!setQ.isEmpty()) {
            Vertex u = null;
            for (Vertex uit : setQ)
                if (distances.containsKey(uit))
                    if (u == null || (distances.get(uit) < distances.get(u)))
                        u = uit;
            if (u == null) // happens if there are unreachable vertices
                break;

            setQ.remove(u);
            if (u == target) // found desired shortest path
                // target is always null so idk why we check this?? - Ethan
                break;

            Double distu = distances.get(u);

            for (Edge e : u.getIncidentEdges()) {
                if (e.getSource() != u && e.isDirected) // incoming (directed) edge
                    continue;

                Vertex other = e.getTarget();
                if (other == u)
                    other = e.getSource();

                if (!setQ.contains(other)) // shortest path already found
                    continue;

                Double alt = distu + e.weight;
                if ((!distances.containsKey(other)) || (alt < distances.get(other))) {
                    distances.put(other, alt);
                    predecessor.put(other, u);
                    edgeFromPredecessor.put(other, e);
                }
            }
        }
    }

    public static void dijkstraShortestPath(Structure s, Vertex start,
                                            HashMap<Vertex, Vertex> predecessor,
                                            HashMap<Vertex, Edge> edgeFromPredecessor,
                                            HashMap<Vertex, Double> distances) {
        algoTest.dijkstraShortestPath(s, start, null, predecessor, edgeFromPredecessor, distances);
    }

    public Object run(Structure s, AlgorithmParameters p, Set<Object> selection,
                      ProgressHandler onprogress) throws Exception {
        if (s.getVertices().size() == 0)
            return ("The structure should not be empty.");
        for (Edge e : (Set<Edge>) s.getEdges()) {
            if (e.weight < 0d)
                return ("Dijkstra's Algorithm requires non-negative edge weights");
        }


        // As confusing as this is, only checks that ONE vertex is selected, any number of edges can also be selected - Ethan
        Vertex v = selectedUniqueVertex(selection);
        if (v == null)
            return "Please, select exactly one vertex: then all shortest paths to all other vertices will be computed.";


        // deselect all edges
        HashSet<Object> selectedEdges = new HashSet<>();
        for (Object o : selection)
            if (o instanceof Edge)
                selectedEdges.add(o);
        selection.removeAll(selectedEdges);

        // Creates new HashMaps for Vertices, Edges and distances - Ethan
        HashMap<Vertex, Vertex> predecessor = new HashMap<>();
        HashMap<Vertex, Edge> edgeFromPredecessor = new HashMap<>();
        HashMap<Vertex, Double> distances = new HashMap<>();

        // The algorithm puts values in the HashMap so they can be called here without returning a value it would seem - Ethan
        algoTest.dijkstraShortestPath(s, v, predecessor, edgeFromPredecessor, distances);

        selectedEdges.clear();
        edgeFromPredecessor.remove(v);
        selectedEdges.addAll(edgeFromPredecessor.values());


        return selectedEdges;
    }
}