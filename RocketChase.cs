using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RocketChase : MonoBehaviour
{
    public GameObject rocket;
    public PlayerCharacter playerChar;
    public GameObject player;
    public Target targetSelf;

    private GameObject wayPoint;
    private Vector3 wayPointPos;


    private float speed = 6.0f;
    void Start()
    {
        player = GameObject.Find("PlayerCharacter");
        wayPoint = GameObject.Find("wayPoint");
    }

    // Update is called once per frame
    void Update()
    {
        wayPointPos = new Vector3(wayPoint.transform.position.x, wayPoint.transform.position.y, wayPoint.transform.position.z);

        transform.position = Vector3.MoveTowards(transform.position, wayPointPos, speed * Time.deltaTime);
    }

    private void OnTriggerEnter(Collider other)
    {
        if(other.tag == "Player")
        {                          
          PlayerCharacter target = other.transform.GetComponent<PlayerCharacter>();
          target.TakeDamage(10);
          targetSelf.TakeDamage(10);               
        }
    }
}
