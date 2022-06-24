using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCharacter : MonoBehaviour
{

    public int maxHealth = 100;
    public int currentHealth;

    public GameObject wayPoint;
    public GameObject rocket;
    public GameObject endGameScreen;

    public GameObject maincamera;
    public GameObject playercharacter;

    private MouseLook look;
    private PlayerMovement move;

    private float timer = 0.1f;

    public HealthBar healthBar;

    public PlayerCharacter player;

    public Target target;


    void Start()
    {
        currentHealth = maxHealth;
        healthBar.SetMaxHealth(maxHealth);

        look = maincamera.GetComponent<MouseLook>();
        move = player.GetComponent<PlayerMovement>();
    }

    void Update()
    {
        if(Input.GetKeyDown(KeyCode.J))
        {
            TakeDamage(20);
        }

        if(currentHealth <= 0)
        {
            death();
        }

        if(timer > 0)
        {
            timer -= Time.deltaTime;
        }
        if(timer <= 0)
        {
            UpdatePosition();
            timer = 0.1f;
        }
    }

    public void TakeDamage(int damage)
    {
        currentHealth -= damage;

        healthBar.SetHealth(currentHealth);
    }  

    void UpdatePosition()
    {
        wayPoint.transform.position = transform.position;
    }

    void death()
    {
        look.isDead = true;
        move.isDead = true;
        Cursor.visible = true;
        Cursor.lockState = CursorLockMode.None;
        endGameScreen.SetActive(true);
    }

}
