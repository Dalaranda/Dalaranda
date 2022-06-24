using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BackpackScript : MonoBehaviour
{
    public GameObject LeftHand;
    public GameObject RightHand;
    public GameObject PlayerdataHolder;

    public GameObject Tablet;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    void OnTriggerEnter(Collider col)
    {
        if (col.gameObject.tag == "LeftHand")
        {
            PlayerdataHolder.GetComponent<PlayerScript>().LeftHandCanGrabTablet = true;
        }

        if (col.gameObject.tag == "RightHand")
        {
            PlayerdataHolder.GetComponent<PlayerScript>().RightHandCanGrabTablet = true;
        }

        if (col.gameObject.tag == "Tablet")
        {

        }
    }

    void OnTriggerExit(Collider col)
    {
        if (col.gameObject.tag == "LeftHand")
        {
            PlayerdataHolder.GetComponent<PlayerScript>().LeftHandCanGrabTablet = false;
        }

        if (col.gameObject.tag == "RightHand")
        {
            PlayerdataHolder.GetComponent<PlayerScript>().RightHandCanGrabTablet = false;
        }

        if (col.gameObject.tag == "Tablet")
        {

        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
