using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class GoalScript : MonoBehaviour
{



    public GameObject Player;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    void OnTriggerEnter(Collider col)
    {
        if(col.gameObject.tag == "Puck")
        {
            Debug.Log("Works");
            Player.GetComponent<PlayerScript>().CurrentScore += 1;
            Player.GetComponent<PlayerScript>().RoundScore += 1;
            col.GetComponent<AttachToStick>().ResetLocation();
            Player.GetComponent<AudioSource>().Play();
        }
    }

        // Update is called once per frame
        void Update()
    {
        
    }
}
