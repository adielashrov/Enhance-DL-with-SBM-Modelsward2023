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
#### Step 1.2 - Build the PCC-SBM protocol

Build PCC-SBM protocol
```
cd ..
make
```

## Configuring Reduce/Restore policies 
### Reduce policy

### Restore policy


### Launching PCC-SBM together with the naive PCC-IXP

### Launching PCC-SBM together with PCC-RL


