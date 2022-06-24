using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DeathZone : MonoBehaviour
{

    public GameObject player;
    public Transform respawnPoint;

    private bool playerIsoverlapping = false;

    void Update()
    {
        if(playerIsoverlapping == true)
        {
            player.transform.position = respawnPoint.position;
           
            CharacterController cc = player.GetComponent<CharacterController>();
            cc.enabled = false;

            player.transform.position = respawnPoint.position;

            playerIsoverlapping = false;

            cc.enabled = true;
        }
    }
    private void OnTriggerEnter(Collider other)
    {
        if(other.tag == "Player")
        {
            playerIsoverlapping = true;           
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if(other.tag == "Player")
        {
            playerIsoverlapping = false;
        }
    }
}
