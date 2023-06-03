## Getting started with PCC-SBM protocol execution

**Prerequisite:** before you try to run the **PCC-SBM** protocol, make sure that you are able to 
run the [PCC-Uspace simulator and PCC-RL together](https://github.com/adielashrov/Enhance-DL-with-SBM-Modelsward2023/blob/main/PCC_SBM/GettingStarted_with_PCC_RL_PCC_Uspace.md).

### Step 1- Build PCC-SBM

First we need to build the BPC package, which is a C++ implementation of Scenario-based modeling.

The next section assume that the project was cloned into the "Enhance-DL-with-SBM-Modelsward2023" folder.

<!--TODO - copy the engineFactory file-->

#### Step 1.1 - Build the PCC protocol

```
cd Enhance-DL-with-SBM-Modelsward2023
cd PCC_SBM
make
```

You should see the following errors (it is expected).

```
g++: error: bpc_core_v1.0.0/engine/CommonReal.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/EngineConfiguration.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/Errno.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/SelectorFactory.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/ServerSocketFactory.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/SocketFactory.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/ThreadListenerFactory.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/Truth.obj: No such file or directory
g++: error: bpc_core_v1.0.0/engine/WeaverFactory.obj: No such file or directory
g++: error: say_good_morning/src/EngineFactory.obj: No such file or directory
```

#### Step 1.1 - Build the BPC package

Build say_good_morning
```
cd say_good_morning
make
```

Build bpc_core
```
cd ..
cd bpc_core_v1.0.0
make
```

#### Step 1.2 - Build the PCC-SBM protocol(Second trial)

Build PCC-SBM protocol
```
cd ..
make
```
You should see the following messages.

```
./EngineTests.tests
Running 30 tests..............................OK!
make[2]: Leaving directory '/Enhance-DL-with-SBM-Modelsward2023/PCC_SBM/src/bpc_core_v1.0.0/engine/tests'
make[1]: Leaving directory '/Enhance-DL-with-SBM-Modelsward2023/PCC_SBM/src/bpc_core_v1.0.0/engine'
Done
```

### Step 2 - Run the PCC_SBM protocol

**Prerequistes:** 

(1) This part assumes that you have trained an agent for controlling the sending rate.

The full guide can be found here:  [Getting_Started_with_PCC_RL](https://github.com/adielashrov/Enhance-DL-with-SBM-Modelsward2023/blob/main/PCC_SBM/GettingStarted_with_PCC_RL_PCC_Uspace.md).

(2) You have a trained model named  ```paper_model_v0``` at the folder ```PCC_SBM_RL/src/gym/```.

Go to the project source folder.
```
cd /Enhance-DL-with-SBM-Modelsward2023/PCC_SBM/src
```

#### Step 2.1 - Run the PCC_SBM server

Start the server:
```
./app/pccserver recv 9000
```

#### Step 2.2 - Run the PCC_SBM protocol

Start the PCC-SBM:
```
./app/pccclient send 127.0.0.1 9000 --pcc-rate-control=bp -pyhelper=loaded_client -pypath=/mnt/hgfs/PCC_SBM_RL/src/udt-plugins/testing/ --history-len=10 --pcc-utility-calc=loss-only --model-path=/mnt/hgfs/PCC_SBM_RL/src/gym/paper_model_v0
```

Meaningful parameters:
```
--pcc-rate-control: the protocl which will control the sending rate, in this it is PCC_SBM, a.k.a bp protocol.
--pcc-utility-calc: the loss function utilized in the monitor interval statistics provdied to the agent
--model-path: the path to the model extended by the PCC_SBM protocol 
```

### Step 3 - Configuring Reduce/Restore policies 

#### Reduce policy

The ```ReduceThroughputBThread``` class controls the reduction of the sending rate.

If you wish to change the policy, you will need to modify the member ```yield_policy``` in this class.
There are three possible yield policies.
1. Immediate decline to a fixed low sending rate
2. Gradual decline using a step function
3. Gradual decline using exponential decay

You can control the policy by setting the ```yield_policy``` member to the appropriate index.

Additional parameters:
```
initial_sending_rate_for_yield: remembers the initial sending rate when reduce begins.
division_exp: parameter controlling the step function policy
lambda: constant affecting exponential decay policy
```

#### Restore policy

The ```RestoreThroughputBThread``` class controls the restoration of the sending rate.

If you wish to change the policy, you will need to modify the member ```restore_policy``` in this class.
There are two possible restore policies.
1. Slow start - slow restoration to the previous values policy
2. Immediate return to the model’s original output

You can control the policy by setting the ```restore_policy``` member to the appropriate index.

#### Alternating between restore/reduce

The ```MonitorNetworkStateBThread``` class controls the transition  of the sending rate.

If you wish to change the policy, you will need to modify the member ```restore_policy``` in this class.
There are two possible restore policies.
1. Slow start - slow restoration to the previous values policy
2. Immediate return to the model’s original output

You can control the policy by setting the ```restore_policy``` member to the appropriate index.


Additional parameters:
```
initial_sending_rate_for_yield: remembers the initial sending rate when reduce begins.
division_exp: parameter controlling the step function policy
lambda: constant affecting exponential decay policy
```

### Step 4 - Launching PCC-SBM together with the naive PCC-IXP

### Step 5 - Launching PCC-SBM together with PCC-RL


