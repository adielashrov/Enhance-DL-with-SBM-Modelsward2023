
### Running PCC-Uspace simulator and PCC-RL together

**Prerequisite:** Download both projects and configure PCC-RL venv
 
Check out the modules before starting to work and make sure that you are defined with python>=3.7

Note to check out PCC-Uspace from the deep learning branch

https://github.com/PCCproject/PCC-RL

https://github.com/PCCProject/PCC-Uspace/tree/deep-learning


Configuring  venv for PCC-RL was covered in the previous guide.
 
https://github.com/guykatzz/KatzLab/blob/master/docs/Getting_Started_with_PCC_RL.md


 

### Step 1- Train an aurora model and save it
 

Navigate to the relevant folder
```
cd ~/Documents/Research/Aurora/PCC-RL
```
 

Activate venv:
```
source pcc_rl_env/bin/activate
```

Navigate to folder:
```
cd src/gym
```
 
Create dir for model:
```
mkdir saved_model
```

Train and save the model:
```
python stable_solve.py --model-dir=saved_model
```
 

You can change the number of iterations in stable_solve.py for a shorter training session

 
### Step 2 – Download and build PCC-Uspace
 

Install python3.7 dev tools
```
sudo apt-get install python3.7-dev
```
 
Download PCC-Uspace from the link above and open the project in an editor
 
Add the following lines to loaded_client.py in the PCC-RL project (the path should be to your virtual environment folder in PCC-RL project):
```
path_to_packges = "/home/ashrov/Documents/Research/Aurora/PCC-RL/pcc_rl_env/lib/python3.7/site-packages/"
sys.path.insert(0, path_to_packges)
print("sys.path after insert")
print('\n'.join(sys.path))
```

Modify python3.5 to python3.7 in the following locations
```
pcc_python_rc.h
Makefile
```
Note that the makefile references libs in system locations, make sure that the libs are indeed in those locations.

Define an environment variable for finding .so files
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ashrov/Documents/Research/Aurora/PCC-Uspace-DL/src/core/
```

Note that this should be defined in each terminal window where you plan to run the Client/Server.

Go to src and build the project
```
cd src
make clean
cd src/say_good_morning
make
cd ..
make
```
This should build the PCC client and PCC server applications.

Start the udt server side of the environment – regular:
```
./app/pccserver recv 9000
```
 

Start the udt client side of the environment – regular:
```
./app/pccclient send 127.0.0.1 9000
```
 

Start the udt client side of the environment – with Deep Learning:
```
./app/pccclient send 127.0.0.1 9000 --pcc-rate-control=python -pyhelper=loaded_client -pypath=/home/ashrov/Documents/Research/Aurora/PCC-RL/src/udt-plugins/testing/ --history-len=10 --pcc-utility-calc=linear --model-path=/home/ashrov/Documents/Research/Aurora/PCC-RL/src/gym/
```
