## Overview
This Project is based around developing an expanded version of GrALoG - A program built around Visualizing Graphs, Algorithms, Logic and Games interactively. \
The original version of GrALoG that this Project expanded on can be found at: https://github.com/gralog

## Contents

This repository contains a build of GrALoG with the new functionality already implemented in `gralog-master/`. \
On top of this, a copy of the files used to create the plugin, and a pre-compiled .jar are included in `plugin-development/final-plugins/`, and copies of any other GrALoG files that have been edited are included in `edited-files/`.

Below are instructions for how to run GrALoG, as well as rebuild the program from scratch if necessary.

## Running GrALoG
The plugins for GrALoG were designed on a Windows machine running Java 11. The following instructions are to run the program under those conditions

### How To Run:
- Open the `gralog-master/` directory in cmd.
- Enter the following:
```
gradlew
```
- After building, GrALoG will run.
- To run the code again without gradlew, navigate to `gralog-master/build/dist/` and run `gralog-fx.jar`

If the build is functioning as intended, there should be an option on the menu ribbon titled "Visualize", as well as a panel in the lower right corner titled "Visualization Control"

### Running from a fresh GrALoG build
If the build is not functioning as intended, a fresh version of the GraLoG software can be downloaded from https://github.com/gralog/gralog. \
The following steps can be taken to build the code as intended:

#### Setting up GrALoG:
- From `edited-files/`, copy AlgorithmThread.java, config.xml, MainMenu.java and MainWindow.java, and paste them in `gralog-master/gralog-fx/src/main/java/gralog/gralogfx/`
- From `edited-files/`, copy VisualizationPanel.java and paste it in `gralog-master/gralog-fx/src/main/java/gralog/gralogfx/panels/`
- From `edited-files/`, copy Algorithm.java and AlgorithmManager.java, and paste them in `gralog-master/gralog-core/src/main/java/gralog/algorithm/`

#### Recompiling Plugins:
- Navigate to `gralog-master/build/dist/libs/` and copy the full directory path (eg: "C:\Users\\[username]\Documents\gralog-master\...")
- Open `plugin-development/final-plugins/` in cmd
- Enter the following:
```
javac -classpath "[path to libs]\gralog-core.jar" PrimMST.java KruskalMST.java FloydWarshall.java

jar cf Plugins.jar PrimMST.class KruskalMST.class FloydWarshall.class
```
- Copy Plugins.jar and paste it in `gralog-master/plugins/`

#### Rebuilding GrALoG:
Now take the steps from "How To Run" and GrALoG should run as intended with the Plugins.
