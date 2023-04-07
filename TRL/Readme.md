
## TRL

The **TRL** project was build on top of the https://github.com/d-corsi/ScenarioBasedRL project.

### Part 1: Setting up the python environment
We used Pycharm for editing the project, but you can of course select a different method.

1. Open the cloned python project in Pycharm.

`"Enhance-DL-with-SBM-Modelsward2023\TRL\"`

2. Pycharm should offer you to configure a virtual environment using the included `requirements.txt` file. More details in this [guide](https://www.jetbrains.com/help/pycharm/creating-virtual-environment.html#env-requirements).

3. Configure a python interpreter created in your virtual environment. For example: 
`Enhance-DL-with-SBM-Modelsward2023\TRL\venv_pycharm\Scripts\python.exe`.


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

Prerequisite: You should have both the Python project opened in pycharm, and the unity project opened in Unity editor.

1. In pycharm, add a run configuration to execute `main_guard.py` 

2.  Run the python file, you should see the following line printed in the console:

`[INFO] Listening on port 5004. Start training by pressing the Play button in the Unity Editor.`

3. Go to the "Unity Editor -> Game" and click "play".

4. This will run a subset of the simulation seen in "**Enhancing Deep Learning with Scenario-Based Override Rules: A Case Study**"

### Meaningful parameters:

`models_dir_path`: the directory containing the trained TRL agents.

*test_models*: The default option which is a subset of the trained models.

*gradual_models* - The full simulation as seen in the paper.

`override_enabled`: A parameter that controls whether the override rule will be enabled in the next set of experiments.

`num_of_experiments`: The number of times a loaded agent attempts to solve the maze.

`editor_run`: This parameter controls if the TRL simulation will run with/without UNITY gui

Good luck!
