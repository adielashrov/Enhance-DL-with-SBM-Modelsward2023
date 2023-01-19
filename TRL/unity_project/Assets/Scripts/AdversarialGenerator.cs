using UnityEngine;
using Unity.MLAgents.Sensors;
using System.Linq;


public class AdversarialGenerator : MonoBehaviour {


	// 
	public TextAsset lidarConfigurationFile; 
	public int numberOfInput = 9;
	public bool clockwise = true;

	//
	private float[] initialLidar;
	private float[] additionalLidar;
	public float[] completeLidar;

	//
	private float initialAngle;
	private float initialDistance;
	private float startingAngle;

	// Prefab of the obstacle, a minimal cylinder 
	public GameObject obstaclePrefab;
	public GameObject targetObject;
	public GameObject agentObject;

	// Information about the normalization of the input values
	// to recreate the original values
	public float distanceNormFactor;
	private float angularStep;


	void Start() {

		// Disable the randomization of the target for the adversarial example
		agentObject.GetComponent<CustomAgent>().randomizeTarget = false;
		// Disable the randomization of the agent initial rotation and position
		agentObject.GetComponent<CustomAgent>().randomizeAgentRotation = false;
		agentObject.GetComponent<CustomAgent>().randomizeAgentPosition = false;
		// Read the distance (and angle) normalization factor from the agent
		angularStep = agentObject.GetComponent<CustomAgent>().angularStep;
		distanceNormFactor = agentObject.GetComponent<CustomAgent>().distanceNormFact;	

		//  Compute the initial value of the y axis for the obstacle,
		// to instiantiate in the correct position
		float basicY = obstaclePrefab.transform.position.y;
		// Computing the size of the obstacle, necessary to compute an offset for the 
		// scan, correction of the position 
		float obstacleScale = obstaclePrefab.transform.localScale.x / 2;
		// Find the agent to obtain the length of the lidars scan
		float scanSize = agentObject.GetComponent<RayPerceptionSensorComponent3D>().RayLength;
		// Computing the scale of the agent, necessary to obtain real length
		// of the scan, the length of the scan is so: agentScale * scanSize
		scanSize = scanSize * agentObject.transform.localScale.x;		
		
		// 
		readFIle();

		// Computation of the position for the target given the input at the time T1
		// De-normalization of the values to recreate the map
		// 		normalization formula -> angle = 0.5f - (angle / 360f);
		//float denormalizedAngle = 180f - 360f * initialAngle + 90f;
		float denormalizedAngle = -(initialAngle - 0.5f) * 360f + 90f;

		float dedenormalizeDistance = distanceNormFactor * initialDistance;
		// Computation of the new position for the target based on angle e distance
		float newTargetX = Mathf.Cos( denormalizedAngle * Mathf.Deg2Rad ) * dedenormalizeDistance;
		float newTargetZ = Mathf.Sin( denormalizedAngle * Mathf.Deg2Rad ) * dedenormalizeDistance;
		Vector3 newTargetPos = new Vector3(newTargetX, targetObject.transform.position.y, newTargetZ);
		// Actual changing in the position of the target
		targetObject.transform.position = newTargetPos;

		foreach( float scanVal in completeLidar){
			// Check, if a value of the scan is lower than 1 so i need to put an obstacle
			if (scanVal < 1) {
				float newX = Mathf.Cos( startingAngle * Mathf.Deg2Rad ) * (scanSize * scanVal + obstacleScale);
				float newZ = Mathf.Sin( startingAngle * Mathf.Deg2Rad ) * (scanSize * scanVal + obstacleScale);
				Vector3 newPosition = new Vector3( newX, basicY, newZ );
				Instantiate(obstaclePrefab, newPosition, Quaternion.identity, this.transform);
			}
			startingAngle -= angularStep;
		}

	}

	private void readFIle() {

		string fileText = lidarConfigurationFile.text;
		int timeStep = fileText.Split().Length / numberOfInput;
		if( timeStep >= (int)(angularStep/4) ) timeStep = (int)(angularStep/4)-1;

		initialLidar = new float[numberOfInput-2];
		additionalLidar = new float[timeStep-1];

		for( int i=0; i<numberOfInput-2; i++ ) 
			initialLidar[i] = float.Parse(fileText.Split()[i]);
		

		if( clockwise ) {
			
			for (int i=0; i<timeStep-1; i++) {
				int id = numberOfInput+(i+1)*numberOfInput-3;
				additionalLidar[i] = float.Parse(fileText.Split()[id]);
			}
			startingAngle = 180f;
			completeLidar = initialLidar.Concat(additionalLidar).ToArray();

		} else {

			for (int i=0; i<timeStep-1; i++) {
				int id = (i+1)*numberOfInput;
				additionalLidar[i] = float.Parse(fileText.Split()[id]);
			}
			startingAngle = 180f + (timeStep-1) * angularStep;

			System.Array.Reverse(additionalLidar);
			completeLidar = additionalLidar.Concat(initialLidar).ToArray();

		}

		initialAngle = float.Parse(fileText.Split()[numberOfInput-2]);
		initialDistance = float.Parse(fileText.Split()[numberOfInput-1]);
	}

	


}