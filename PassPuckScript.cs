using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PassPuckScript : MonoBehaviour
{
    public float timerCounter = 2;
    public bool timerIsRunning = false;

    public GameObject Puck;
    public GameObject Player;
    public GameObject propPuck;
    public GameObject Light;
    public GameObject PropZone;

    public Material greenMat;
    public Material redMat;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    void OnTriggerEnter(Collider col)
    {
        if (col.gameObject.tag == "Puck")
        {
            propPuck.SetActive(true);
            PropZone.GetComponent<MeshRenderer>().material = greenMat;
            Light.GetComponent<Light>().color = Color.green;            
            col.gameObject.GetComponent<AttachToStick>().ResetLocation();
            Player.GetComponent<PlayerScript>().CurrentScore += 1;
            Player.GetComponent<PlayerScript>().RoundScore += 1;
            gameObject.GetComponent<BoxCollider>().enabled = false;
            Player.GetComponent<AudioSource>().Play();
            timerIsRunning = true;
        }

    }

    public void ResetObjects()
    {
        propPuck.SetActive(false);
        PropZone.GetComponent<MeshRenderer>().material = redMat;
        Light.GetComponent<Light>().color = Color.red;
        gameObject.GetComponent<BoxCollider>().enabled = true;
        timerCounter = 2;
    }
    // Update is called once per frame
    void Update()
    {
        if (timerIsRunning)
        {
            if(timerCounter > 0)
            {
                timerCounter -= Time.deltaTime;
            }
            if(timerCounter <= 0)
            {
                ResetObjects();
                timerIsRunning = false;
            }
        }
    }
}
