## TRL

The **TRL** project was build on top of the https://github.com/d-corsi/ScenarioBasedRL project.

### Part 1: Setting up the python environment

### Part 2: Setting up Unity

Here are the steps for setting the simulation environment for the TurtleBot3.

1. Download the last version of the UnityHub here: [https://unity.com/download](https://unity.com/download).

2. Open the Unity Hub application. Go to -> "Installs" tab and click on "Install Editor". We used 2021.3.6f1.

3. Download the package ML-Agents from the official unity GitHub: [https://github.com/Unity-Technologies/ml-agents](https://github.com/Unity-Technologies/ml-agents). The only folder which is actually required is **com.unity.ml-agents**

4. *Optional:* Inside Unity go to  "window -> package manager" -> click on the "+" (upper left) -> add package from list -> select the folder **com.unity.ml-agents**  and install.

5. Go to the projects tab. Open the folder `"Enhance-DL-with-SBM-Modelsward2023\TRL\unity_project\"` in your cloned repository.

6. Create a folder in the unity project called Build. 
For example:  `"Enhance-DL-with-SBM-Modelsward2023\TRL\unity_project\Build"`.

7. Go to  "file -> build setting -> build"  and build the environment for the simulation. You can choose to build for Linux/Windows.

8. Once the build is completed, Go to "File -> Open Scene" and open the following scene:

`"Enhance-DL-with-SBM-Modelsward2023\TRL\unity_project\Assets\Scenes\SafeRobotics.unity"`

9. You should now see two tabs: "Scene" and "Game". Click on the "Game" tab and you should be able to see the robot at the center of the maze.

### Part 3: Executing the simulation
