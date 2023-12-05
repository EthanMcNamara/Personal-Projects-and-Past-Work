// New class to control Visualizations - Ethan

package gralog.gralogfx.panels;

import gralog.gralogfx.StructurePane;
import gralog.rendering.GralogColor;
import gralog.structure.*;

import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.layout.*;
import javafx.scene.text.*;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

public class VisualizationPanel extends AnchorPane {
    
    private ArrayList<HashSet<Object>> states = null;
    private ArrayList<String> annotations = null;
    private Button fwd, back, start, end;
    private int stateCount = 0, currentState = 0;
    private ScrollPane annotationScroll;    
    private StructurePane sp = null;
    private Text stateCounter, annotationText;

    public VisualizationPanel() { // Initializes the Panel
        // Label all buttons, set their actions and disable them until they need to be used
        fwd = new Button(">>>");
        fwd.setOnMouseClicked(event -> drawNextState());
        fwd.setDisable(true);
        fwd.setMinWidth(75);

        back = new Button("<<<");
        back.setOnMouseClicked(event -> drawPrevState());
        back.setDisable(true);
        back.setMinWidth(75);

        start = new Button("|<");
        start.setOnMouseClicked(event -> drawFirstState());
        start.setDisable(true);
        start.setMinWidth(75);

        end = new Button(">|");
        end.setOnMouseClicked(event -> drawLastState());
        end.setDisable(true);
        end.setMinWidth(75);

        // Initialize text
        stateCounter = new Text();
        Font font = stateCounter.getFont();
        stateCounter.setFont(Font.font(font.getFamily(), FontWeight.BOLD, font.getSize()));

        annotationText = new Text();
        annotationText.setWrappingWidth(300);
        updateText();

        annotationScroll = new ScrollPane();
        annotationScroll.setContent(annotationText);
        annotationScroll.setStyle("-fx-background-color:transparent;"); // From https://stackoverflow.com/questions/12899788/javafx-hide-scrollpane-gray-border ?

        // Add buttons to HBoxes
        HBox stepButtons = new HBox();
        stepButtons.getChildren().addAll(back, fwd);

        HBox jumpButtons = new HBox();
        jumpButtons.getChildren().addAll(start, end);

        // Add buttons to a VBox
        VBox controls = new VBox();
        controls.getChildren().addAll(stepButtons, jumpButtons);
        controls.setMinWidth(150);
        controls.setPrefWidth(150);

        // Add text to a VBox
        VBox annotations = new VBox();
        annotations.getChildren().addAll(stateCounter, annotationScroll);

        // combine VBoxes to HBox
        HBox contents = new HBox(5);
        contents.getChildren().addAll(controls, annotations);

        this.getChildren().addAll(contents);

        setTopAnchor(contents, 5.0);
        setRightAnchor(contents, 5.0);
        setLeftAnchor(contents, 5.0);
        setBottomAnchor(contents, 5.0);
    }

    public void clear() {
        stateCount = 0;
        currentState = 0;

        fwd.setDisable(true);
        back.setDisable(true);
        start.setDisable(true);
        end.setDisable(true);

        updateText();
    }

    private void updateText() {
        if(stateCount > 0) { // Update current step and annotation
            stateCounter.setText("step "+(currentState+1)+" of "+stateCount+":");
            annotationText.setText(annotations.get(currentState));
        } else { // Show nothing when not visualizing
            stateCounter.setText("");
            annotationText.setText("");
        }  
    }

    public void drawFirstState() { // Set currentState to 0 and call drawState
        currentState = 0;
        drawState(currentState);
    }

    public void drawLastState() { // Set currentState to final state and call drawState
        currentState = stateCount-1;
        drawState(currentState);
    }

    private void drawNextState() { // Increment currentState and call drawState
        if(currentState < stateCount - 1)
            currentState++;

        drawState(currentState);
    }

    private void drawPrevState() { // Decrease currentState and call drawState
        if(currentState > 0)
            currentState--;

        drawState(currentState);
    }

    private void drawState(int i) { // Process state and annotations
        // If currentState is the final state, disable the 'fwd' and 'end' buttons, otherwise enable them
        if(i == stateCount - 1) {
            fwd.setDisable(true);
            end.setDisable(true);
        } else {
            fwd.setDisable(false);
            end.setDisable(false);
        }

        // If currentState is the 0, disable the 'back' and 'start' buttons, otherwise enable them
        if(i == 0) {
            back.setDisable(true);
            start.setDisable(true);
        } else {
            back.setDisable(false);
            start.setDisable(false);
        }

        if(sp == null || states == null) { // Returns if there is no assigned StructurePane or State to return. Shouldn't need to happen but just in case something is wrong
            return;
        }

        HashSet<Object> state = states.get(i);
        // Taken from MainWindow when handling instanceof Set
        boolean isSelection = true;
        for (Object o : (Set) state)
            if (!(o instanceof Vertex)
                    && !(o instanceof Edge)
                    && !(o instanceof EdgeIntermediatePoint))
                isSelection = false;
        if (isSelection) {
            Structure s = sp.getStructure();
            HashSet<Edge> edges = new HashSet<Edge>();
            edges.addAll(s.getEdges());
            for(Edge e : edges) {
                e.color = GralogColor.BLACK;
            }

            for (Object o : (Set) state) {
                if (o instanceof Vertex)
                    ((Vertex) o).fillColor = GralogColor.WHITE;
                if (o instanceof Edge)
                    ((Edge) o).color = GralogColor.BLUE;
            } 

            sp.clearSelection();
            sp.selectAll((Set) state);
            sp.requestRedraw();
        }
        // End of MainWindow snippet

        // Finally, call updateText
        updateText();
    }

    public void setStructurePane(StructurePane newSP) {
        sp = newSP;
    }

    public void setData(Object[] visData) {
        // Get states and annotations from visData 
        states = (ArrayList<HashSet<Object>>) visData[0];
        annotations = (ArrayList<String>) visData[1];

        // Update stateCount and reset currentState
        stateCount = states.size();
        currentState = 0;
        updateText();
    }
}
