using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DoorScript : MonoBehaviour
{
    private Animator doorAnim;

    public bool doorOpen = false;
    public float doorTimeLeft = 5.0f;

    private void Awake()
    {
        doorAnim = gameObject.GetComponent<Animator>();
    }

    public void openDoor()
    {
        if (!doorOpen)
        {
            doorTimeLeft = 5;
            doorAnim.Play("DoorOpen", 0, 0.0f);
            doorOpen = true;
        }
        else
        {
            doorTimeLeft = 5;
            doorAnim.Play("DoorClose", 0, 0.0f);
            doorOpen = false;
        }
    }   

    void Update()
    {
        doorTimeLeft -= Time.deltaTime;
        if(doorTimeLeft < 0 && doorOpen == true)
        {
            openDoor();
        }
       
    }

}
