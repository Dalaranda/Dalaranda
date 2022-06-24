using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PuzzleSpawner1 : MonoBehaviour
{
    public GameObject[] spawnees;
    public Transform spawnPos;

    public float boxSpawnTimer = 2.0f;

    int randomInt;

    public int x = 1;

    private GameObject instantiatedObj;

    public ClimbRoomButtonScript startTime;

    private void Start()
    {
        startTime = GameObject.Find("ClimbRoomButton").GetComponent<ClimbRoomButtonScript>();
    }

    void Update()
    {
        if (x == 0)
        {
            Destroy(instantiatedObj);
            boxSpawnTimer = 2.0f;
            SpawnRandom();
            EndSpawn();
        }

        boxSpawnTimer -= Time.deltaTime;
        if (boxSpawnTimer < 0 && startTime.isTimer2Running == true)
        {
            startSpawn();
        }
        if(startTime.isTimer2Running == false)
        {
            Destroy(instantiatedObj);
        }
    }

    void SpawnRandom()
    {
        randomInt = Random.Range(0, spawnees.Length);
        instantiatedObj = (GameObject) Instantiate(spawnees[randomInt], spawnPos.position, spawnPos.rotation);
    }

    public void startSpawn()
    {
        x = 0;
    }

    public void EndSpawn()
    {
        x = 1;
    }
}
