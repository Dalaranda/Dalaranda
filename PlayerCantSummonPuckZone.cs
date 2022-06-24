using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCantSummonPuckZone : MonoBehaviour
{
    public SphereCollider Player;
    public GameObject Puck;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    void OnTriggerEnter(Collider col)
    {
        if (col == Player)
        {
            Debug.Log("PlayerDetected");
            Puck.GetComponent<AttachToStick>().canResetPuckLocation = false;
        }
       
    }

    void OnTriggerExit(Collider col)
    {
        if (col == Player)
        {
            Puck.GetComponent<AttachToStick>().canResetPuckLocation = true;
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
