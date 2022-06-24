using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TargetScript : MonoBehaviour
{



    public GameObject Player;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    void OnTriggerEnter(Collider col)
    {
        if (col.gameObject.tag == "Puck")
        {
            Debug.Log("Works");
            Player.GetComponent<PlayerScript>().CurrentScore += 3;
            Player.GetComponent<PlayerScript>().RoundScore += 3;
            col.GetComponent<AttachToStick>().ResetLocation();
            Player.GetComponent<AudioSource>().Play();
            gameObject.SetActive(false);
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
