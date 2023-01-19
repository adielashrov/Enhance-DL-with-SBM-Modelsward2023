import gym; gym.logger.set_level(40)
from mlagents_envs.environment import UnityEnvironment
from gym_unity.envs import UnityToGymWrapper
import numpy as np
import os, collections


class RoboticNavigation( gym.Env ):
	
	"""
	A class that implements a wrapper between the Unity Engine environment of and a custom Gym environment.
	
	The main motivations for this wrapper are:

		1) Fix the sate
			originally the LiDAR scans arrive with a size of 2 * number_of_scan, beacuse for each direction Unity returns two values, the 
			first one is a float that represent the distance from the first obstacle, nomralized between [0, 1]. The second one is a flag integer [0, 1]
			which indicates if there is an obstacle in the range of the corresponing scan. To avoid a strong correlation between the sensors input of the network, 
			we removed the flag value. This is also to increase the explainability of the state value (useful also for the properties).

		2) Change the reward
			this wrapper allows us to change the reward function without modifying the Unity3D project.
	"""

	def __init__( self, step_limit=300, worker_id=0, editor_run=False, random_seed=0, rules_active=False ):

		"""
		Constructor of the class.

		Parameters
		----------
			rendered : bool
				flag to run the envornoment in rendered mode, currently unused (default: False)
		"""

		# Load input properties
		self.step_limit = step_limit
		self.rules_active = rules_active

		# Load the scan number given in input
		self.scan_number = 7		

		# If the env_path is given as input override the environment search
		if not editor_run:

			"""
			env_type = None

			# Sanity check for the 'env_type' option
			assert_message = "Invalid env_type, options [None, training, render]"
			assert (env_type in [None, "training", "render", "gym"]), assert_message

			# Detect the platform (linux) and load the corresponding environment path
			# without any specification using the default one.
			if env_type == "training": env_path = "env/linux_training/SafeRobotics"
			if env_type == "render": env_path = "env/linux_render/SafeRobotics"
			if env_type == "gym": env_path = "env/linux_gym/SafeRobotics"
			"""

			# If on windows call the windows build
			if os.name == "nt": env_path = "env/windows_training/SafeRobotics"
			# If on linux call the inux build
			else: env_path = "env/linux_training/SafeRobotics"

		# For the editor build force the path to None and the worker id to 0, 
		# assigned values for the editor build.
		else: 
			env_path = None
			worker_id = 0

		# Load the Unity Environment
		unity_env = UnityEnvironment(env_path, worker_id=worker_id , seed=random_seed )
		
		# Convert the Unity Environment in a OpenAI Gym Environment, setting some flag 
		# according with the current setup (only one branch in output and no vision observation)
		self.env = UnityToGymWrapper( unity_env, flatten_branched=True )

		#TODO: we should arrive at this point after launching the gym unity simulator

		# Override the action space of the wrapper
		self.action_space = self.env.action_space

		# Override the state_size, the orginal version provide a 2*scan_number size for the LiDAR,
		# for each direction 2 value, one with the flaot value and one with the flag [0, 1]. In this
		# wrapper we remove the flag, maintaining only one value for each direction
		# Subtract also the dummy value for the cost.
		state_size = self.env.observation_space.shape[0] - self.scan_number - 1

		# Sanity check for the scan number
		assert_message = "Mismatching between the given scan number and the observations (check the cost value)"
		assert (state_size == self.scan_number + 2), assert_messages

		# Initialize the counter for the maximum step counter
		self.step_counter = 0

		# Acoording to the previous line, we override the observation space
		# lidar_scan + 2 elements, normalized in [0, 1] ==> heading (first) + distance (second)
		self.observation_space = gym.spaces.Box(
			np.array([0 for _ in range(self.scan_number)] + [0, 0]),
			np.array([1 for _ in range(self.scan_number)] + [1, 1]),
			dtype=np.float64
		)

		# RULES: TEMPORARY VERSION TO REMOVE
		from collections import deque
		self.last_3_actions = deque( maxlen=3 )
		self.rules_violated = 0

		# Current state for BP sensor/actuator query/update
		self.current_state = []
		self.state_is_ready_to_read = False
		self.done = False


	def reset( self ):

		"""
		Override of the reset function of OpenAI Gym

		Returns
		----------
			state : list
				a list of the observation, with scan_number + 2 elements, the first part contains the information
				about the ldiar sensor and the second angle and distance in respect to the target. All the values
				are normalized between [0, 1]
		"""

		# Reset the counter for the maximum step counter
		self.step_counter = 0

		# Override the state to return with the fixed state, as described in the constructor
		state = self.env.reset()

		# Call the function that fix the state according with our setup
		state = self.fix_state( state )

		# Store the distance for the reward function
		self.target_distance = state[-1]

		# When we are resetting we need to turn the done flag off
		self.done = False
		self.set_state(state)
		#
		return state


	def step(self, action):

		"""
		Override of the step function of OpenAI Gym

		Parameters
		----------
			action : int
				integer that represent the action that the agent must performs

		Returns
		----------
			state : list
				a list of the observation, with scan_number + 2 elements, the first part contains the information
				about the ldiar sensor and the second angle and distance in respect to the target. All the values
				are normalized between [0, 1]
			reward : float
				a single value that represent the value of the reward function from the tuple (state, action)
			done : bool
				flag that indicates if the current state is terminal
			state : dict
				a dictionary with some additional information, currently empty
		"""

		# Call the step function of the OpenAI Gym class
		state, reward, _, _ = self.env.step( action )

		# Initialize the empty dictionary
		info = {}

		# Increase the step counter
		self.step_counter += 1

		# Computing all the info from the environment 
		info["goal_reached"] = (reward == 1)
		info["collision"] = (reward == -1)
		info["cost"] = (state[-1] == 1)
		info["time_out"] = (self.step_counter >= self.step_limit)
		info["rules_value"] = 0

		# Call the function that fix the state according with our setup
		state = self.fix_state( state )

		# Fix the state, if the agent is 'inside' an obstacle (i.e., cost = 1)
		# all the lidar scan must be equal to 0
		if info["cost"]: state[:-2] = 0

		# Overrride the Done function, we consider an episode terminated if:
		# - the agent reaches the goal
		# - there is collision with a 'hard' obstacle
		# - the number of steps exceed the limit
		# - the agent goes too far away from the goal (greater than normalization factor)
		done = (info["goal_reached"] or info["collision"] or info["time_out"] or state[-1] > 1)

		# Computing the basic step reward with the new
		# formula: a bonus to move towards the target adding a nomralization
		# multiplier and a small penalty for each step
		# If terminal states (done) use the default value (1 or -1).
		if not done:
			reward_multiplier, step_penalty = 3, 0.0001
			new_distance = state[-1]
			reward = (self.target_distance - new_distance) * reward_multiplier - step_penalty
			self.target_distance = new_distance
		else:
			self.done = True

		# Here it's possible to override the reward given by the Unity Engine
		# default returns the standard reward from the environment
		reward = self.override_reward( state, reward, action, done, info )

		# To implement unsupervised RL remove the comment,
		# this menas that the basic reward can be only a binary value 1 or 0
		# if reward != 1: reward = 0

		#
		self.set_state(state)
		return state, reward, done, info

	def override_reward( self, state, reward, action, done, info ):
		# @Raz here you can implement your rules to modify the reward function

		# This first coniditon is to run the code without the rules,
		# to take vanilla data for the comparison
		if not self.rules_active: return reward

		# Rule/Scenario 1:
		#
		# Rule/Scenario 2:
		#
		# Rule/Scenario 2:
		#
		return reward

	def fix_state( self, state ):

		"""
		Support function to convert the observation vector from the version obtained by Unity3D to our configuration.
		The orginal version provide a 2*scan_number size for the LiDAR,
		for each direction 2 value, one with the flaot value and one with the flag [0, 1]. 
		In this	wrapper we remove the flag, maintaining only one value for each direction

		Parameters
		----------
			state : list
				a list of the observation original observations from the environment

		Returns
		----------
			state : list
				a list of the observation, with scan_number + 2 elements, the first part contains the information
				about the ldiar sensor and the second angle and distance in respect to the target. All the values
				are normalized between [0, 1]
		"""

		# Compute the size of the observation array that correspond to the lidar sensor, 
		# the other portion is maintened
		scan_limit = 2 * (self.scan_number)
		state_lidar = [s for id, s in enumerate(state[:scan_limit]) if id % 2 == 1]

		# Change the order of the lidar scan to the order of the wrapper (see the class declaration for details)
		lidar_ordered_1 = [s for id, s in enumerate(reversed(state_lidar)) if id % 2 == 0 ]
		lidar_ordered_2 = [s for id, s in enumerate(state_lidar) if id % 2 == 1 ]
		lidar_ordered = lidar_ordered_1 + lidar_ordered_2

		# Concatenate the ordered lidar state with the other values of the state
		state_fixed = lidar_ordered + list(state[scan_limit:-1])

		#
		return np.array(state_fixed)

	def get_current_state(self):
		self.state_is_ready_to_read = False
		return self.current_state

	def is_current_state_updated(self):
		if(len(self.current_state) > 0 and self.state_is_ready_to_read
		and not self.done):
			return True
		return False

	def set_state(self, state):
		self.current_state = state
		self.state_is_ready_to_read = True

	# Override the "close" function
	def close( self ): self.env.close()

	# Override the "render" function
	def render( self ):	pass

