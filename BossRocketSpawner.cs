using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossRocketSpawner : MonoBehaviour
{
    public GameObject[] spawnees;
    public Transform spawnPos;

    public float waveSpawner = 5.0f;

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
            waveSpawner = 5.0f;
            SpawnRandom();
            EndSpawn();
        }

        waveSpawner -= Time.deltaTime;
        if (waveSpawner < 0 && startTime.isTimer3Running == true)
        {
            startSpawn();
        }
        if(startTime.isTimer3Running == false)
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
