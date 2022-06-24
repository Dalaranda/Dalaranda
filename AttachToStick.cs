using System.Collections;
using System.Collections.Generic;
using UnityEngine.XR.Interaction.Toolkit;
using UnityEngine;
using System;

public class AttachToStick : MonoBehaviour
{
    public GameObject AttachVolume;
    public GameObject Self;
    public GameObject HockeyStick;

    public GameObject HockeyStickAttachSocket;
    public GameObject DefaltAttachSocket;

    public GameObject ResetLocationObject;
    public GameObject LocationToSpawner;

    public BoxCollider StickCollider;
    public BoxCollider OutofBounds;

    public bool canStickSnap = false;
    public bool canResetPuckLocation = true;

    // Start is called before the first frame update
    void Start()
    {
        Self = gameObject;

       
        //StickCollider = HockeyStick.Find("Collider2").GetComponent<BoxCollider>();
        // HockeyStick = GameObject.Find("Hockeystick");
    }

    void OnTriggerEnter(Collider col)
    {
        if(col == StickCollider)
        {           
            canStickSnap = true;
        }

        if(col == OutofBounds)
        {
            ResetLocation();
            Debug.Log("Out of Bounds");
        }
    }

    void OnTriggerExit(Collider col)
    {
        if(col == StickCollider)
        {
            canStickSnap = false;

            Rigidbody rb = gameObject.GetComponent<Rigidbody>();
            Vector3 v3Velocity = rb.velocity;


            if (v3Velocity.magnitude >= 1)
            {
                gameObject.GetComponent<AudioSource>().Play();
            }
            if(v3Velocity.magnitude <= -1)
            {
                gameObject.GetComponent<AudioSource>().Play();
            }
        }
    }

    public void attachToStick()
    {
        if(canStickSnap == true)
        {
            if (HockeyStick.GetComponent<XRSocketInteractor>().socketActive == true)
            {
                HockeyStick.GetComponent<XRSocketInteractor>().socketActive = false;
            }
            else
            {
                HockeyStick.GetComponent<XRSocketInteractor>().socketActive = true;
            }
        }
        else
        {
            HockeyStick.GetComponent<XRSocketInteractor>().socketActive = false;
        }
    }

    public void ResetLocation()
    {
        gameObject.transform.position = ResetLocationObject.transform.position;
        gameObject.GetComponent<Rigidbody>().velocity = Vector3.zero;
        gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
        LocationToSpawner.GetComponent<AudioSource>().Play();
    }

    public void LocationToSpawnerFunc()
    {
        if (canResetPuckLocation == true)
        {
            gameObject.transform.position = LocationToSpawner.transform.position;
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
