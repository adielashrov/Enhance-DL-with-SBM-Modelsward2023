This is a short guide on how to get started with training [Aurora PCC-RL](https://github.com/PCCproject/PCC-RL) on linux/ubuntu.

**Prerequisite:** You should have Python above >= 3.7.

### Step 1 - Install OpenMPI - Package
```
sudo apt-get update && sudo apt-get install cmake libopenmpi-dev python3-dev zlib1g-dev
```

### Step 2 - Create virtual environment and upgrade pip

```
pip install --upgrade pip
python -m venv pcc_rl_env
source pcc_rl_env/bin/activate
```
if you want to leave/deactivate the virtual environment:
```
deactivate
```

### Step 3 - How to install stable-baselines (version 2):
```
pip install git+https://github.com/hill-a/stable-baselines 
```
This should take care of the tensorflow appropriate version requirement

### Step 4 - Tensorflow - 1.8.0 to 1.15.0

Stable-Baselines supports Tensorflow versions from 1.8.0 to 1.15.0, and does not work on Tensorflow versions 2.0.0 and above.

The Command from Step 3 should install Tensorflow automatically.

If this wasn't succesffull, please follow through with manaul installation.

#### Manual installation of  tensorflow :
```
pip uninstall tensorflow
pip install tensorflow==1.15
```

### Step 5 -numpy  collision error with tensorflow:

```
ERROR: tensorflow 1.15.5 has requirement numpy<1.19.0,>=1.16.0, but you'll have numpy 1.21.3 which is incompatible.
```
**Solved:**
```
pip uninstall numpy
pip3 install numpy==1.18.5
```

### Step 6 - mpi4py is a requirement for stable-baselines

**Issue:**
```
Traceback (most recent call last):
 File "stable_solve.py", line 21, in <module>
   from stable_baselines import PPO1
ImportError: cannot import name 'PPO1'
```
**Solved:**
```
pip install mpi4py==3.0.3
```

Step 6 - Train using stable_solve:
----------------------------------
```
cd src/gym
python stable_solve.py 
```

Good luck!
